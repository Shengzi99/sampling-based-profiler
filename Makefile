CC=mpicc
CXX=g++

MPIRUN=mpirun

LIBUWD_DIR=/home/spack/spack/opt/spack/linux-debian11-zen2/gcc-10.2.1/libunwind-1.6.2-irka6man627xtagwc5dloqonj5srhbsa

CFLAGS=-g -O0
LFLAGS=-lunwind -lunwind-x86_64

LIBS+=-L${LIBUWD_DIR}/lib -L.
INCLUDES+=-I${LIBUWD_DIR}/include


SRCS = ./src/FunctionInfo.cpp \
	   ./src/utils.cpp\
	   ./src/sbProf.cpp\

all:test test_mpi

libsbProf.so: ${SRCS}
	${CXX} -fPIC -shared ${LIBS} ${INCLUDES} ${CFLAGS} ${LFLAGS} -o $@ $^

test: example/test.cpp libsbProf.so
	${CC} ${CFLAGS} ${LIBS} ${LFLAGS} -lsbProf -o $@ $<

test_mpi: example/test_mpi.cpp libsbProf.so
	${CC} ${CFLAGS} ${LIBS} -lsbProf -o $@ $<

clean:
	rm -rf  ./libsbProf.so ./test ./test_mpi

# libsbProf.o: ${SRCS}
# 	${CXX} ${INCLUDES} ${CFLAGS} ${LIBS}  ${LFLAGS} -o $@ $^

# test: example/test.cpp libsbProf.o
# 	${CXX} ${CFLAGS} ${LIBS} ${LFLAGS} -o $@ $^

# clean:
# 	rm -rf  ./libsbProf.o ./test

run:
	LD_LIBRARY_PATH="./:$LD_LIBRARY_PATH" ./test

run_mpi:
	# srun --nodes=1 --ntasks=8 ./test
	mpirun -np 4 ./test_mpi