#include <stdio.h>
#include <stdlib.h> // mac os x
#include <string.h>
#include <math.h>
typedef enum { false, true } bool;
int sizeX = 0, sizeOfList = 0;
const long long max_size = 255;         // max length of strings
const long long max_w = 50;              // max length of vocabulary entries
const int exampleOfProblem = 4;
const int lineOfProblem = 5;
const int maxWordInLine = 20;
int totalNumOfProblem = 0;           // modify if you append your data
const int maxNumOfProblem = 300;
bool output = false, senseFlag = false;

char** res, **arr, **sense_num;

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

int main(int argc, char **argv) {
	FILE *aFile, *outputFile, *korlexFile;
	char ch, text[max_size], stringList[maxNumOfProblem][max_size], exampleList[exampleOfProblem][max_size];
	float distance, len, vec[max_size], distList[exampleOfProblem];
	long long a,b,c,totalWordNum, size, aIndex, bIndex, score, wordIndex[maxNumOfProblem], exampleIndex[exampleOfProblem] , senseSize;
	float *aMemory;
	char korlex_line[max_size];
	char *vocab, *main_p, *main_sp;
	char ** tokens;
	char *** sentence;
	int* solution, *numOfProblemWordList, *numOfProblemWordListSdk;

	if (argc == 3) {}
	else if (argc == 5) {
		if (strcmp(argv[3], "-o") == 0) {
			output = true;
		}
		else if (strcmp(argv[3], "-s") == 0) {
			senseFlag = true;
			korlexFile = fopen(argv[4], "r");
		}
		else {
			printf("Parameter Bad\n");
			printf("Usage: ./sentence_complete <VECTOR FILE> <DATA> -o <Output File> -k <Korlex Data>\nwhere VECTOR FILE contains word projections in the BINARY FORMAT\nDATA contains test word data\nOutput is result of print.\nKorlex usage to sense vector\n");
			return 0;
		}
	}
	else if (argc == 7) {
		if (strcmp(argv[3], "-o") == 0 && strcmp(argv[5], "-s") == 0) {
			output = true;
			senseFlag = true;
			korlexFile = fopen(argv[6], "r");
		}
		else {
			printf("Parameter Bad\n");
			printf("Usage: ./sentence_complete <VECTOR FILE> <DATA> -o <Output File> -k <Korlex Data>\nwhere VECTOR FILE contains word projections in the BINARY FORMAT\nDATA contains test word data\nOutput is result of print.\nKorlex usage to sense vector\n");
			return 0;
		}
	}
	else {
		printf("Parameter Bad\n");
		printf("Usage: ./sentence_complete <VECTOR FILE> <DATA> -o <Output File> -k <Korlex Data>\nwhere VECTOR FILE contains word projections in the BINARY FORMAT\nDATA contains test word data\nOutput is result of print.\nKorlex usage to sense vector\n");
		return 0;
	}
	strcpy(text, argv[1]);
	aFile = fopen(text, "rb");
	if (aFile == NULL) {
		printf("Input file not found\n");
		return -1;
	}
	if (output) {
		strcpy(text, argv[4]);
		outputFile = fopen(text, "wb");
	}
	fscanf(aFile, "%lld", &totalWordNum);
	fscanf(aFile, "%lld", &size);
	vocab = (char *)malloc((long long)totalWordNum * max_w * sizeof(char));
	aMemory = (float *)malloc((long long)totalWordNum * (long long)size * sizeof(float));
	if (aMemory == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)totalWordNum * size * sizeof(float) / 1048576, totalWordNum, size);
		return -1;
	}
	for (bIndex = 0; bIndex < totalWordNum; bIndex++) {
		fscanf(aFile, "%s%c", &vocab[bIndex * max_w], &ch);
		for (aIndex = 0; aIndex < size; aIndex++) fread(&aMemory[aIndex + bIndex * size], sizeof(float), 1, aFile);
		len = 0;
		for (aIndex = 0; aIndex < size; aIndex++) len += aMemory[aIndex + bIndex * size] * aMemory[aIndex + bIndex * size];
		len = (float)sqrt(len);
		for (aIndex = 0; aIndex < size; aIndex++) aMemory[aIndex + bIndex * size] /= len;
	}
	fclose(aFile);
	strcpy(text, argv[2]);
	aFile = fopen(text, "rb");
	if (aFile == NULL) {
		printf("Input file not found\n");
		return -1;
	}
	sentence = (char***)malloc((maxNumOfProblem * lineOfProblem + 1) * sizeof(char**));
	for (int i = 0; i < maxNumOfProblem * lineOfProblem; i++) {
		sentence[i] = (char**)malloc(maxWordInLine * sizeof(char*));
		for (int j = 0; j < maxWordInLine; j++) {
			sentence[i][j] = (char*)malloc(20 * sizeof(char));
		}
	}
	solution = (int*)malloc(maxNumOfProblem * sizeof(int));
	numOfProblemWordList = (int*)malloc(maxNumOfProblem * sizeof(int));
	numOfProblemWordListSdk = (int*)malloc(maxNumOfProblem * sizeof(int));
	int indexOfLine = 0, indexOfWord = 0;

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


	while (fgets(text, sizeof(text), aFile) != NULL) {
		tokens = (char **)malloc(lineOfProblem * sizeof(char**));
		for (int i = 0; i < lineOfProblem; i++) {
			tokens[i] = (char*)malloc(max_size * sizeof(char*));
		}
		char *pointer = strtok(text, "/");
		int a = 0;
		while (pointer != NULL) {              // '/' 으로 자르기
			tokens[a] = pointer;
			pointer = strtok(NULL, "/");
			a++;
		}
		for (int it = 0; it < lineOfProblem; it++) {
			indexOfWord = 0;
			pointer = strtok(tokens[it], " ");
			while (pointer != NULL) {           // space 로 자르기.
				strcpy(sentence[lineOfProblem * indexOfLine + it][indexOfWord], pointer);
				pointer = strtok(NULL, " ");    // 다음 문자열을 잘라서 포인터를 반환
				indexOfWord++;
			}
			if (it == 0) numOfProblemWordList[indexOfLine] = indexOfWord;
			else if (it == 2) numOfProblemWordListSdk[indexOfLine] = indexOfWord;
		}
		indexOfLine++;
	}
	for (int i = 0; i < maxNumOfProblem; i++) {
		strcpy(text, sentence[lineOfProblem * i + 4][0]);
		text[1] = '\0';
		solution[i] = atoi(text);
	}
	totalNumOfProblem = indexOfLine;
	score = 0;
	int missedScore = 0;
	for (int index = 0; index<totalNumOfProblem; index++) {
		if (output) {
			fprintf(outputFile, "\n\n================================== 문제 %d ==================================\n", index + 1);
			fprintf(outputFile, "\n\t\t");

			for (int x = 0; x < numOfProblemWordList[index]; x++) {
				fprintf(outputFile, "%s ", sentence[lineOfProblem * index][x]);
			}
			if (index < 9) fprintf(outputFile, "\n\n============================================================================\n\n");
			else fprintf(outputFile, "\n\n=============================================================================\n\n");
			fprintf(outputFile, "    ");
		}
		for (int j = 0; j < numOfProblemWordListSdk[index]; j++) {
			strcpy(stringList[j], sentence[lineOfProblem * index + 2][j]);                 // 문제 parsing
		}
		for (int x = 0; x < exampleOfProblem; x++) {
			if (output) {
				fprintf(outputFile, "%d. %s    ", x + 1, sentence[lineOfProblem * index + 1][x]);      // 보기 출력
			}
			strcpy(exampleList[x], sentence[lineOfProblem * index + 3][x]);                                          // 보기 parsing
		}
		if (senseFlag) {
			//문제 to sense@@@@@@@@@@@@@@@@@@@@@@@@@
			for (int t = 0; t<numOfProblemWordListSdk[index]; t++) {
				int index = SearchWord(stringList[t]);
				if (index != -1) {
					strcpy(stringList[t], list[index].sense);
				}
			}
		}
		for (aIndex = 0; aIndex < numOfProblemWordListSdk[index]; aIndex++) {
			for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], stringList[aIndex])) break;
			if (bIndex == totalWordNum) bIndex = -1;
			wordIndex[aIndex] = bIndex;
			//printf("\tWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
			if (bIndex == -1) {
				if (output) {
					fprintf(outputFile, "Out of dictionary word!\n");
				}
				missedScore++;
				break;
			}
		}
		if (bIndex == -1) continue;
		if (senseFlag) {
			//보기 to sense@@@@@@@@@@@@@@@@@@@@@@@@@
			for (int t = 0; t<exampleOfProblem; t++) {
				int index = SearchWord(exampleList[t]);
				if (index != -1) {
					strcpy(exampleList[t], list[index].sense);
				}
			}
		}
		for (aIndex = 0; aIndex < exampleOfProblem; aIndex++) {
			for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], exampleList[aIndex])) break;
			if (bIndex == totalWordNum) bIndex = -1;
			exampleIndex[aIndex] = bIndex;
			//printf("\n\tWord: %s  Position in vocabulary: %lld", ex[a], exi[a]);
			if (exampleIndex[aIndex] == -1) {
				if (output) {
					fprintf(outputFile, "  |- Input '%s' is Out of dictionary word or Bad Vector!\n", exampleList[aIndex]);
				}
				missedScore++;
				break;
			}
		}
		if (bIndex == -1) continue;
		//bi 는 문제 , exi 는 보기 위치
		for (int i = 0; i < exampleOfProblem; i++) {
			for (aIndex = 0; aIndex < size; aIndex++) vec[aIndex] = 0;
			for (aIndex = 0; aIndex < size; aIndex++) vec[aIndex] += aMemory[aIndex + exampleIndex[i] * size];
			len = 0;
			for (aIndex = 0; aIndex < size; aIndex++) len += vec[aIndex] * vec[aIndex];
			len = (float)sqrt(len);
			for (aIndex = 0; aIndex < size; aIndex++) vec[aIndex] /= len;
			distance = 0;
			for (int j = 0; j < numOfProblemWordListSdk[index]; j++) {
				for (aIndex = 0; aIndex < size; aIndex++) distance += vec[aIndex] * aMemory[aIndex + wordIndex[j] * size];
			}
			distList[i] = distance;
		}
		int maxValue = 0;
		float maxDist = 0;
		maxDist = distList[0];
		for (int iter = 1; iter < exampleOfProblem; iter++) {               // max 구하기
			if (maxDist < distList[iter]) {
				maxValue = iter;
				maxDist = distList[iter];
			}
		}
		if (solution[index] == maxValue + 1) {
			score++;
		}
		if (output) {
			if (index < 9) fprintf(outputFile, "\n\n============================================================================\n");
			else fprintf(outputFile, "\n\n=============================================================================\n");
			if (solution[index] == maxValue + 1) {
				fprintf(outputFile, "    정답입니다. 학습의 선택은 %d번입니다.\n", maxValue + 1);
			}
			else {
				fprintf(outputFile, "    틀렸습니다. 학습의 선택은 %d번입니다. \n", maxValue + 1);
			}
			fprintf(outputFile, "    현재 상황 : %lld / %d \n", score, index + 1);
		}
	}
	if (output) {
		fprintf(outputFile, "=============================================================================\n");
		fprintf(outputFile, "\n\tALL Questions : %0.2f % (%lld/%d) \n\n", (score / (float)totalNumOfProblem) * 100, score, totalNumOfProblem);
		fprintf(outputFile, "\n\tCalculated Questions : %0.2f % (%lld/%d) \n\n", (score / (float)(totalNumOfProblem - missedScore)) * 100, score, totalNumOfProblem - missedScore);
		fprintf(outputFile, "=============================================================================\n");
		fclose(outputFile);
	}
	printf("=============================================================================\n");
	printf("\n\tALL Questions : %0.2f % (%lld/%d) \n\n", (score / (float)totalNumOfProblem) * 100, score, totalNumOfProblem);
	printf("\n\tCalculated Questions : %0.2f % (%lld/%d) \n\n", (score / (float)(totalNumOfProblem - missedScore)) * 100, score, totalNumOfProblem - missedScore);
	printf("=============================================================================\n");
	fclose(aFile);
	return 0;
}
