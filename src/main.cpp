#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <chrono>

#include <cmath>

#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS //Enable OpenCL exceptions
#include <CL/cl.hpp>

int main(int argc, char** argv) {
    
	/**************************************************************************\
    *                         read cli arguments                               *
    \**************************************************************************/

	int N, M; // rows, cols
    bool zol; // use 0-1 sorting lemma, initialize matrix to 1s and 0s
    bool sph; // print array in each phase
    
	N = M = 0;
    sph = zol = false;

    for(int i=1; i<argc; i++) {
		if(!strcmp(argv[i], "-n")) {
			N = atoi(argv[++i]);
		} else if(!strcmp(argv[i], "-m")) {
			N = atoi(argv[++i]);
		} else if(!strcmp(argv[i], "-zol")) {
            zol = true;
        } else if(!strcmp(argv[i], "-sph")) {
            sph = true;
        } else if(!strcmp(argv[i], "-h")) {
            std::cout << "shearsort [options] \n\t";
            std::cout << "-n N : define rows of matrix\n\t";
            std::cout << "-m M : define cols of matrix\n\t";
            std::cout << "-zol : use 0-1 sorting lemma\n\t";
            std::cout << "-sph : print array in each phase\n\t";
            std::cout << "-h   : show this help screen\n";
            return 0;
        } else {
            std::cout << "wrong arguments\nshearsort -h for help" << std::endl;
            return -1;
        }
	}
	
	/* if N = 0, default 4 */
	if(N == 0) {
		N = 4;
	}
	
	/* if M = 0, array is square NxN */
	if(M == 0) {
		M = N;
	}

    /**************************************************************************\
    *                        Declare Variables                                 *
    \**************************************************************************/

	unsigned seed;
    bool rows;
	
	std::default_random_engine generator (seed);
	std::uniform_int_distribution<int> distribution(0,((zol)?1:99));

    cl_int **a, sorted;
	cl::vector< cl::Platform > platforms;
	cl_context_properties cprops[3];
	cl::Buffer outCL;
	cl::vector<cl::Device> devices;		

	sorted = 0;
	rows = true;
	seed = std::chrono::system_clock::now().time_since_epoch().count();

	try {
	
	    /**************************************************************************\
   		*                        Data Initialization                               *
  	  	\**************************************************************************/
		/* create matrix */
		cl_int *buffer;
		
		a = new cl_int*[N];
		buffer = new cl_int[N*M];
		for(int i=0; i<N; i++) {
			a[i] = buffer + (i*M);
		}
		
		/* initialize matrix with random numbers */
		for(int i=0; i<N; i++) {
			for(int j=0; j<M; j++) {
				a[i][j] = distribution(generator);
			}
		}
		
		/* display initial matrix */
		std::cout << "A : \n";
		for(int i=0; i<N; i++) {
			for(int j=0; j<M; j++) {
				std::cout << std::setw(2) << a[i][j] << " ";
			}
			std::cout << "\n";	
		}
		
	    /**************************************************************************\
		*                        System Initialization                             *
 		\**************************************************************************/
		/* get opencl platforms */
		cl::Platform::get(&platforms);
		
		/* create opencl context */
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM,
            (cl_context_properties)(platforms[0])(), 0};

	    cl::Context context(CL_DEVICE_TYPE_GPU, properties); 
        devices = context.getInfo<CL_CONTEXT_DEVICES>();
        
        /* create command queue */
		cl::CommandQueue queue(context, devices[0], 0);
		
		/* create event */
	    cl::Event event;
        
   		/* create device buffers */
  		cl::Buffer buffer_a(context, CL_MEM_READ_WRITE, N * M * sizeof(cl_int));

	    /* transfer matrix to device */
		queue.enqueueWriteBuffer(buffer_a, CL_TRUE, 0,
                N * M * sizeof(cl_int), a[0]);
	    
	    /* load kernels from file to string */
		std::ifstream file("src/shearsort.cl");
		std::string prog(std::istreambuf_iterator<char>(file),
                (std::istreambuf_iterator<char>()));
	    
	    /* compile kernels from sources */
		cl::Program::Sources source( 1, std::make_pair(prog.c_str(),
                    prog.length()+1) );
		cl::Program program(context, source);
		program.build(devices);

		cl::Kernel odd_even_row(program, "odd_even_row");
 		odd_even_row.setArg(0, buffer_a);
		odd_even_row.setArg(1, M);
		
		cl::Kernel odd_even_col(program, "odd_even_col");
 		odd_even_col.setArg(0, buffer_a);
		odd_even_col.setArg(1, M);
		
		/**************************************************************************\
		*                   	  Shearsort Algorithm                              *
 		\**************************************************************************/	
		for(int i=0; i<log(N*M)/log(2) + 1; i++) {
			if(rows) {
				for(int j=0; j<N; j+=2) {
					// sort rows with odd even
					odd_even_row.setArg(2, 1);
					queue.enqueueNDRangeKernel(odd_even_row, cl::NullRange,
                        cl::NDRange(M/2 - 1, N), cl::NDRange(M/2 - 1, N), NULL, &event);
					event.wait();
					
					odd_even_row.setArg(2, 0);
					queue.enqueueNDRangeKernel(odd_even_row, cl::NullRange,
                        cl::NDRange(M/2,N), cl::NDRange(M/2,N), NULL, &event);
					event.wait();
				}				
		    } else {
   				for(int j=0; j<M; j+=2) {
				    // sort cols with odd even
   					odd_even_col.setArg(2, 1);
					queue.enqueueNDRangeKernel(odd_even_col, cl::NullRange,
                        cl::NDRange(M, N/2 - 1), cl::NDRange(M, N/2 - 1), NULL, &event);
				    event.wait();
				 
   					odd_even_col.setArg(2, 0);
					queue.enqueueNDRangeKernel(odd_even_col, cl::NullRange,
                        cl::NDRange(M,N/2), cl::NDRange(M,N/2), NULL, &event);
				    event.wait();
				}
			}
			
			rows = !rows;

            /* display sorted matrix */
            if(sph) {
    	        /* transfer results back to host */
        		queue.enqueueReadBuffer(buffer_a, CL_TRUE, 0, N * M * sizeof(cl_int), a[0]);

		        std::cout << "A (phase " << i + 1 << ") : \n";
        		for(int i=0; i<N; i++) {
		        	for(int j=0; j<M; j++) {
				        std::cout << std::setw(2) << a[i][j] << " ";
			        }
			        std::cout << "\n";	
		        }
		    }
        }
		    
		/**************************************************************************\
		*                   	  Gather Results                                   *
 		\**************************************************************************/
    	/* transfer results back to host */
		queue.enqueueReadBuffer(buffer_a, CL_TRUE, 0, N * M * sizeof(cl_int), a[0]);


		/* display sorted matrix */
		std::cout << "A sorted : \n";
		for(int i=0; i<N; i++) {
			for(int j=0; j<M; j++) {
				std::cout << std::setw(2) << a[i][j] << " ";
			}
			std::cout << "\n";	
		}


		/**************************************************************************\
		*                   	 	 Cleanup System                                *
 		\**************************************************************************/
		/* free arrays */
		if(a) {
			if(a[0]) {
				delete(a[0]);
			}
			delete(a);
		}

	} catch (std::exception &e) {
		std::cerr << "Exception : " << e.what() << std::endl;
		
		/* free arrays */
		if(a) {
			if(a[0]) {
				delete(a[0]);
			}
			delete(a);
		}
	}

    return EXIT_SUCCESS;
}
