
void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

	// perform loop fusion to transform this loop and parallelize it with OpenMP
	// ABHIJEET - check whether j could be combined and made as the outer loop.
#pragma omp parallel for schedule(static)
	for (int i = 1; i < N; i++) {
  // #pragma omp parallel for schedule(static)
		for (int j = 1; j < N; j++) {
			a[i][j] = 2 * b[i][j];
			d[i][j] = a[i][j] * c[i][j];
		}
	
  //  #pragma omp parallel for schedule(static)
	     for (int j = 1; j < N; j++) 
			c[i][j - 1] = a[i][j - 1] - a[i][j + 1];
		
	}
}
