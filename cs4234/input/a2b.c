
//
// usage: a2b infile outfile
//
// infile: ascii file defining a matrix of integers
// outfile: binary file defining same matrix
//

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

  FILE *infileptr;  
  FILE *outfileptr;  
  int i, j, m, n, data;
  int dsize = sizeof(int);

  if (argc != 3) {
     printf("usage: a2b infile outfile\n");
     exit(1);
  }

  infileptr = fopen(argv[1], "r");
  if (infileptr == NULL) {
     printf("Problem opening %s\n", argv[1]);
     exit(1);
  }

  outfileptr = fopen(argv[2], "w");
  if (outfileptr == NULL) {
     printf("Problem opening %s\n", argv[2]);
     exit(1);
  }

  fscanf(infileptr, "%d %d", &m, &n);
  fwrite(&m, dsize, 1, outfileptr);
  if (n>1) fwrite(&n, dsize, 1, outfileptr);  /* if n=1, assume this is a vector */

  for (i=0; i<m; i++)
     for (j=0; j<n; j++) {
        fscanf(infileptr, "%d", &data);
        fwrite(&data, dsize, 1, outfileptr);
     }
  
  fclose(infileptr);
  fclose(outfileptr);
}
