
#include <stdio.h>
#include <mpi.h>
#include "MyMPI.h"

typedef int dtype;
#define MPI_TYPE MPI_INT

int main (int argc, char *argv[]) {
   dtype** a;          /* Doubly-subscripted array */
   dtype*  storage;    /* Local portion of array elements */
   int     id;         /* Process rank */
   int     m;          /* Rows in matrix */
   int     n;          /* Columns in matrix */
   int     p;          /* Number of processes */
   dtype*  pvec;       /* points to permutation vector */
   int     pveclength; /* length of permutation vector */

   MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);

   /* Get matrix */

   read_row_striped_matrix (argv[1], (void *) &a, (void *) &storage, MPI_TYPE, &m, &n, 
		   MPI_COMM_WORLD);

   /* Get permutation vector */

   read_replicated_vector (argv[2], (void *) &pvec, MPI_TYPE, &pveclength, 
		   MPI_COMM_WORLD);

   if (m != pveclength) {
      if (!id) printf("m and pveclength do not match.\n");
      MPI_Finalize();
      exit(1);
   }

   /* begin matrix permutation */


   /* uh .... */


   /* end matrix permutation */

   /* print permutation vector (not needed in final submission) */

   print_replicated_vector ((void *) pvec, MPI_TYPE, m, MPI_COMM_WORLD);

   /* print permuated matrix */

   print_row_striped_matrix ((void **) a, MPI_TYPE, m, n, MPI_COMM_WORLD);

   MPI_Finalize();
}
