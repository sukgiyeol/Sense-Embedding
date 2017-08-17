#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 
typedef enum { false, true } bool;
const long long max_size = 2000;         // max length of strings
const long long max_width = 50; 		 // max length of word alphabet

int main(int argc, char **argv) {
	FILE *vectorFile, *testFile, *outputFile;
	bool option, found;
	char ch, filename[100], data[2][20];
	char *vocab;
	float dist, len, vec[max_size];
	float survey_result, accuracySum = 0.0, accuracy = 0.0;
	float *Vectors;
	long long words, vectorLen, wordNum[100];
	long long a, b;			//loof index
	int testNum = 0, allNum = 0;
	
	//Parameter Check
	if (argc == 3 || argc == 5) {
		option = false;
		if (argc == 5) {
			if (strcmp(argv[3], "-o")==0) {
				option = true;
			}
		}
	}
	else {
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return 0;
	}

	//Vector File(bin) open and Read word and vector array.
	strcpy(filename, argv[1]);
	vectorFile = fopen(filename, "rb");		//read binary
	if (vectorFile == NULL) {
		printf("<Vector File> not found\n");
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	strcpy(filename, argv[2]);
	testFile = fopen(filename, "r");
	if (testFile == NULL) {
		printf("<Test File> not found\n");
		printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
		printf("Vector File contains word projections in the Binary Format.\n");
		printf("Output File is Option. You don't need to use it.\n\n");
		return -1;
	}

	if (option) {
		strcpy(filename, argv[4]);
		outputFile = fopen(filename, "w");
		if (outputFile == NULL) {
			printf("<Output File> not found\n");
			printf("Usage: ./distance-pair-accuracy <Vector File> <Test File> -o <Output File>\n");
			printf("Vector File contains word projections in the Binary Format.\n");
			printf("Output File is Option. You don't need to use it.\n\n");
			return -1;
		}
	}

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

	//Loop Main Function
	if(option) fprintf(outputFile, "word\tvocabNum\tword\tvocabNum\tservey\tdistance\tmatching rate\n");
	while (!feof(testFile)) {
		fscanf(testFile, "%s\t%s\t%f", &data[0][0], &data[1][0], &survey_result);
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
			if (option)	fprintf(outputFile, "%s\t%lld\t%s\t%lld\t%.4f\t%.4f\t%.4f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], survey_result, 0.00, 0.00);
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
			if (option) fprintf(outputFile, "%s\t%lld\t%s\t%lld\t%.4f\t%.4f\t%.4f(%%)\n", &data[0][0], wordNum[0], &data[1][0], wordNum[1], survey_result, dist, accuracy);
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
		
	