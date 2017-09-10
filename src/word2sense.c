#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

const long long max_size = 2000;         // max length of strings
const long long N = 3;                   // number of closest words
const long long max_w = 50;              // max length of vocabulary entries

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

int main(int argc, char **argv) {
	char korlex_line[max_size], corp_line[max_size] , file_name[max_size];
	char *main_p, *main_sp;
	char *sub_p, *sub_sp;
	FILE *corpFile, *korlexFile, *writeFile;
	long long senseSize,a;

	if (argc < 4) {
		printf("parameter error, Usage : ./word2sense corpus korlexData output\n");
		return 0;
	}
	strcpy(file_name, argv[1]);
	corpFile = fopen(file_name, "r");
	char data[50];
	unsigned int length, hash;

	strcpy(file_name, argv[2]);
	korlexFile = fopen(file_name, "r");
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

	strcpy(file_name, argv[3]);
	writeFile = fopen(file_name, "w");

	int count = 0;
	while (!feof(corpFile)) {
		count++;
		fgets(corp_line, sizeof(corp_line), corpFile);
		corp_line[strlen(corp_line) - 1] = '\0';
		sub_p = strtok_r(corp_line, " ", &sub_sp);
		while (sub_p != NULL) {
			int index = SearchWord(sub_p);
			if (index != -1) {
				fprintf(writeFile, "%s ", list[index].sense);
			}
			else {
				fprintf(writeFile, "%s ", sub_p);
			}
			sub_p = strtok_r(NULL, " ", &sub_sp);
		}
		fprintf(writeFile, "\n");
	}
	return 0;
}

