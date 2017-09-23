#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
typedef enum { false, true } bool;
const long long max_size = 2000;         // max length of strings
const long long max_width = 50;                  // max length of word alphabet
struct sense_data {
	char *sense, *word;
};
bool senseFlag = false , option = false , found = false;
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
		if (a >= max_width - 1) a--;   // Truncate too long words
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


int main(int argc, char **argv) {
	FILE *vectorFile, *testFile, *outputFile, *korlexFile;
	char ch, file_name[100], data[2][20], korlex_line[max_size];;
	char *vocab, *main_p, *main_sp;
	float dist, len, vec[max_size];
	float survey_result, accuracySum = 0.0, accuracy = 0.0;
	float *Vectors;
	long long words, vectorLen, wordNum[100], senseSize;
	long long a, b;                 //loof index
	int testNum = 0, allNum = 0;

	//Parameter Check
	if (argc < 3) {
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File> -s <Korlex>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return 0;
	}
	if (argc == 5 || argc == 7) {
		if (argc == 5) {
			if (strcmp(argv[3], "-o") == 0) {
				option = true;
			}
			else if (strcmp(argv[3], "-s") == 0) {
				senseFlag = true;
				korlexFile = fopen(argv[4], "r");
			}
			else {
				printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File> -s <Korlex>\n");
				printf("Vector File contains word projections in the Binary Format.\n");
				printf("Output File is Option. You don't need to use it.\n\n");
				return 0;
			}
		}
		else {
			if (strcmp(argv[3], "-o") == 0 && strcmp(argv[5], "-s") == 0) {
				option = true;
				senseFlag = true;
				korlexFile = fopen(argv[6], "r");
			}
			else {
				printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File> -s <Korlex>\n");
				printf("Vector File contains word projections in the Binary Format.\n");
				printf("Output File is Option. You don't need to use it.\n\n");
				return 0;
			}
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
	//Vector File(bin) open and Read word and vector array.
	strcpy(file_name, argv[1]);
	vectorFile = fopen(file_name, "rb");             //read binary
	if (vectorFile == NULL) {
		printf("<Vector File> not found\n");
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	strcpy(file_name, argv[2]);
	testFile = fopen(file_name, "r");
	if (testFile == NULL) {
		printf("<Test File> not found\n");
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	if (option) {
		strcpy(file_name, argv[4]);
		outputFile = fopen(file_name, "w");
		if (outputFile == NULL) {
			printf("<Output File> not found\n");
			printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
			printf("Vector File contains word projections in the Binary Format.\n");
			printf("Output File is Option. You don't need to use it.\n\n");
			return -1;
		}
	}

	fscanf(vectorFile, "%lld", &words);                                             //%lld long long Data
	fscanf(vectorFile, "%lld", &vectorLen);
	vocab = (char *)malloc((long long)words * max_width * sizeof(char));
	Vectors = (float *)malloc((long long)words * (long long)vectorLen * sizeof(float));

	if (Vectors == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * vectorLen * sizeof(float) / 1048576, words, vectorLen);
		return -1;
	}

	for (b = 0; b < words; b++) {
		fscanf(vectorFile, "%s%c", &vocab[b * max_width], &ch);
		for (a = 0; a < vectorLen; a++) fread(&Vectors[a + b * vectorLen], sizeof(float), 1, vectorFile);
		len = 0;
		for (a = 0; a < vectorLen; a++) len += Vectors[a + b * vectorLen] * Vectors[a + b * vectorLen];
		len = sqrt(len);
		for (a = 0; a < vectorLen; a++) Vectors[a + b * vectorLen] /= len;
	}
	fclose(vectorFile);

	//Loop Main Function
	if (option) fprintf(outputFile, "word\tvocabNum\tword\tvocabNum\tservey\tdistance\tmatching rate\n");
	while (!feof(testFile)) {
		fscanf(testFile, "%s\t%s\t%f", &data[0][0], &data[1][0], &survey_result);
		found = true;
		allNum++;
		//Get Number of two Words in vocabulary
		if (senseFlag) {
			for (int t = 0; t < 2; t++) {
				int index = SearchWord(data[t]);
				if (index != -1) {
					strcpy(data[t], list[index].sense);
				}
			}
		}
		for (a = 0; a < 2; a++) {
			for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_width], data[a])) break;
			if (b == words) b = -1;
			wordNum[a] = b;
			if (b == -1) {
				found = false;
			}
		}

		if (!found) {
                        if (option) {
                                if(senseFlag){
                                        for(int ind = 0; ind < 2; ind ++) {
                                                int index = SearchSense(data[ind]);
                                                if (index != -1){
                                                        fprintf(outputFile, "( %s", list[index].word);
                                                        index++;
                                                        while (!strcmp(data[ind], list[index].sense)) {
                                                                fprintf(outputFile, ", %s", list[index].word);
                                                                index++;
                                                        }
                                                        fprintf(outputFile,")\t");
                                                }
                                                else {
                                                        fprintf(outputFile, "( %s)\t", &data[ind][0]);
                                                }
                                                fprintf(outputFile,"%lld\t",wordNum[ind]);
                                        }
                                        fprintf(outputFile,"%.4f\t%.4f\t%.4f(%%)\n",survey_result, dist, accuracy);
                                }
                                else {
                                        fprintf(outputFile, "( %s )\t%lld\t( %s )\t%lld\t%.4f\t%.4f\t%.4f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], survey_result, dist, accuracy);
                                }
                        }

		}
		else {
			accuracy = 0;
			testNum++;
			//Calculation Variable 'vec' with first word vector
			for (a = 0; a < vectorLen; a++) vec[a] = 0;
			for (a = 0; a < vectorLen; a++) vec[a] += Vectors[a + wordNum[0] * vectorLen];
			len = 0;
			for (a = 0; a < vectorLen; a++) len += vec[a] * vec[a];
			len = sqrt(len);
			for (a = 0; a < vectorLen; a++) vec[a] /= len;

			//Calculation distance with two input words
			dist = 0.0;
			for (a = 0; a < vectorLen; a++) dist += vec[a] * Vectors[a + wordNum[1] * vectorLen];
			dist = fabs(dist);
			dist *= 10;
			if (dist > survey_result) accuracy = 100 - ((dist - survey_result) * 100 / dist);
			else accuracy = 100 - ((survey_result - dist) * 100 / survey_result);

			accuracySum += accuracy;
			if (option) {
				if(senseFlag){
					for(int ind = 0; ind < 2; ind ++) {
						int index = SearchSense(data[ind]);
						if (index != -1){
							fprintf(outputFile, "( %s", list[index].word);
							index++;
							while (!strcmp(data[ind], list[index].sense)) {
								fprintf(outputFile, ", %s", list[index].word);
								index++;
							}
							fprintf(outputFile,")\t");
						}
						else {
		                                        fprintf(outputFile, "( %s)\t", &data[ind][0]);
						}
						fprintf(outputFile,"%lld\t",wordNum[ind]);
					}
					fprintf(outputFile,"%.4f\t%.4f\t%.4f(%%)\n",survey_result, dist, accuracy);
				}
				else {
					fprintf(outputFile, "( %s )\t%lld\t( %s )\t%lld\t%.4f\t%.4f\t%.4f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], survey_result, dist, accuracy);
				}
			}
		}
	}

	if (option) {
		fprintf(outputFile, "\nCalculated Questions Rate of All Questions : %.2f%% (%d/%d)\n", testNum / (float)allNum * 100, testNum, allNum);
		fprintf(outputFile, "Average matching rate of All Questions : %.2lf%%\n", accuracySum / allNum);
		fprintf(outputFile, "Average matching rate of Calculated Questions : %.2lf%%", accuracySum / testNum);
		fclose(outputFile);
	}

	printf("Calculated Questions Rate of All Questions : %.2f%% (%d/%d)\n", testNum / (float)allNum * 100, testNum, allNum);
	printf("Average matching rate of All Questions : %.2lf%%\n", accuracySum / allNum);
	printf("Average matching rate of Calculated Questions : %.2lf%%\n\n", accuracySum / testNum);
	fclose(testFile);
	return 0;
}
