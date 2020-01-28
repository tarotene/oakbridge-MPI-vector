#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <mpi.h>

void filevec(char file[80], const int N, double *v);

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
	double ss = 0.0, *v = 0;
	sprintf(file, "./a2/a2.%d", rank);
	int num = 0;
	{
		FILE *fp = fopen(file, "r");
		fscanf(fp, "%d", &num);

		v = malloc(num * sizeof(double));
		for (i = 0; i < num; ++i)
		{
			fscanf(fp, "%lf", &v[i]);
		}

		fclose(fp);
	}

	// construct global vector from local vectors
	double *vg = 0;
	int ng = 0;
	{
		int *rcounts = calloc(comm_size, sizeof(int));
		MPI_Allgather(&num, 1, MPI_INT, rcounts, 1, MPI_INT, MPI_COMM_WORLD);

		int *displs = calloc(comm_size + 1, sizeof(int));
		displs[0] = 0;
		for (i = 0; i < comm_size; i++)
		{
			displs[i + 1] = displs[i] + rcounts[i];
		}
		ng = displs[comm_size];

		vg = calloc(ng, sizeof(double));
		MPI_Allgatherv(v, num, MPI_DOUBLE, vg, rcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
	}

	// output global vector
	if (rank == 0)
	{
		FILE *fp = fopen("./a2/a2x.all", "w");
		for (i = 0; i < ng; i++)
		{
			fprintf(fp, "vg[%5d] = %10.3f\n", i, vg[i]);
		}
		fclose(fp);
	}

	// compatibility check
	if (rank == 0)
	{
		int is_equal = 1; /* bool alternative */
		double *vg_test = calloc(ng, sizeof(double));
		filevec("./a2/a2x.all.test", ng, vg_test);
		for (i = 0; i < ng; i++)
		{
			if (vg[i] != vg_test[i])
			{
				is_equal = 0;
				break;
			}
		}

		printf((is_equal == 1 ? "OK\n" : "NG\n"));
	}

	MPI_Finalize();
	return 0;
}

void filevec(char file[80], const int N, double *v)
{
	int i = 0; /* counter */

	FILE *fp;
	fp = fopen(file, "r");
	assert(fp != NULL); /* requires assert.h */

	for (i = 0; i < N; ++i)
	{
		fscanf(fp, "%lf", &v[i]);
	}

	fclose(fp);
	return;
}