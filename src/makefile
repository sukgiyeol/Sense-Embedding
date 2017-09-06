SCRIPTS_DIR=../scripts
BIN_DIR=../bin

CC = gcc
CFLAGS = -lm -pthread -O2 -Wall -funroll-loops -Wno-unused-result

all: corp2word sense2vec distance distance-pair distance-pair-accuracy sentence-complete word-analogy word-analogy-accuracy

corp2word : corp2word.c
	$(CC) corp2word.c -o ${BIN_DIR}/corp2word $(CFLAGS)
sense2vec : sense2vec.c
	$(CC) sense2vec.c -o ${BIN_DIR}/sense2vec $(CFLAGS)
distance : distance.c
	$(CC) distance.c -o ${BIN_DIR}/distance $(CFLAGS)
distance-pair : distance-pair.c
	$(CC) distance-pair.c -o ${BIN_DIR}/distance-pair $(CFLAGS)
distance-pair-accuracy : distance-pair-accuracy.c
	$(CC) distance-pair-accuracy.c -o ${BIN_DIR}/distance-pair-accuracy $(CFLAGS)
sentence-complete : sentence-complete.c
	$(CC) sentence-complete.c -o ${BIN_DIR}/sentence-complete $(CFLAGS)
word-analogy : word-analogy.c
	$(CC) word-analogy.c -o ${BIN_DIR}/word-analogy $(CFLAGS)
word-analogy-accuracy: word-analogy-accuracy.c
	$(CC) word-analogy-accuracy.c -o ${BIN_DIR}/word-analogy-accuracy $(CFLAGS)
	chmod +x ${SCRIPTS_DIR}/*.sh
clean:
	pushd ${BIN_DIR} && rm -rf corp2word sense2vec distance distance-pair distance-pair-accuracy sentence-complete word-analogy word-analogy-accuracy; popd
