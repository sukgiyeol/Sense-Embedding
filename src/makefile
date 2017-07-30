SCRIPTS_DIR=../scripts
BIN_DIR=../bin

CC = gcc
CPP = g++
CFLAGS = -lm -pthread -O2 -Wall -funroll-loops -Wno-unused-result

all: corp2data word2vec distance pair_distance
corp2data : corp2word.cpp
	$(CPP) corp2word.cpp -o ${BIN_DIR}/corp2word $(CFLAGS)
word2vec : word2vec.c
	$(CC) word2vec.c -o ${BIN_DIR}/word2vec $(CFLAGS)
distance : distance.c
	$(CC) distance.c -o ${BIN_DIR}/distance $(CFLAGS)
pair_distance : pair_distance.c
	$(CC) pair_distance.c -o ${BIN_DIR}/pair_distance $(CFLAGS)
	chmod +x ${SCRIPTS_DIR}/*.sh
clean:
	pushd ${BIN_DIR} && rm -rf corp2data word2vec distance pair_distance; popd
