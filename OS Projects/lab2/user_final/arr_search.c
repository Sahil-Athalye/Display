/******************************************************************************
*
* arr_search.c
*
* This program implements several library function,
* and can be used to illustrate differences between static and dynamic linking.
* It can also be used as a hypothetical workload.
*
* Each iteration of the program's work function
* allocates an array of floats using malloc()
* The array is filled by calculating the square root, with sqrt(),
* of the array's index.
* Then, bsearch() is used to find a value in the array.
* The array is cleared with memset().
* Finally, free() is called to free the allocated memory.
*
* Usage: This program takes a single input describing the number of
*        iterations to run.
*
* Written August 14, 2020 by Marion Sudvarg
******************************************************************************/

#include <stdio.h> //For printf
#include <stdlib.h> //For atoi, malloc, free, bsearch
#include <math.h> //For sqrt
#include <string.h> //For memset

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <paging.h>
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#define ARR_SIZE 262144
#define ARG_ITERATIONS 1
#define NUM_ARGS ( ARG_ITERATIONS + 1 )


static void *
mmap_malloc(int    fd,
            size_t bytes)
{

    void * data;

    data = mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "Could not mmap " DEV_NAME ": %s\n", strerror(errno));
        return NULL;
    }

    return data;
}

double elapsedTime(struct timeval start, struct timeval end ) {
    double elapsed_time;
    const double MSECS_IN_SEC = 1000.0;
    const double USECS_IN_MSEC = 1000.0;

    elapsed_time = (end.tv_sec - start.tv_sec) * MSECS_IN_SEC;
    elapsed_time += (end.tv_usec - start.tv_usec) / USECS_IN_MSEC;
    return elapsed_time;
}


//Compare floating point values in bsearch
int compare_float(const void * f1, const void * f2) {
        return ( *( (float *) f1) - *( (float*) f2) );
}

//Workload for each iteration
int library_calls(int fd) {
    float * values, * value;
    float key;
    int i;
    struct timeval start_mmap, end_mmap, start_bsearch, end_bsearch;

    gettimeofday(&start_mmap, NULL);
    //Allocate float array
    
    values = (float *) mmap_malloc(fd, ARR_SIZE * sizeof(float));
    
    if(!values) return -1;
    
    gettimeofday(&end_mmap, NULL);
    
    gettimeofday(&start_bsearch, NULL);

    //Assign values to array with sqrt()
    for (i=0; i<ARR_SIZE; i++) {
            values[i] = sqrt(i+1); // I bet the segfault is at the i+1
    }

    //Find value in array
    key = sqrt(383);
    value = (float *) bsearch (&key, values, ARR_SIZE, sizeof(float), compare_float);
    gettimeofday(&end_bsearch, NULL);

    //Clear array memory with memset()
    memset(values, 0, ARR_SIZE * sizeof(float));

    printf("Elapsed Time (ms) mmap: %f array alloc + bsearch: %f\n", elapsedTime(start_mmap, end_mmap),
    elapsedTime(start_bsearch, end_bsearch));

    return 0;

}

int main (int argc, char * argv[]) {

    int fd, iterations;

    //Make sure iterations are specified
    if (argc < NUM_ARGS) {
        printf("Usage: %s <iterations>\n", argv[0]);
        return -1;
    }

    iterations = atoi(argv[ARG_ITERATIONS]);

    //Specified iterations must be positive
    if (iterations <= 0) {
        printf("ERROR: Iteration count must be greater than 0!\n");
        return -1;
    }

    fd = open(DEV_NAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Could not open " DEV_NAME ": %s\n", strerror(errno));
        return -1;
    }	

    //Execute workload for specified iterations
    for (unsigned int i = 0; i < iterations; i++) {   
        //Break if allocation fails
        if (library_calls(fd)){
           printf("allocation failed\n");
	   return -1;
	}
    }

    printf("%s completed %d iterations\n", argv[0], iterations);

    return 0;

}
