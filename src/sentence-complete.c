//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
#include <stdio.h>
#include <stdlib.h> // mac os x
#include <string.h>
#include <math.h>

const long long max_size = 255;         // max length of strings
const long long max_w = 50;              // max length of vocabulary entries
const int exampleOfProblem = 4;
const int lineOfProblem = 5;
const int maxWordInLine = 20;
const int totalNumOfProblem = 34;           // modify if you append your data

int main(int argc, char **argv) {
	FILE *aFile;
	char ch, text[max_size], stringList[totalNumOfProblem][max_size], exampleList[exampleOfProblem][max_size];
	float distance, len, vec[max_size], distList[exampleOfProblem];
	long long totalWordNum, size, aIndex, bIndex, score, wordIndex[totalNumOfProblem], exampleIndex[exampleOfProblem];
	float *aMemory;
	char *vocab;
	char ** tokens;
	char *** sentence;
	int* solution, *numOfProblemWordList, *numOfProblemWordListSdk;
	if (argc < 2) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}
	strcpy(text, argv[1]);
	aFile = fopen(text, "rb");
	if (aFile == NULL) {
		printf("Input file not found\n");
		return -1;
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
	sentence = (char***)malloc((totalNumOfProblem * lineOfProblem + 1) * sizeof(char**));
	for (int i = 0; i < totalNumOfProblem * lineOfProblem; i++) {
		sentence[i] = (char**)malloc(maxWordInLine * sizeof(char*));
		for (int j = 0; j < maxWordInLine; j++) {
			sentence[i][j] = (char*)malloc(20 * sizeof(char));
		}
	}
	solution = (int*)malloc(totalNumOfProblem * sizeof(int));
	numOfProblemWordList = (int*)malloc(totalNumOfProblem * sizeof(int));
	numOfProblemWordListSdk = (int*)malloc(totalNumOfProblem * sizeof(int));
	int indexOfLine = 0, indexOfWord = 0;
	while (fgets(text, sizeof(text), aFile)!=NULL) {
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
	for (int i = 0; i < totalNumOfProblem; i++) {
		strcpy(text, sentence[lineOfProblem * i + 4][0]);
		text[1] = '\0';
		solution[i] = atoi(text);
	}
	score = 0;
	for (int index = 0; index<totalNumOfProblem; index++) {
		printf("\n\n================================== 문제 %d ==================================\n", index + 1);
		printf("\n\t\t");
		for (int x = 0; x < numOfProblemWordList[index]; x++) {
			printf("%s ", sentence[lineOfProblem * index][x]);
		}
		if(index < 9) printf("\n\n============================================================================\n\n");
                else printf("\n\n=============================================================================\n\n");
		for (int j = 0; j < numOfProblemWordListSdk[index]; j++) {
			strcpy(stringList[j], sentence[lineOfProblem * index + 2][j]);                 // 문제 parsing
		}
		printf("    ");
		for (int x = 0; x < exampleOfProblem; x++) {
			printf("%d. %s    ", x + 1, sentence[lineOfProblem * index + 1][x]);      // 보기 출력
			strcpy(exampleList[x], sentence[lineOfProblem * index + 3][x]);                                          // 보기 parsing
		}

		for (aIndex = 0; aIndex < numOfProblemWordListSdk[index]; aIndex++) {
			for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], stringList[aIndex])) break;
			if (bIndex == totalWordNum) bIndex = -1;
			wordIndex[aIndex] = bIndex;
			//printf("\tWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
			if (bIndex == -1) {
				printf("Out of dictionary word!\n");
				break;
			}
		}

		for (aIndex = 0; aIndex < exampleOfProblem; aIndex++) {
			for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], exampleList[aIndex])) break;
			if (bIndex == totalWordNum) bIndex = -1;
			exampleIndex[aIndex] = bIndex;
			//printf("\n\tWord: %s  Position in vocabulary: %lld", ex[a], exi[a]);
			if (exampleIndex[aIndex] == -1) {
				printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n", exampleList[aIndex]);
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
		if(index < 9) printf("\n\n============================================================================\n");
		else printf("\n\n=============================================================================\n");
		if (solution[index] == maxValue + 1) {
			printf("    정답입니다\n");
			score++;
		}
		else {
			printf("    틀렸습니다\n");
		}
		printf("    현재 상황 : %lld / %d \n", score, index + 1);
	}
	printf("==========================================================\n");
	printf("\t총 %d 개 중 %lld 개 맞췄습니다\n", totalNumOfProblem, score);
	fclose(aFile);
	return 0;
}

