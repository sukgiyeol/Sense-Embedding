#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define buf 1024
int start = 0;
int end = 0;

int index_range(char* word);

int main(int argc, char **argv) {
	char korlex_line[buf], corp_line[buf];
	char *main_p, *main_sp;
	char *sub_p, *sub_sp;
	FILE *corpFile, *korlexFile, *writeFile;
	int i=0;

  corpFile = fopen("bigtext11", "r");
  korlexFile = fopen("korlex_data", "r");
  writeFile = fopen("sense-corp-11", "w");

  fgets(korlex_line, sizeof(korlex_line), korlexFile);

  int sizeX = atoi(korlex_line);

  char **arr = (char**)malloc(sizeof(char*)*sizeX);
  char **sense_num = (char**)malloc(sizeof(char*)*sizeX);

  for (int i = 0; i < sizeX; ++i){
    arr[i] = (char*)malloc(sizeof(char)*buf);
    sense_num[i] = (char*)malloc(sizeof(char)*10);
    memset(arr[i], 0, sizeof(char)*buf);
    memset(sense_num[i], 0, sizeof(char)*10);
    fgets(korlex_line, sizeof(korlex_line), korlexFile);
    korlex_line[strlen(korlex_line) - 1] = '\0';
    main_p = strtok_r(korlex_line, "	", &main_sp);
    main_p = strtok_r(NULL, "	", &main_sp);
    strcpy(sense_num[i],main_p);

    main_p = strtok_r(NULL, "	", &main_sp);
    strcpy(arr[i],main_p);
  }

  float line = 0;
  int count = 0;
  while(!feof(corpFile)){
  	line = ((float)count/2727710)*100;
  	printf("%.3f\% \n", line);
  	count++;
    fgets(corp_line, sizeof(corp_line), corpFile);
    corp_line[strlen(corp_line) - 1] = '\0';
    sub_p = strtok_r(corp_line, " ", &sub_sp);
    while (sub_p != NULL) {
      if(index_range(sub_p) == 1){
        start = end - 1;
      }
      for (int i = start; i < end; ++i){
      	if (strcmp(sub_p, arr[i]) == 0){
      		fprintf(writeFile, "%s ", sense_num[i]);
      		break;
      	}
      	if (i == end - 1){
      		fprintf(writeFile, "%s ", sub_p);
      	}
      }
      sub_p = strtok_r(NULL, " ", &sub_sp);
    }
    system("clear");
    fprintf(writeFile,"\n");
  }
  return 0;
}

int index_range(char* word){
  if (strcmp(word, "가") < 0){
    return 1;
  }// not find sense!
  if (strcmp(word, "나") < 0){
    start = 0;
    end = 23487;
    return 0;
  }
  if (strcmp(word, "다") < 0){
    start = 23487;
    end = 29761;
    return 0;
  }
  if (strcmp(word, "라") < 0){
    start = 29761;
    end = 40376;
    return 0;
  }
  if (strcmp(word, "마") < 0){
    start = 40376;
    end = 42879;
    return 0;
  }
  if (strcmp(word, "바") < 0){
    start = 42879;
    end = 54309;
    return 0;
  }
  if (strcmp(word, "사") < 0){
    start = 54309;
    end = 71181;
    return 0;
  }
  if (strcmp(word, "아") < 0){
    start = 71181;
    end = 93025;
    return 0;
  }
  if (strcmp(word, "자") < 0){
    start = 93025;
    end = 121017;
    return 0;
  }
  if (strcmp(word, "차") < 0){
    start = 121017;
    end = 139623;
    return 0;
  }
  if (strcmp(word, "카") < 0){
    start = 139623;
    end = 145958;
    return 0;
  }
  if (strcmp(word, "타") < 0){
    start = 145958;
    end = 149938;
    return 0;
  }
  if (strcmp(word, "파") < 0){
    start = 149938;
    end = 154513;
    return 0;
  }
  if (strcmp(word, "하") < 0){
    start = 154513;
    end = 161544;
    return 0;
  }
  else{
    start = 161544;
    end = 171856; // +1
    return 0;
  }
}