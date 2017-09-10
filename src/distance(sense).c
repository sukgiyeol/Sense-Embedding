#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> // mac os x
typedef enum { false, true } bool;

const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
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
      if (a >= max_w - 1) a--;   // Truncate too long words
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
   FILE *f, *s;
   char st1[max_size], bestw[N][max_size], file_name[max_size], st[100][max_size];
   char ch, *vocab;
   float dist, len, bestd[N], vec[max_size];
   float *M;
   long long words, size, a, b, c, d, cn, bi[100], senses;

   if (argc == 2 || argc == 4) {}
   else {
      printf("Usage: ./distance <File> <Option>");
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

   //read binfile
   fscanf(f, "%lld", &words);
   fscanf(f, "%lld", &size);
   vocab = (char *)malloc((long long)words * max_w * sizeof(char));
   M = (float *)malloc((long long)words * (long long)size * sizeof(float));
   if (M == NULL) {
      printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
      return -1;
   }
   for (b = 0; b < words; b++) {
      fscanf(f, "%s%c", &vocab[b * max_w], &ch);
      for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
      len = 0;
      for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
      len = sqrt(len);
      for (a = 0; a < size; a++) M[a + b * size] /= len;
   }
   fclose(f);

   //read sense file
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
      fscanf(s, "%lld", &senses);

      hash_size = senses * 10;
      hash_array = (int *)calloc(hash_size, sizeof(int));
      list = (struct sense_data*)malloc((long long)senses * sizeof(struct sense_data));

      for (a = 0; a < hash_size; a++) hash_array[a] = -1;
      for (a = 0; a < senses; a++) {
         ReadWord(data, s);
         length = strlen(data) + 1;
         list[a].sense = (char *)calloc(length, sizeof(char));
         strcpy(list[a].sense, data);

         ReadWord(data, s);
         length = strlen(data) + 1;
         list[a].word = (char *)calloc(length, sizeof(char));
         strcpy(list[a].word, data);
      }
      qsort(list, senses, sizeof(struct sense_data), StrNumCompare);
      for (a = 0; a < senses; a++) {
         hash = GetHashKey(list[a].sense);
         while (hash_array[hash] != -1) hash = (hash + 1) % hash_size;
         hash_array[hash] = a;
      }

      for (a = 0; a < senses; a++) {
         hash = GetHashKey(list[a].word);
         while (hash_array[hash] != -1) hash = (hash + 1) % hash_size;
         hash_array[hash] = a;
      }
   }

   while (1) {
      for (a = 0; a < N; a++) bestd[a] = 0;
      for (a = 0; a < N; a++) bestw[a][0] = 0;
      printf("Enter word or sentence (EXIT to break): ");
      a = 0;
      while (1) {
         st1[a] = fgetc(stdin);
         if ((st1[a] == '\n') || (a >= max_size - 1)) {
            st1[a] = 0;
            break;
         }
         a++;
      }
      if (!strcmp(st1, "EXIT")) break;
      cn = 0;
      b = 0;
      c = 0;
      while (1) {
         st[cn][b] = st1[c];
         b++;
         c++;
         st[cn][b] = 0;
         if (st1[c] == 0) break;
         if (st1[c] == ' ') {
            cn++;
            b = 0;
            c++;
         }
      }
      cn++;

      if (senseFlag) {
         for (a = 0; a < cn; a++) {
            b = SearchWord(st[a]);
            if (b != -1) {
               strcpy(st[a], list[b].sense);
            }
         }
      }

      for (a = 0; a < cn; a++) {
         for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
         if (b == words) b = -1;
         bi[a] = b;
         if (senseFlag) {
            printf("\nWord: %s", st[a]);
            c = SearchSense(st[a]);
            if (c != -1) {
               printf("(%s", list[c].word);
               c++;
               while (!strcmp(st[a], list[c].sense)) {
                  printf(", %s", list[c].word);
                  c++;
               }
               printf(")");
            }
            printf(" Position in vocabulary : %lld\n", bi[a]);
         }
         else {
            printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
         }
         if (b == -1) {
            printf("Out of dictionary word!\n");
            break;
         }
      }

      if (b == -1) continue;
      printf("\n\tWord\t|\tCosine distance\n---------------------------------------------\n");
      for (a = 0; a < size; a++) vec[a] = 0;
      for (b = 0; b < cn; b++) {
         if (bi[b] == -1) continue;
         for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
      }
      len = 0;
      for (a = 0; a < size; a++) len += vec[a] * vec[a];
      len = sqrt(len);
      for (a = 0; a < size; a++) vec[a] /= len;
      for (a = 0; a < N; a++) bestd[a] = 0;
      for (a = 0; a < N; a++) bestw[a][0] = 0;
      for (c = 0; c < words; c++) {
         a = 0;
         for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
         if (a == 1) continue;
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

      if (senseFlag) {
         for (a = 0; a < N; a++) {
            c = SearchSense(bestw[a]);
            if (c != -1) {
                printf("( %s", list[c].word);
                c++;
                while (!strcmp(bestw[a], list[c].sense)) {
                    printf(", %s", list[c].word);
                    c++;
                }
		printf(" )");
            }
	    else {
		printf("( %s )", bestw[a]);
            }	
            printf("\t\t%f\n", bestd[a]);
         }
      }
      else {
         for (a = 0; a < N; a++) printf("%lld\t%s\t%f\n", a, bestw[a], bestd[a]);
      }
   }
   return 0;
}
