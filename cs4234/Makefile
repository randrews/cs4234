CC=mpic++
SOURCES=project1.cpp
HEADERS=
OBJECTS=project1.o
DEFINES=

.PHONY: all Makefile

all: project1

project1: ${OBJECTS}
	${CC} ${OBJECTS} -o project1 ${DEFINES}

%.cpp:
	co $@
%.h:
	co $@

%.o: %.cpp ${HEADERS}
	${CC} -g -c $< -o $@ ${DEFINES}

clean:
	rm -f ${OBJECTS} project1 *~

archive:
	tar -czvf project1.tar.gz ${SOURCES} ${HEADERS} Makefile