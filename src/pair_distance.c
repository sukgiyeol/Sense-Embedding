#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 
const long long max_size = 2000;         // max length of strings
const long long max_width = 50; 		 // max length of word alphabet

int main(int argc, char **argv) {
	FILE *vectorFile;
	char inputChar[max_size], inputWord[2][max_size];
	char fileName[max_size];
	float dist, len, vec[max_size];
	long long words, vectorLen, wordNum[100];
	long long a, b, c;			//loof index
	char ch;
	float *Vectors;
	char *vocab;

	//Parameter Check
	if (argc < 2) {
		printf("Usage: ./pair_distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n\n");
		return 0;
	}

	//Vector File(bin) open and Read word and vector array.
	strcpy(fileName, argv[1]);
	vectorFile = fopen(fileName, "rb");
	if (vectorFile == NULL) {
		printf("Input file not found\n");
		return -1;
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
	while (1) {
	back:
		printf("Enter two words separated by space (EXIT to break): ");
		
		//Save input word to variable
		a = 0;
		while (1) {
			inputChar[a] = fgetc(stdin);
			if ((inputChar[a] == '\n') || (a >= max_size - 1)) {
				inputChar[a] = 0;
				break;
			}
			a++;
		}
		if (!strcmp(inputChar, "EXIT")) break;
		a = 0;
		b = 0;
		c = 0;
		while (1) {
			inputWord[a][b] = inputChar[c];
			b++;
			c++;
			inputWord[a][b] = 0;
			if (inputChar[c] == 0 ) break;
			if (inputChar[c] == ' ') {
				a++;
				b = 0;
				c++;
				if (a > 1) {
					break;
				}
			}
		}
		if (a != 1) {
			printf("  |- Two word are not enter by input parameter\n\n");
			goto back;
		}
		
		//Get Number of two Words in vocabulary
		for (a = 0; a < 2; a++) {
			for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_width], inputWord[a])) break;
			if (b == words) b = -1;
			wordNum[a] = b;
			if (b == -1) {
				printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n\n", inputWord[a]);
				goto back;
			}
		}

		if (wordNum[0] == -1 ) {
			printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n\n", inputWord[0]);
			goto back;
		}
		if (wordNum[1] == -1) {
			printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n\n", inputWord[1]);
			goto back;
		}

		printf("\n--------- Cosine Distance Between Two Words! ---------\n");
		printf("   |- Input First Word : %s(%lld)\n", inputWord[0], wordNum[0]);
		printf("   |- Input Second Word : %s(%lld)\n", inputWord[1], wordNum[1]);

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
		printf("   |- Distance : %f\n\n", dist);		
	}
	return 0;
}