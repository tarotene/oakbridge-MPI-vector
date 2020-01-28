#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <mpi.h>

void filevec(char file[80], const int N, double *v);

double squaresum(const int N, double *v);

int main(int argc, char **argv)
{
	int i = 0;			  /* counter */
	char file[80] = {""}; /* variable character array */

	// initialize
	MPI_Init(&argc, &argv);

	int comm_size = 0, rank = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// input local vectors
	sprintf(file, "./a1/a1.%d", rank);
	double v[8] = {0.0};
	filevec(file, 8, v);
	double ss = squaresum(8, v); /* local square-sum */

	// construct global vector from local vectors
	double ss0 = 0.0;
	MPI_Reduce(&ss, &ss0, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); /* global square-sum */

	// compatibility check
	if (rank == 0)
	{
		printf("square sum (4 nodes): %10.0f\n", ss0);

		// global opearation
		double vg[32] = {0.0};
		filevec("./a1/a1x.all", 32, vg);
		double ss1 = squaresum(32, vg);

		printf("square sum (1 node) : %10.0f\n", ss1);
	}

	if (rank == 0)
	{
		printf("two-norm of the input vector: %10.10f\n", sqrt(ss0));
	}

	MPI_Finalize();
	return 0;
}

void filevec(char file[80], const int N, double *v)
{
	int i = 0; /* counter */

	FILE *fp = fopen(file, "r");
	assert(fp != NULL); /* requires assert.h */

	for (i = 0; i < N; ++i)
	{
		fscanf(fp, "%lf", &v[i]);
	}

	fclose(fp);
	return;
}

double squaresum(const int N, double *v)
{
	int i = 0; /* counter */

	double res = 0.0;
	for (i = 0; i < N; ++i)
	{
		res += v[i] * v[i];
	}

	return res;
}