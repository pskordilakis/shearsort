__kernel void odd_even_row(__global int *A, int cols, int offset) {
	int x = get_global_id(0);
	int y = get_global_id(1);
	int temp;
		
	if(y%2 == 0) {
		if(A[y * cols + x * 2 + offset] > A[y * cols + x * 2 + 1 + offset]) {
			temp = A[y * cols + x * 2 + offset];
			A[y * cols + x * 2 + offset] = A[y * cols + x * 2 + 1 + offset];
			A[y * cols + x * 2 + 1 + offset] = temp;
		}
	} else {
		if(A[y * cols + x * 2 + offset] < A[y * cols + x * 2 + 1 + offset]) {
			temp = A[y * cols + x * 2 + offset];
			A[y * cols + x * 2 + offset] = A[y * cols + x * 2 + 1 + offset];
			A[y * cols + x * 2 + 1 + offset] = temp;
		}
	}
}

__kernel void odd_even_col(__global int *A, int cols, int offset) {
	int x = get_global_id(0);
	int y = get_global_id(1);
	int temp;
		
	if(A[(y * 2 + offset) * cols + x] > A[(y * 2 + offset + 1) * cols + x]) {
		temp = A[(y * 2 + offset) * cols + x];
		A[(y * 2 + offset) * cols + x] = A[(y * 2 + offset + 1) * cols + x];
		A[(y * 2 + offset + 1) * cols + x] = temp;
	}	
}

