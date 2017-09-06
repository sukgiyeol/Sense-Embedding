#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#define buf 1024
#define classNum 15

typedef enum { false, true } bool;
void corp2word(const char *path, const char *filename);
bool corp2word_init(const char *filename);
bool compound;

FILE *wordclass, *word;
char wordC[classNum][25];

int main(int argc, char **argv) {
	DIR *dir;
	struct dirent *ent;
	int fileNum = 0, currntNum = 0;

	//Parameter Check
	if (argc == 3 || argc == 4) {}
	else {
		printf("Usage: ./corp2vec <Corp Folder Path> <OutPut File Path> <Compound>\n");
		printf("This Program get Corp Files in 'Folder'. so you input Folder Path into <Corp Folder Path>.\n");
		printf("Option <Compound> divide Compoound noun into each noun. (0 or 1... default 0)\n");
		printf("\tYou don't need to input parameter <Compoud>\n");
		printf("\nExamples:\n");
		return 0;
	}
	
	if (argc == 4 && !strcmp(argv[3], "1")) compound = true; 
	else	compound = false;

	dir = opendir(argv[1]);
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				fileNum++;
			}
		}
		closedir(dir);
	}

	if (!corp2word_init(argv[2])) return -1;

	dir = opendir(argv[1]);
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				currntNum++;
				printf("Program is parsing the Corp(%s[%d/%d])\n", ent->d_name, currntNum, fileNum);
				corp2word(argv[1], ent->d_name);
				fflush(stdout);
			}
		}
		closedir(dir);
	}

	fclose(wordclass);
	fclose(word);

	return 0;
}
bool corp2word_init(const char *filename) {
	int i = 0;
	wordclass = fopen("wordclass", "r");
	if (wordclass == NULL) {
		printf("File 'wordclass' not found. so Create 'wordclass' file\n");
		printf("Change File Encoding to utf-8\n");
		printf(" 1. vi wordclass\n");
		printf(" 2. :set fileencoding=utf-8\n");
		printf(" 3. :wq\n\n");
		wordclass = fopen("wordclass", "w");
		fprintf(wordclass, "전성어미\n");
		fprintf(wordclass, "조사\n");
		fprintf(wordclass, "단위성의존명사\n");
		fprintf(wordclass, "명사\n");
		fprintf(wordclass, "외국어\n");
		fprintf(wordclass, "서수사\n");
		fprintf(wordclass, "형용사\n");
		fprintf(wordclass, "부사\n");
		fprintf(wordclass, "화폐단위\n");
		fprintf(wordclass, "도량형단위\n");
		fprintf(wordclass, "수관형사\n");
		fprintf(wordclass, "동사화접미사\n");
		fprintf(wordclass, "동사\n");
		fprintf(wordclass, "하다\n");
		fprintf(wordclass, "온점");
		fclose(wordclass);
		return false;
	}

	while (!feof(wordclass)) {
		fscanf(wordclass, "%s\n", wordC[i]);
		i++;
	}

	if (i != classNum) {
		fclose(wordclass);
		printf("Checking 'wordclass' is fail. so Create new 'wordclass' file\n");
		printf("Change File Encoding to utf-8\n");
		printf(" 1. vi wordclass\n");
		printf(" 2. :set fileencoding=utf-8\n");
		printf(" 3. :wq\n\n");
		wordclass = fopen("wordclass", "w");
		fprintf(wordclass, "전성어미\n");
		fprintf(wordclass, "조사\n");
		fprintf(wordclass, "단위성의존명사\n");
		fprintf(wordclass, "명사\n");
		fprintf(wordclass, "외국어\n");
		fprintf(wordclass, "서수사\n");
		fprintf(wordclass, "형용사\n");
		fprintf(wordclass, "부사\n");
		fprintf(wordclass, "화폐단위\n");
		fprintf(wordclass, "도량형단위\n");
		fprintf(wordclass, "수관형사\n");
		fprintf(wordclass, "동사화접미사\n");
		fprintf(wordclass, "동사\n");
		fprintf(wordclass, "하다\n");
		fprintf(wordclass, "온점");
		fclose(wordclass);
		return false;
	}

	word = fopen(filename, "w");
	if (word == NULL) {
		printf("File '%s' not open\n", filename);
		return false;
	}

	return true;
}

void corp2word(const char *path, const char *filename) {
	FILE *corp;
	char name[100];
	int i, j;
	char line[buf];
	char *main_p, *sub_p, *sub2_p, *sub3_p;
	char *main_sp, *sub_sp, *sub2_sp, *sub3_sp;
	char *main_temp, *sub_temp, *sub2_temp;
	char *contain;
	bool noun, number, number_deter, details, point;				
	char result[buf], temp[buf], ctemp[3];

	const size_t len1 = strlen(path);
	const size_t len2 = strlen(filename);
	memcpy(name, path, len1);
	memcpy(name + len1, filename, len2 + 1);

	corp = fopen(name, "r");

	if (corp == NULL) {
		printf("Corp file(%s) not found\n", filename);
		return;
	}

	while (!feof(corp))
	{
		fgets(line, sizeof(line), corp);						//Example '동아대는 동아대/고유명사/116+는/보조사/255	1989년 1989/수관형사/231+년/단위성의존명사/235	전국 전국/일반명사/63'
		main_p = strtok_r(line, "	", &main_sp);				//Not '\t' Be Careful !!
		while (main_p != NULL) {								//Example '동아대는 동아대/고유명사/116+는/보조사/255'
			main_temp = main_p;
			sub_p = strtok_r(main_temp, " ", &sub_sp);
			sub_p = strtok_r(NULL, " ", &sub_sp);

			strcpy(result, "");
			strcpy(temp, "");

			noun = false;	
			number = false;
			number_deter = false;
			details = false;
			point = false;
			if (sub_p != NULL) {								//Example '동아대/고유명사/116+는/보조사/255'
				sub_temp = sub_p;
				sub2_p = strtok_r(sub_temp, "+", &sub2_sp);
				while (sub2_p != NULL) {						//Example '동아대/고유명사/116'
					j = 0;
					contain = strstr(sub2_p, wordC[14]);		//온점
					if (contain != NULL) {
						j = 7;
						goto s0;
					}
					contain = strstr(sub2_p, wordC[0]);		//전성어미
					if (contain != NULL)  goto s0;

					contain = strstr(sub2_p, wordC[1]);		//조사
					if (contain != NULL)  goto s0;

					contain = strstr(sub2_p, wordC[2]);		//단위성의존명사
					if (contain != NULL) {
						j = 5;
						goto s0;
					}
					contain = strstr(sub2_p, wordC[3]);		//명사
					if (contain != NULL) {
						j = 1;
						goto s0;
					}
					contain = strstr(sub2_p, wordC[4]);		//외국어
					if (contain != NULL) {
						j = 1;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[5]);		//서수사
					if (contain != NULL) {
						j = 1;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[6]);		//형용사
					if (contain != NULL) {
						j = 2;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[7]);		//부사
					if (contain != NULL) {
						j = 2;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[8]);		//화폐단위
					if (contain != NULL) {
						j = 5;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[9]);		//도량형단위
					if (contain != NULL) {
						j = 5;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[10]);		//수관형사
					if (contain != NULL) {
						j = 3;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[11]);		//동사화접미사
					if (contain != NULL) {
						j = 6;
						goto s0;
					}

					contain = strstr(sub2_p, wordC[12]);		//동사
					if (contain != NULL) {
						j = 4;
						goto s0;
					}

	

				s0:
					sub2_temp = sub2_p;
					sub3_p = strtok_r(sub2_temp, "/", &sub3_sp);	//Example '동아대'

					if (j == 1) {
						if (noun) {
							strcat(temp, "+");
							strcat(temp, sub3_p);
							details = true;
						}
						else	strcpy(temp, sub3_p);
						noun = true;
						strcat(result, sub3_p);
					}
					else if (j == 2) {
						strcat(result, sub3_p);
					}
					else if (j == 3) {
						if (number)	strcat(result, " ");		//11개/수관형사+23개/수관형사 -> n 개
						number_deter = false;					//11개/수관형사 -> n 개
						for (i = 0; i < strlen(sub3_p); i++) {
							if (sub3_p[i] - 48 >= 0 && sub3_p[i] - 48 <= 9) {
								if (!number_deter) {
									strcat(result, "n");
									number = true;
									number_deter = true;
								}
							}
							else if (sub3_p[i] == '.') {
								break;
							}
							else if (sub3_p[i] == ',') {
								//none
							}
							else {
								number = true;
								ctemp[0] = sub3_p[i++];
								ctemp[1] = sub3_p[i++];
								ctemp[2] = sub3_p[i++];
								if (number_deter)	strcat(result, " ");
								strcat(result, ctemp);
								break;								//232명2323 => n 명
							}
						}
					}
					else if (j == 4) {
						if (noun) {
							strcat(temp, "+");
							strcat(temp, sub3_p);
							details = true;
						}
						strcat(result, sub3_p);
						break;
					}
					else if (j == 5) {
						if (number) strcat(result, " ");
						strcat(result, sub3_p);
					}
					else if (j == 6) {
						strcat(result, wordC[13]);
						break;
					}
					else if (j == 7) {
						strcat(result, "\n");
						point = true;
						break;
					}
					sub2_p = strtok_r(NULL, "+", &sub2_sp);
				}

				if (details && compound) {
					strcat(result, "(");
					strcat(result, temp);
					strcat(result, ")");
				}

				if (strcmp(result, "") != 0) {
					if(point) fprintf(word, "%s", result);
					else fprintf(word, "%s ", result);
				}

			}
			main_p = strtok_r(NULL, "	", &main_sp);
		}
	}
	fclose(corp);

}