//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http:/wwwoapachecore);org/licenses/LICENSE-2.0
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
typedef enum {false,true} bool;
const long long max_size = 255;         // max length of strings
const long long max_w = 50;              // max length of vocabulary entries
const int exampleOfProblem = 4;
const int lineOfProblem = 5;
const int maxWordInLine = 20;
int totalNumOfProblem = 0;           // modify if you append your data
const int maxNumOfProblem = 300;
bool output = false;
int main(int argc, char **argv) {
        FILE *aFile,*outputFile;
        char ch, text[max_size], stringList[maxNumOfProblem][max_size], exampleList[exampleOfProblem][max_size];
        float distance, len, vec[max_size], distList[exampleOfProblem];
        long long totalWordNum, size, aIndex, bIndex, score, wordIndex[maxNumOfProblem], exampleIndex[exampleOfProblem];
        float *aMemory;
        char *vocab;
        char ** tokens;
        char *** sentence;
        int* solution, *numOfProblemWordList, *numOfProblemWordListSdk;

        if (argc == 3){}
        else if ( argc == 5 ) {
                if (strcmp(argv[3],"-o")==0) {
                        output = true;
                }
                else {
                        printf("Parameter Bad\n");
                        printf("Usage: ./sentence_complete <VECTOR FILE> <DATA> -o <Output File>\nwhere VECTOR FILE contains word projections in the BINARY FORMAT\nDATA contains test word data\nOutput is result of print.\n");
                        return 0;
                }
        }
        else{
                printf("Parameter Bad\n");
                printf("Usage: ./sentence_complete <VECTOR FILE> <DATA> -o <Output File>\nwhere VECTOR FILE contains word projections in the BINARY FORMAT\nDATA contains test word data\nOutput is result of print.\n");
                return 0;
        }
        strcpy(text, argv[1]);
        aFile = fopen(text, "rb");
        if (aFile == NULL) {
                printf("Input file not found\n");
                return -1;
        }
        if(output) {
                strcpy(text,argv[4]);
                outputFile = fopen(text,"wb");
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
        for (int i = 0; i < maxNumOfProblem; i++) {
                strcpy(text, sentence[lineOfProblem * i + 4][0]);
                text[1] = '\0';
                solution[i] = atoi(text);
        }
        totalNumOfProblem = indexOfLine;
        score = 0;
        int missedScore = 0;
        for (int index = 0; index<totalNumOfProblem; index++) {
                if(output) {
                        fprintf(outputFile,"\n\n================================== 문제 %d ==================================\n", index + 1);
                        fprintf(outputFile,"\n\t\t");

                        for (int x = 0; x < numOfProblemWordList[index]; x++) {
                                fprintf(outputFile,"%s ", sentence[lineOfProblem * index][x]);
                        }
                        if(index < 9) fprintf(outputFile,"\n\n============================================================================\n\n");
                        else fprintf(outputFile,"\n\n=============================================================================\n\n");
                        fprintf(outputFile,"    ");
                }
                for (int j = 0; j < numOfProblemWordListSdk[index]; j++) {
                        strcpy(stringList[j], sentence[lineOfProblem * index + 2][j]);                 // 문제 parsing
                }
                for (int x = 0; x < exampleOfProblem; x++) {
                        if (output){
                                 fprintf(outputFile,"%d. %s    ", x + 1, sentence[lineOfProblem * index + 1][x]);      // 보기 출력
                        }
                        strcpy(exampleList[x], sentence[lineOfProblem * index + 3][x]);                                          // 보기 parsing
                }

                for (aIndex = 0; aIndex < numOfProblemWordListSdk[index]; aIndex++) {
                        for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], stringList[aIndex])) break;
                        if (bIndex == totalWordNum) bIndex = -1;
                        wordIndex[aIndex] = bIndex;
                        //printf("\tWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
                        if (bIndex == -1) {
                                if(output){
                                        fprintf(outputFile,"Out of dictionary word!\n");
                                }
                                missedScore++;
                                break;
                        }
                }
                if(bIndex == -1) continue;
                for (aIndex = 0; aIndex < exampleOfProblem; aIndex++) {
                        for (bIndex = 0; bIndex < totalWordNum; bIndex++) if (!strcmp(&vocab[bIndex * max_w], exampleList[aIndex])) break;
                        if (bIndex == totalWordNum) bIndex = -1;
                        exampleIndex[aIndex] = bIndex;
                        //printf("\n\tWord: %s  Position in vocabulary: %lld", ex[a], exi[a]);
                        if (exampleIndex[aIndex] == -1) {
                                if (output) {
                                        fprintf(outputFile,"  |- Input '%s' is Out of dictionary word or Bad Vector!\n", exampleList[aIndex]);
                                }
                                missedScore++;
                                break;
                        }
                }
                if(bIndex == -1) continue;
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
                if ( solution[index] == maxValue + 1) {
                        score++;
                }
                if ( output ) {
                        if(index < 9) fprintf(outputFile,"\n\n============================================================================\n");
                        else fprintf(outputFile,"\n\n=============================================================================\n");
                        if (solution[index] == maxValue + 1) {
                                fprintf(outputFile,"    정답입니다\n");
                        }
                        else {
                                fprintf(outputFile,"    틀렸습니다\n");
                        }
                        fprintf(outputFile,"    현재 상황 : %lld / %d \n", score, index + 1);
                }
        }
        if(output) {
                fprintf(outputFile, "=============================================================================\n");
                fprintf(outputFile, "\n\tALL Questions : %0.2f % (%lld/%d) \n\n", (score / (float)totalNumOfProblem) * 100, score , totalNumOfProblem);
                fprintf(outputFile, "\n\tCalculated Questions : %0.2f % (%lld/%d) \n\n", (score/(float)(totalNumOfProblem - missedScore))*100 ,score , totalNumOfProblem-missedScore );
                fprintf(outputFile, "=============================================================================\n");
                fclose(outputFile);
        }
        printf("=============================================================================\n");
        printf("\n\tALL Questions : %0.2f % (%lld/%d) \n\n", (score / (float)totalNumOfProblem) * 100, score , totalNumOfProblem);
        printf("\n\tCalculated Questions : %0.2f % (%lld/%d) \n\n", (score/(float)(totalNumOfProblem - missedScore))*100,score, totalNumOfProblem-missedScore );
        printf("=============================================================================\n");
        fclose(aFile);
        return 0;
}
