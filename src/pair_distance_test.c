#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 
typedef enum { false, true } bool;
const long long max_size = 2000;         // max length of strings
const long long max_width = 50; 		 // max length of word alphabet

int main(int argc, char **argv) {
	FILE *vectorFile, *testFile, *outputFile;
	char filename[100];
	float dist, len, vec[max_size];
	long long words, vectorLen, wordNum[100];
	long long a, b;			//loof index
	char ch;
	float *Vectors;
	char *vocab;

	//Parameter Check
	if (argc < 4) {
		printf("Usage: ./pair_distance <Vector File> <Test File> <Output File>\n\n");
		return 0;
	}

	//Vector File(bin) open and Read word and vector array.
	strcpy(filename, argv[1]);
	vectorFile = fopen(filename, "rb");		//read binary
	if (vectorFile == NULL) {
		printf("vector file not found\n");
		return -1;
	}

	strcpy(filename, argv[2]);
	testFile = fopen(filename, "r");
	if (testFile == NULL) {
		printf("test file not found\n");
		return -1;
	}

	strcpy(filename, argv[3]);
	outputFile = fopen(filename, "w");
	if (outputFile == NULL) {
		printf("output file not create\n");
		return -1;
	}
	fprintf(outputFile, "word\tvocabNum\tword\tvocabNum\tservey\tdistance\tresult\n");

	fscanf(vectorFile, "%lld", &words);						//%lld long long Data
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

	char data[2][20];
	float data3, accuracySum = 0.0, accuracy = 0.0;
	int testNum = 0, allNum = 0;
	bool found;
	//Loop Main Function
	while (!feof(testFile)) {
		fscanf(testFile, "%s\t%s\t%f", &data[0][0], &data[1][0], &data3);
		found = true;
		allNum++;
		//Get Number of two Words in vocabulary
		for (a = 0; a < 2; a++) {
			for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_width], data[a])) break;
			if (b == words) b = -1;
			wordNum[a] = b;
			if (b == -1) {
				found = false;
			}
		}

		if (!found) {
			fprintf(outputFile, "%s\t%lld\t%s\t%lld\t%f\t%f\t%f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], data3, 0.00, 0.00);
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
			dist = 0;
			for (a = 0; a < vectorLen; a++) dist += vec[a] * Vectors[a + wordNum[1] * vectorLen];
			
			dist *= 10;
			accuracy = dist * 100 / data3;
			accuracySum += accuracy;
			fprintf(outputFile, "%s\t%lld\t%s\t%lld\t%f\t%f\t%f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], data3, dist, accuracy);
		}
	}
	accuracySum = accuracySum / testNum;
	fprintf(outputFile,"\nNumber of All Questions in Survey : %d\n", allNum);
	printf("\nNumber of All Questions in Survey : %d\n", allNum);
	fprintf(outputFile, "Number of Calculated Questions in Survey : %d\n", testNum);
	printf("Number of Calculated Questions in Survey : %d\n", testNum);
	fprintf(outputFile, "Average difference : %lf\n", accuracySum);
	printf("Average difference : %lf\n", accuracySum);

	return 0;
}
		
	