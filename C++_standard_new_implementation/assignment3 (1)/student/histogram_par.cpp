#include "histogram.h"
#include "names.h"
#include <future>
#include <thread>
#include <vector>
#include <algorithm>
#include<mutex>

histogram_t count_in_parallel(const std::vector<word_t>& words, int  initial_, int final_)
{
	
	histogram_t histog;
	
	for(int i=0; i<NNAMES; i++)
	histog[i] =0;
	
	for_each(begin(words)+ initial_,begin(words)+final_ ,[&histog](const word_t &single_word )
	{
		int res = getNameIndex(single_word.data());
		if (res != -1)
			histog[res]++;
	});
	return histog;
}

void get_histogram(const std::vector<word_t>& words, histogram_t& histogram, int num_threads)
{

	std::vector<std::future<histogram_t>> histogram_results;
	int initial_, final_;
	int c = 0;
	int word_size = words.size();
	int words_per_thread = word_size / num_threads;
	int rem = word_size % num_threads;


	for (int i = 0; i < num_threads; i++)
	{
		initial_ = c;
		final_ = initial_ + words_per_thread;
		if (i == num_threads - 1)
			final_ = final_ + rem;
		c = final_;


		histogram_results.push_back(std::async(std::launch::async, count_in_parallel, std::ref(words), initial_, final_));
	}
	
	for(int i=0; i< NNAMES; i++)
	histogram[i] = 0;
	
	for (auto &x: histogram_results)
	{
	      histogram_t temp = x.get();
		for (int j = 0; j < NNAMES ; ++j)
	      histogram[j] += temp[j] ;

	}
 }
