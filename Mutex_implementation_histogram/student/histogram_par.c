#include "histogram.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include "names.h"

#define N 10 

struct global_storage{
    
    int *global_hist; // for each thread to do reduction
    char *shared_buffer; // start pointer of the names
    int residual;
    int job_no;
    int num_chunks;
    
}pthread_args;

// mutex for tasks
pthread_mutex_t mutex_locker = PTHREAD_MUTEX_INITIALIZER;

void * WorkerThread(void * arg) {
    int local_hist[N];
    
    for (int i = 0; i < N; i ++)
        local_hist[i] = 0;

    int* m_hist = (int*)arg;

    int count = 0;
    unsigned int start, end;
    // local start and end points
    while(TRUE) {
        pthread_mutex_lock(&mutex_locker);
        count = pthread_args.job_no;
        pthread_args.job_no++;
        pthread_mutex_unlock(&mutex_locker);
        if (count < (pthread_args.num_chunks-1)) {
            start = count * CHUNKSIZE;
            end = (count + 1) * CHUNKSIZE;
        } else if (count == (pthread_args.num_chunks-1)) {
            start = count * CHUNKSIZE;
            end = count * CHUNKSIZE + pthread_args.residual;
        } else
            break;
        char current_word[20] = "";
        int c = 0;
        for (unsigned int i = start; i < end; i++) {
        	if (isalpha(pthread_args.shared_buffer[i])) {
	            current_word[c++] = pthread_args.shared_buffer[i];
	        } else {
	            current_word[c] = '\0';
	            int res = getNameIndex(current_word);
	            if (res != -1)
	                local_hist[res]++;
	            c = 0;
	        }
        }
    }
    
    for (int i=0; i<N; i++)
    {
    	m_hist[i] = local_hist[i];
    }
    
    return NULL;
}

void Producer() {
    int residual = 0;
    while (pthread_args.shared_buffer[(pthread_args.num_chunks-1)*CHUNKSIZE+residual] != TERMINATOR)
        residual += 2;
    if (pthread_args.shared_buffer[(pthread_args.num_chunks-1)*CHUNKSIZE+residual-1] == TERMINATOR)
    	residual -= 1;
    pthread_args.residual = residual;
}

void get_histogram(char *buffer, int* histogram, int num_threads) {
    pthread_args.shared_buffer = buffer;
    pthread_args.job_no = 0;
    pthread_args.global_hist = histogram;
	pthread_args.num_chunks = strlen(buffer) / CHUNKSIZE;
	
	//Allocating threads
    pthread_t *thread;
    thread = (pthread_t*)malloc(num_threads * sizeof(*thread));

	//Histograms for every thread
    int** hist = (int**)malloc(num_threads*sizeof(int*));
    for (int i=0; i<num_threads; i++)
    {
    	hist[i] = (int*)calloc(N,sizeof(int));
    }

	//Creating and joining threads
    for (int i = 0; i < num_threads; i ++) {
        pthread_create(thread+i, NULL, &WorkerThread, hist[i]);
    }
    
    Producer();
    
    for (int i = 0; i < num_threads; i ++)
        pthread_join(thread[i], NULL);
    
    free(thread);

	//Summation of all histograms
    for (int i=0; i<num_threads; i++)
    {
    	for (int j=0; j<N; j++)
    	{
    		histogram[j] += hist[i][j];
    	}
    }
}

