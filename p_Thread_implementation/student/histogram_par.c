#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "histogram.h"

struct pthread_argus
{
block_t *blocks;
int histogm[NNAMES];
int initial;
int final;
};


void* get_histogram_in_parallel(void *ptr)
{ 

struct pthread_argus *par = (struct pthread_argus*)ptr;
char current_word[20] = "";
	int c = 0;


	// loop over every character
	for (int i = par->initial; i < par->final; i++) {
		
                     for (int j = 0; j < BLOCKSIZE; j++) {


			// if the character is an alphabet store it to form a word.
			if(isalpha(par->blocks[i][j])){
				current_word[c++] = par->blocks[i][j];
			}

			// if the character is not an alphabet it is the end of a word.
			// Compare the word with the list of names.
			else {
				current_word[c] = '\0';
				for(int k = 0; k < NNAMES; k++){                                      
                       if(current_word[0] == names[k][0])
                                   {
					   if(!strcmp(current_word, names[k]))
						par->histogm[k]++;
                                   }
       				}
				c = 0;
			}
		}
	}
return NULL;
}



void get_histogram(int nBlocks, block_t *blocks, int* histogram, int num_threads)
{
	
 int i,j;
 int c = 0;
 pthread_t *threads;
 struct pthread_argus *thread_args;



 threads =(pthread_t*) malloc(num_threads * sizeof (*threads));
 thread_args = (struct pthread_argus*) malloc(num_threads * sizeof (*thread_args));
 int block_per_thread = nBlocks / num_threads;


for(i = 0; i < num_threads; i++)
{

    thread_args[i].initial = c;
	c = (i+1)* block_per_thread;
    thread_args[i].final = c;

	if (i == (num_threads-1)) 
	{
    thread_args[i].final = nBlocks;
	}

	for (int a=0; a<NNAMES; a++){

		thread_args[i].histogm[a]=0;
	}

thread_args[i].blocks = blocks;
pthread_create( threads + i, NULL, &get_histogram_in_parallel, thread_args + i );

}

for(j =0 ; j < num_threads ; j++)
{
pthread_join(threads[j] , NULL);
for(i=0; i<NNAMES; i++)
histogram[i] += thread_args[j].histogm[i];
}

free(threads);
free(thread_args);

}
