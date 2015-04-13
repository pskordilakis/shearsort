CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
project(Shearsort)

#need to include FindOPENCL.cmake to
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

#set sources dir
set(SRC src)

message("CMake module path: ${CMAKE_MODULE_PATH}\n")

find_package(OPENCL)

#include_directories(${SOURCE_DIR}/../opencl10)

#message("include directories: \n")
#get_directory_property(includes INCLUDE_DIRECTORIES)
#message("${includes}\n")

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11")        
    #set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

#provide the source path so we can open our .cl file at runtime
add_definitions(-DCL_SOURCE_DIR="${shearsort_SOURCE_DIR}")
#build in debug mode
add_definitions(-g )

#create the executable
SET(EXEC shearsort)
ADD_EXECUTABLE(${EXEC} ${SRC}/main.cpp)

TARGET_LINK_LIBRARIES (${EXEC}
  # ${cllib}
   ${OPENCL_LIBRARIES}
)

#the source files we want to compile into the library
#set (LIBCXXFILES cll.cpp part1.cpp util.cpp)
#set a CMake variable to name the library
#SET(cllib part1)
#ADD_LIBRARY(${cllib} ${LIBCXXFILES})

