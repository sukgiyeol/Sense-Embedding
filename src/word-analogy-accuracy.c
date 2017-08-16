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

int main(int argc, char **argv)
{
	FILE *vectorFile, *testFile, *outputFile;
	bool option = false, notVocab;
	char st1[max_size], st2[max_size], st3[max_size], st4[max_size], bestw[N][max_size], filename[100],ch;
	float dist, len, bestd[N], vec[max_size];
	long long words, size, a, b, c, d, b1, b2, b3;
	float *M;
	char *vocab;
	
	int subjectNum = 0, allQueNum = 0, allRightNum = 0, allCalNum = 0,
		subQueNum = 0, subRightNum = 0, subCalNum = 0;
	//Parameter Check
	if (argc == 3 || argc == 5) {
		option = false;
		if (argc == 5) {
			if (strcmp(argv[3], "-o") == 0) {
				option = true;
			}
		}
	}
	else {
		printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return 0;
	}

	//Vector File(bin) open and Read word and vector array.
	strcpy(filename, argv[1]);
	vectorFile = fopen(filename, "rb");		//read binary
	if (vectorFile == NULL) {
		printf("<Vector File> not found\n");
		printf("Usage: ./word-analogy-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

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

	while (!feof(testFile)){
		notVocab = false;
		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;
		fscanf(testFile,"%s", st1);
	
		if (!strcmp(st1, ":")) {
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
			fscanf(testFile,"%s", st1);
			if (option) fprintf(outputFile, "%d. %s\n", subjectNum, st1);
			continue;
		}
		fscanf(testFile, "%s", st2);
		fscanf(testFile, "%s", st3);
		fscanf(testFile, "%s", st4);
		subQueNum++;
		
		for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st1)) break;
		b1 = b;
		for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st2)) break;
		b2 = b;
		for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st3)) break;
		b3 = b;
		for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st4)) break;

		if (b1 == words) { b1 = -1; notVocab = true; }
		if (b2 == words) { b2 = -1; notVocab = true; }
		if (b3 == words) { b3 = -1; notVocab = true; }
		if (b == words) { b = -1; notVocab = true; }

		if (notVocab) { 
			if (option) fprintf(outputFile, "%s(%lld) %s(%lld) %s(%lld) | %s(%lld)\n", st1, b1, st2, b2, st3, b3, st4, b);
			continue;
		}
		else subCalNum++;
			
		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;
				
		for (a = 0; a < size; a++) vec[a] = (M[a + b2 * size] - M[a + b1 * size]) + M[a + b3 * size];
		
		for (c = 0; c < words; c++) {
			if (c == b1) continue;
			if (c == b2) continue;
			if (c == b3) continue;
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
		
		if (!strcmp(st4, bestw[0])) subRightNum++;
		else {
			if (option) { 
				fprintf(outputFile, "%s(%lld) %s(%lld) %s(%lld) | %s(%lld) =", st1, b1, st2, b2, st3, b3, st4, b); 
				for (a = 0; a < N; a++) fprintf(outputFile, " %s", bestw[a]);
				fprintf(outputFile, "\n");
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
