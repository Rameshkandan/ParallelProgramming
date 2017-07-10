#include "companytree.h"
#include<omp.h>

void traverse_in_parallel(tree *node)
{

	if (node != NULL) {

		if (omp_get_level() < 4)
		{

#pragma omp  parallel sections
			{

#pragma omp section
				traverse_in_parallel(node->right);

#pragma omp section 
				traverse_in_parallel(node->left);
			}
		}

		else
		{
			traverse_in_parallel(node->right);
			traverse_in_parallel(node->left);

		}

		node->work_hours = compute_workHours(node->data);
		top_work_hours[node->id] = node->work_hours;

	}

}

void traverse(tree *node, int numThreads)
{


	omp_set_num_threads(numThreads);
	omp_set_nested(1);
	traverse_in_parallel(node);


}