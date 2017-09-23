#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> // mac os x
#include <ctype.h>
typedef enum { false, true } bool;
const long long max_size = 2000;         // max length of strings
const long long N = 3;                   // number of closest words
const long long max_w = 50;              // max length of vocabulary entries
bool senseFlag = false , option = false, notVocab = false; 


struct sense_data {
	char *sense, *word;
};

int *hash_array, hash_size;
struct sense_data *list;


int StrNumCompare(const void *a, const void *b) {
	struct sense_data* ptr_a = (struct sense_data*)a;
	struct sense_data* ptr_b = (struct sense_data*)b;
	if (strcmp(ptr_a->sense, ptr_b->sense)<0) return -1;
	else return 1;
}

void ReadWord(char *word, FILE *fin) {
	int a = 0, ch;
	while (!feof(fin)) {
		ch = fgetc(fin);
		if (ch == 13) continue;
		if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
			if (a > 0) {
				if (ch == '\n') ungetc(ch, fin);
				break;
			}
			else continue;
		}
		word[a] = ch;
		a++;
		if (a >= max_w - 1) a--;   // Truncate too long words
	}
	word[a] = 0;
}

int GetHashKey(char *word) {
	unsigned long long a, hash = 0;
	for (a = 0; a < strlen(word); a++) hash = hash * 257 + word[a];
	hash = hash % hash_size;
	return hash;
}

int SearchWord(char *word) {
	unsigned int hash = GetHashKey(word);
	while (1) {
		if (hash_array[hash] == -1) return -1;
		if (!strcmp(word, list[hash_array[hash]].word))
			return hash_array[hash];
		hash = (hash + 1) % hash_size;
	}
	return -1;
}

int SearchSense(char *sense) {
	unsigned int hash = GetHashKey(sense);
	while (1) {
		if (hash_array[hash] == -1) return -1;
		if (!strcmp(sense, list[hash_array[hash]].sense))
			return hash_array[hash];
		hash = (hash + 1) % hash_size;
	}
	return -1;
}


int main(int argc, char **argv)
{
	FILE *vectorFile, *testFile, *outputFile, *korlexFile;
	char bestw[N][max_size], filename[100], ch;
	char **st;
	float dist, len, bestd[N], vec[max_size];
	long long words, size, a, b, c, d, senseSize;
	long long bList[4];
	float *M;
	char *vocab, *main_p, *main_sp;
	char korlex_line[max_size];

	int subjectNum = 0, allQueNum = 0, allRightNum = 0, allCalNum = 0,
		subQueNum = 0, subRightNum = 0, subCalNum = 0;
	//Parameter Check
	if (argc < 3) {
		printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File> -k <Korlex File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		printf("Korlex File is Option. You don't need to use it.\n\n");
		return 0;
	}
	if ( argc == 5 || argc == 7) {
		if (argc == 5) {
			if (strcmp(argv[3], "-o") == 0) {
				option = true;
			}
			else if (strcmp(argv[3], "-s") == 0) {
				senseFlag = true;
				korlexFile = fopen(argv[4], "r");
			}
			else {
				printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File> -k <Korlex File>\n");
				printf("Vector File contains word projections in the Binary Format.\n");
				printf("Output File is Option. You don't need to use it.\n\n");
				printf("Korlex File is Option. You don't need to use it.\n\n");
				return 0;
			}
		}
		else  {
			if (strcmp(argv[3], "-o") == 0 && strcmp(argv[5], "-s") == 0) {
				option = true;
				senseFlag = true;
				korlexFile = fopen(argv[6], "r");
			}
			else {
				printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File> -k <Korlex File>\n");
				printf("Vector File contains word projections in the Binary Format.\n");
				printf("Output File is Option. You don't need to use it.\n\n");
				printf("Korlex File is Option. You don't need to use it.\n\n");
				return 0;
			}
		}
	}

	strcpy(filename, argv[1]);
	vectorFile = fopen(filename, "rb");             //read binary
	if (vectorFile == NULL) {
		printf("<Vector File> not found\n");
		printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	fscanf(vectorFile, "%lld", &words);
	fscanf(vectorFile, "%lld", &size);
	vocab = (char *)malloc(words * max_w * sizeof(char));
	M = (float *)malloc(words * size * sizeof(float));
	if (M == NULL) {
		printf("Cannot allocate memory: %lld MB\n", words * size * sizeof(float) / 1048576);
		return -1;
	}
	for (b = 0; b < words; b++) {
		fscanf(vectorFile, "%s%c", &vocab[b * max_w], &ch);
		for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, vectorFile);
		len = 0;
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
		len = sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;
	}
	fclose(vectorFile);

	strcpy(filename, argv[2]);
	testFile = fopen(filename, "r");
	if (testFile == NULL) {
		printf("<Test File> not found\n");
		printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	if (option) {
		strcpy(filename, argv[4]);
		outputFile = fopen(filename, "w");
		if (outputFile == NULL) {
			printf("<Output File> not found\n");
			printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File>\n");
			printf("Vector File contains word projections in the Binary Format.\n");
			printf("Output File is Option. You don't need to use it.\n\n");
			return -1;
		}
	}

	if (senseFlag) {
		char data[50];
		unsigned int length, hash;

		if (korlexFile == NULL) {
			printf("Sense file not found\n");
			return -1;
		}
		fscanf(korlexFile, "%lld", &senseSize);

		hash_size = senseSize * 10;
		hash_array = (int *)calloc(hash_size, sizeof(int));
		list = (struct sense_data*)malloc((long long)senseSize * sizeof(struct sense_data));

		for (a = 0; a < hash_size; a++) hash_array[a] = -1;
		for (a = 0; a < senseSize; a++) {
			ReadWord(data, korlexFile);
			length = strlen(data) + 1;
			list[a].sense = (char *)calloc(length, sizeof(char));
			strcpy(list[a].sense, data);

			ReadWord(data, korlexFile);
			length = strlen(data) + 1;
			list[a].word = (char *)calloc(length, sizeof(char));
			strcpy(list[a].word, data);
		}
		qsort(list, senseSize, sizeof(struct sense_data), StrNumCompare);
		for (a = 0; a < senseSize; a++) {
			hash = GetHashKey(list[a].sense);
			while (hash_array[hash] != -1) hash = (hash + 1) % hash_size;
			hash_array[hash] = a;
		}

		for (a = 0; a < senseSize; a++) {
			hash = GetHashKey(list[a].word);
			while (hash_array[hash] != -1) hash = (hash + 1) % hash_size;
			hash_array[hash] = a;
		}
	}


	st = (char**)malloc(sizeof(char*) * 4);
	for (int i = 0; i < 4; i++) {
		st[i] = (char*)malloc(sizeof(char)*max_size);
		memset(st[i], 0, sizeof(char)*max_size);
	}
	memset(bList, 0, sizeof(bList));
	while (!feof(testFile)) {
		notVocab = false;
		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;
		fscanf(testFile, "%s", st[0]);

		if (!strcmp(st[0], ":")) {
			if (subQueNum != 0) {
				if (option) {
					fprintf(outputFile, "Right Answer Rate of Subject's Questions : %.2lf%% (%d/%d)\n", subRightNum / (float)subQueNum * 100, subRightNum, subQueNum);
					fprintf(outputFile, "Right Answer Rate of Subject's Calculated Questions : %.2lf%% (%d/%d)\n\n", subRightNum / (float)subCalNum * 100, subRightNum, subCalNum);
				}
				allQueNum += subQueNum;
				allRightNum += subRightNum;
				allCalNum += subCalNum;
				subQueNum = 0;
				subRightNum = 0;
				subCalNum = 0;
			}
			subjectNum++;
			fscanf(testFile, "%s", st[0]);
			if (option) fprintf(outputFile, "%d. %s\n", subjectNum, st[0]);
			continue;
		}
		fscanf(testFile, "%s", st[1]);
		fscanf(testFile, "%s", st[2]);
		fscanf(testFile, "%s", st[3]);
		subQueNum++;
		if (senseFlag) {
			for (int t = 0; t<4; t++) {
				int index = SearchWord(st[t]);
				if (index != -1) {
					strcpy(st[t], list[index].sense);
				}
			}
		}
		for (int ind=0 ; ind < 4; ind++) {
	                for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[ind])) break;
			bList[ind] = b;
			if (bList[ind] == words) { bList[ind] = -1; notVocab = true; }
		}

		if (notVocab) {
			if (option) {
				if(senseFlag){
					for(int id = 0 ; id < 4; id++) {
						if(id ==3) fprintf(outputFile, " | ");
						int index = SearchSense(st[id]);
						if (index != -1){
							fprintf(outputFile, "(%s", list[index].word);
							index++;
							while (!strcmp(st[id], list[index].sense)) {
								fprintf(outputFile, ", %s", list[index].word);
								index++;
							}
							fprintf(outputFile, ")");
						}
						else {
							fprintf(outputFile, "( %s)",st[id]);	
						}
						fprintf(outputFile, "(%lld) ",bList[id]);	
					}
					fprintf(outputFile,"\n");	
				}
				else {
					fprintf(outputFile, "%s(%lld) %s(%lld) %s(%lld) | %s(%lld)\n", st[0], bList[0], st[1], bList[1], st[2], bList[2], st[3], bList[3]);
				}
			}
			continue;
		}
		else subCalNum++;

		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;

		for (a = 0; a < size; a++) vec[a] = (M[a + bList[1] * size] - M[a + bList[0] * size]) + M[a + bList[2] * size];

		for (c = 0; c < words; c++) {
			if (c == bList[0]) continue;
			if (c == bList[1]) continue;
			if (c == bList[2]) continue;
			dist = 0;
			for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
			for (a = 0; a < N; a++) {
				if (dist > bestd[a]) {
					for (d = N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy(bestw[d], bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy(bestw[a], &vocab[c * max_w]);
					break;
				}
			}
		}

		if (!strcmp(st[3], bestw[0])) subRightNum++;
		else {
			if (option) {
				if(senseFlag) {
					for(int id = 0 ; id < 4; id++) {
                                                if(id ==3) fprintf(outputFile, " | ");
                                                int index = SearchSense(st[id]);
                                                if (index != -1){
                                                        fprintf(outputFile, "(%s",list[index].word);
                                                        index++;
                                                        while (!strcmp(st[id], list[index].sense)) {
                                                                fprintf(outputFile, ", %s", list[index].word);
                                                                index++;
                                                        }
                                                        fprintf(outputFile, ")");
                                                }
                                                else {
                                                        fprintf(outputFile, "( %s)",st[id]);
                                                }
                                                fprintf(outputFile, "(%lld) ",bList[id]);
                                        }
                                        fprintf(outputFile,"= ");
                                        for (a = 0; a < N; a++) {
						int index = SearchSense(bestw[a]);
						if (index != -1){
							fprintf(outputFile, "( %s", list[index].word);
							index++;
							while (!strcmp(bestw[a], list[index].sense)) {
								fprintf(outputFile, ", %s", list[index].word);
								index++;
                                                       	}
						        fprintf(outputFile, ")");	
						}
						else {
							fprintf(outputFile, "( %s)",bestw[a]);
						}
					}
                                        fprintf(outputFile, "\n");
				}
				else {
					fprintf(outputFile, "%s(%lld) %s(%lld) %s(%lld) | %s(%lld) =", st[0], bList[0], st[1], bList[1], st[2], bList[2], st[3], bList[3]);
					for (a = 0; a < N; a++) fprintf(outputFile, " %s", bestw[a]);
					fprintf(outputFile, "\n");
				}
			}
		}
	}

	allQueNum += subQueNum;
	allRightNum += subRightNum;
	allCalNum += subCalNum;

	if (option) {
		fprintf(outputFile, "Right Answer Rate of Subject's Questions : %.2lf%% (%d/%d)\n", subRightNum / (float)subQueNum * 100, subRightNum, subQueNum);
		fprintf(outputFile, "Right Answer Rate of Subject's Calculated Questions : %.2lf%% (%d/%d)\n\n", subRightNum / (float)subCalNum * 100, subRightNum, subCalNum);

		fprintf(outputFile, "Calculated Questions Rate of All Questions: %.2f%% (%d/%d)\n", allCalNum / (float)allQueNum * 100, allCalNum, allQueNum);
		fprintf(outputFile, "Right Answer Rate of All Questions : %.2lf%% (%d/%d)\n", allRightNum / (float)allQueNum * 100, allRightNum, allQueNum);
		fprintf(outputFile, "Right Answer Rate of Calculated Questions : %.2lf%% (%d/%d)", allRightNum / (float)allCalNum * 100, allRightNum, allCalNum);
		fclose(outputFile);
	}

	printf("Calculated Questions Rate of All Questions: %.2f%% (%d/%d)\n", allCalNum / (float)allQueNum * 100, allCalNum, allQueNum);
	printf("Right Answer Rate of All Questions : %.2lf%% (%d/%d)\n", allRightNum / (float)allQueNum * 100, allRightNum, allQueNum);
	printf("Right Answer Rate of Calculated Questions : %.2lf%% (%d/%d)\n\n", allRightNum / (float)allCalNum * 100, allRightNum, allCalNum);

	fclose(testFile);
	return 0;
}

