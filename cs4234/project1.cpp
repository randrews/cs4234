#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int** readMatrixFile(string matfile,int& numrows,int& numcols);
int* readPermutationFile(string permutefile);
void distributeBlocks(string matfile,string permutefile,int** &rows,int* &perm,int& numrows,int& numcols);
void receiveBlocks(int** &rows,int* &perm,int& numrows,int& numcols);
void getBlockBounds(int& lower,int& upper,int numrows);
int getRowOwner(int row,int numrows);

int main(int argc, char **argv){
  MPI_Init(&argc, &argv); // Init MPI

  string matfile,permutefile; // Names of the files
  int myrank; // The rank of this process

  int **rows,*perm; // This process' rows and the entire permutation vector
  int numrows,numcols; // The dimensions of the matrix

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); // Figure out which proc I am

  if(argc<3) // Check command-line args
    {
      if(myrank==0) // Only output an error if I'm the root;
	cout<<"Usage: "<<argv[0]<<" <matfile> <permutefile>\n";
      return 1;
    }

  matfile=argv[1]; // Command-line args
  permutefile=argv[2];


  if(myrank==0) // If I'm the root, then read the file and pass out rows
    distributeBlocks(matfile,permutefile,rows,perm,numrows,numcols);
  else // If I'm not, then receive my block of the rows from root.
    receiveBlocks(rows,perm,numrows,numcols);


  int lower,upper;
  getBlockBounds(lower,upper,numrows);

  MPI_Request req; // I don't know what this is, but Isend wants it.
  MPI_Status stat; // I don't know what this is, but Recv wants it.

  for(int n=lower;n<=upper;n++)
    {
      int owner=0;while(perm[owner]!=n)owner++;
      if(getRowOwner(owner,numrows)!=myrank) // If I'm not the owner of a row I have
	{
	  MPI_Isend(rows[n],numcols,MPI_INT,getRowOwner(owner,numrows),n,MPI_COMM_WORLD,&req); // Send it to its owner
#ifdef DEBUG
	  cout<<myrank<<" sending row "<<n<<" to "<<getRowOwner(owner,numrows)<<'\n';
#endif
	}
    }

  for(int n=lower;n<=upper;n++)
    {
      if(getRowOwner(perm[n],numrows)==myrank)continue; // Don't receive blocks I already have
      rows[perm[n]]=new int[numcols]; // Allocate space for the new row
#ifdef DEBUG
      cout<<myrank<<" receiving row "<<perm[n]<<" from "<<getRowOwner(perm[n],numrows)<<'\n';
#endif
      MPI_Recv(rows[perm[n]],numcols,MPI_INT,getRowOwner(perm[n],numrows),perm[n],MPI_COMM_WORLD,&stat);
    }

  int** output=new int*[numrows]; // This part does the real work. We now have all the rows we need
  for(int n=lower;n<=upper;n++) // For each row in my block
    {
      output[n]=rows[perm[n]]; // Put the pointer to the input row into the output row set, permuted.
    }

  if(myrank==0 && upper!=numrows-1) // Clause for if I'm the only process
    {
      for(int n=upper+1;n!=numrows;n++) // Receive output rows that are after my block from anyone who cares to send them
	{
	  output[n]=new int[numcols];
	  MPI_Recv(output[n],numcols,MPI_INT,MPI_ANY_SOURCE,n,MPI_COMM_WORLD,&stat);
	}
    }
  else
    {
      for(int n=lower;n<=upper;n++) // I'm not root, so send root my output block
	MPI_Isend(output[n],numcols,MPI_INT,0,n,MPI_COMM_WORLD,&req);
    }


  if(myrank==0) // Only root does output
    for(int y=0;y!=numrows;y++) // Double loop, print the matrix.
      {
	for(int x=0;x!=numcols;x++)
	  cout<<output[y][x]<<'\t';
	cout<<'\n';
      }

  MPI_Finalize(); // End MPI.
  return 0;
}





void receiveBlocks(int** &rows,int* &perm,int& numrows,int& numcols){
  MPI_Status stat; // I don't know what it's for, but Recv wants it
  int dims[2]; // A place to store the matrix dimensions
  MPI_Bcast(dims,2,MPI_INT,0,MPI_COMM_WORLD); // Receive the dimensions
  numrows=dims[0];
  numcols=dims[1];

  perm=new int[numrows]; // The permutation vector
  MPI_Bcast(perm,numrows,MPI_INT,0,MPI_COMM_WORLD); // Receive the permutation vector

  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

  if(myrank>=numrows)exit(0); // There's nothing for this to do in that case.

  int lower,upper;
  getBlockBounds(lower,upper,numrows);
  rows=new int*[numrows];

  for(int n=lower;n<=upper;n++)
    {
      rows[n]=new int[numcols];
      MPI_Recv(rows[n],numcols,MPI_INT,0,n,MPI_COMM_WORLD,&stat);
    }

}





void distributeBlocks(string matfile,string permutefile,int** &rows,int* &perm,int& numrows,int& numcols){
  MPI_Request req; // I don't know what it's for, but Isend wants it

  rows=readMatrixFile(matfile,numrows,numcols);
  perm=readPermutationFile(permutefile);

  int dims[2]; // Broadcast the dimensions of the matrix to everyone
  dims[0]=numrows; // After packing them into a cute array
  dims[1]=numcols;
  MPI_Bcast(dims,2,MPI_INT,0,MPI_COMM_WORLD);

  MPI_Bcast(perm,numrows,MPI_INT,0,MPI_COMM_WORLD);

  for(int n=0;n!=numrows;n++) // Send out the row blocks (asynch so we don't deadlock with ourself)
    {
      if(getRowOwner(n,numrows)!=0)
	MPI_Isend(rows[n],numcols,MPI_INT,getRowOwner(n,numrows),n,MPI_COMM_WORLD,&req);
    }
}




int* readPermutationFile(string permutefile){
  int numrows,numcols;
  int* permute;
  int file;

  file=open(permutefile.c_str(),O_RDONLY,0644); // Open the permutation file
  read(file,&numrows,4); // Read in the file size
  read(file,&numcols,4); // Read in the number of columns, which we don't care about.
  permute=new int[numrows]; // Allocate space fo rthe vector
  read(file,permute,4*numrows); // Read in the actual vector
  close(file);
  for(int n=0;n!=numrows;n++) // The rows in the program are indexed from 0, in the input from 1.
    permute[n]--;
  return permute;
}


int** readMatrixFile(string matfile,int& numrows,int& numcols){
  int matrix,permute; // File descriptors

  matrix=open(matfile.c_str(),O_RDONLY,0644); // Open the matrix file

  read(matrix,&numrows,4); // Read the dimensions
  read(matrix,&numcols,4);

  int** rows=new int*[numrows]; // Allocate space to store the matrix
  for(int n=0;n!=numrows;n++)
    {
      rows[n]=new int[numcols]; // Allocate a row
      read(matrix,rows[n],4*numcols);
    }

  close(matrix); // We're done; close the file
  return rows;
}








void getBlockBounds(int& lower,int& upper,int numrows){
  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); // Figure out which proc I am
  int numprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // How many are there?
  if(numprocs>numrows)numprocs=numrows;

  lower=myrank*numrows/numprocs;
  if(myrank!=numprocs-1)upper=(myrank+1)*numrows/numprocs-1;
  else upper=numrows-1;
}








int getRowOwner(int row,int numrows){
  int numprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  if(numprocs>numrows)numprocs=numrows;
  return (numprocs*(row+1)-1)/numrows;
}
