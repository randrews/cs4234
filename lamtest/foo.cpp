#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  int myrank;

  /* Initialize MPI */

  MPI_Init(&argc, &argv);

  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  cout<<myrank<<'\n';

  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}
