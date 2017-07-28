SCRIPTS_DIR=../scripts
BIN_DIR=../bin

CC = gcc
CPP = g++
CFLAGS = -lm -pthread -O2 -Wall -funroll-loops -Wno-unused-result

all: word2vec corp2data pair_distance distance

word2vec : word2vec.c
        $(CC) word2vec.c -o ${BIN_DIR}/word2vec $(CFLAGS)
corp2data : corp2data.cpp
        $(CPP) corp2data.cpp -o ${BIN_DIR}/corp2data $(CFLAGS)
pair_distance : pair_distance.c
	$(CC) pair_distance.c -o ${BIN_DIR}/pair_distance$(CFLAGS)
distance : distance.c
	$(CC) distance.c -o ${BIN_DIR}/distance $(CFLAGS)
        chmod +x ${SCRIPTS_DIR}/*.sh
clean:
        pushd ${BIN_DIR} && rm -rf word2vec word2phrase distance word-analogy compute-accuracy; popd