#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>

using namespace std;

int matrix[]={
  5,  2, -1,  7,
  6,  3,  2,  0,
  4,  2,  7,  9
};
int rows=3,cols=4;
int permute[]={3, 1, 2};

int main(int argc,char** argv){
  if(argc!=3){cout<<"Filenames?\n";return 1;}
  string file=argv[1];
  string perm=argv[2];

  int fd=open(file.c_str(),O_WRONLY | O_CREAT,0644);
  write(fd,&rows,4);
  write(fd,&cols,4);
  for(int n=0;n!=rows*cols;n++)
    write(fd,matrix+n,4);
  close(fd);

  fd=open(perm.c_str(),O_WRONLY | O_CREAT,0644);
  write(fd,&rows,4);
  write(fd,&cols,4);
  write(fd,permute,4*rows);
  close(fd);
}
