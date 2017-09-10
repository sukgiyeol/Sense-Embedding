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

int *hash_array, hash_size;
struct sense_data *list;
bool senseFlag = false;

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
	FILE *f,*s;
	char inputChar[max_size], inputWord[2][max_size];
	char file_name[max_size], korlex_line[max_size];;
	float dist, len, vec[max_size];
	long long size, a, b, c, words, vectorLen,senseSize,wordNum[100];
	char ch;
	float *Vectors;
	char *vocab, *main_p, *main_sp;;

	if (argc == 2 || argc == 4) {}
	else {
		printf("Usage: ./distance-pair <File> <Option>");
		printf("where FILE contains word projections in the BINARY FORMAT\n");
		printf("Options:\n");
		printf("\t-s <sense file>\n");
		printf("\t\tIn sense file, First Line is Number of Datas\n");
		printf("\t\tOthewr Lines are Sense number and word (ex. 223123   home)\n");
		return 0;
	}

	strcpy(file_name, argv[1]);
	f = fopen(file_name, "rb");
	if (f == NULL) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf(f, "%lld", &words);                                                                                                                                                                                                          //%lld long long Data
	fscanf(f, "%lld", &vectorLen);
	vocab = (char *)malloc((long long)words * max_width * sizeof(char));
	Vectors = (float *)malloc((long long)words * (long long)vectorLen * sizeof(float));
	if (Vectors == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * vectorLen * sizeof(float) / 1048576, words, vectorLen);
		return -1;
	}

	for (b = 0; b < words; b++) {
		fscanf(f, "%s%c", &vocab[b * max_width], &ch);
		for (a = 0; a < vectorLen; a++) fread(&Vectors[a + b * vectorLen], sizeof(float), 1, f);
		len = 0;
		for (a = 0; a < vectorLen; a++) len += Vectors[a + b * vectorLen] * Vectors[a + b * vectorLen];
		len = sqrt(len);
		for (a = 0; a < vectorLen; a++) Vectors[a + b * vectorLen] /= len;
	}
	fclose(f);

	if (argc == 4 && strcmp(argv[2], "-s") == 0) {
		senseFlag = true;
		char data[50];
		unsigned int length, hash;

		strcpy(file_name, argv[3]);
		s = fopen(file_name, "r");
		if (s == NULL) {
			printf("Sense file not found\n");
			return -1;
		}
		fscanf(s, "%lld", &senseSize);

		hash_size = senseSize * 10;
		hash_array = (int *)calloc(hash_size, sizeof(int));
		list = (struct sense_data*)malloc((long long)senseSize * sizeof(struct sense_data));

		for (a = 0; a < hash_size; a++) hash_array[a] = -1;
		for (a = 0; a < senseSize; a++) {
			ReadWord(data, s);
			length = strlen(data) + 1;
			list[a].sense = (char *)calloc(length, sizeof(char));
			strcpy(list[a].sense, data);

			ReadWord(data, s);
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
			if (inputChar[c] == 0) break;
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
		if (senseFlag) {                       //    wordTosense part @@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			for (int t = 0; t<2; t++) {
				int tmp = SearchWord(inputWord[t]);
				if (tmp != -1) {
					strcpy(inputWord[t], list[tmp].sense);
				}
			}
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

		if (wordNum[0] == -1) {
			printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n\n", inputWord[0]);
			goto back;
		}
		if (wordNum[1] == -1) {
			printf("  |- Input '%s' is Out of dictionary word or Bad Vector!\n\n", inputWord[1]);
			goto back;
		}
		if (senseFlag) {                                     // senseToword part @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			printf("\n--------- Cosine Distance Between Two Words! ---------\n");
			for (a = 0; a < 2; a++) {
				int index = SearchSense(inputWord[a]);
				char* tmp = (a == 0) ? "Fisrt" : "Second";
				if (index != -1) {
					printf("   |- Input %s Word : ( %s", tmp, list[index].word);
					index++;
					while (!strcmp(inputWord[a], list[index].sense)) {
						printf(", %s", list[index].word);
						index++;
					}
					printf(")");
				}
				else {
					printf("   |- Input %s Word : ( %s )", tmp, inputWord[a]);
				}
				printf("\t(%lld)\n", wordNum[a]);
			}
		}
		else {
			printf("\n--------- Cosine Distance Between Two Words! ---------\n");
			printf("   |- Input First Word : ( %s )(%lld)\n", inputWord[0], wordNum[0]);
			printf("   |- Input Second Word : ( %s )(%lld)\n", inputWord[1], wordNum[1]);
		}
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

