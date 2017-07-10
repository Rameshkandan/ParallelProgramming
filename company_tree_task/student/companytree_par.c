#include "companytree.h"
#include<omp.h>

void traverse_in_parallel(tree *node)
{

	if (node != NULL) {

#pragma omp task
		traverse_in_parallel(node->right);

#pragma omp task
		traverse_in_parallel(node->left);


		node->work_hours = compute_workHours(node->data);
		top_work_hours[node->id] = node->work_hours;

	}

}

void traverse(tree *node, int numThreads)
{
	omp_set_num_threads(numThreads);

#pragma omp parallel
	{
#pragma omp single
		traverse_in_parallel(node);
	}

}
