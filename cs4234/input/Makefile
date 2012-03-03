QUINN = /home/staff/ribbens/Quinn/util
qlib = ${QUINN}/libutil.a

.c.o:
	mpicc -c -I${QUINN} $*.c

proj1: proj1.o
	mpicc -o proj1 proj1.o ${qlib}

a2b.o: a2b.c
	cc -c a2b.c

a2b: a2b.o
	cc -o a2b a2b.o

