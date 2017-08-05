#include<iostream>
#include<fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>
#include <error.h>
using namespace std;

vector<string> split(string str, char delimiter);
string lastStr(string str, char delimiter);
void corp2word(const char *path, const char *filename);
ofstream writeFile, logFile;
int fileNum = 0, currntNum = 0;
int main(int argc, char **argv) {
	char output_word[100], output_wordlog[100];
	if (argc != 3) {
		cout << "Error!! Put the right Parameter" << endl;
		cout << "Put Parameter for pasing : <Corp Folder Path> <OutPut File> (ex.'./corp2vec <Corp Folder Path> <OutPut File>')" << endl;
		cout << "This Program get file data in folder you add and Parsing All file in folder." << endl;
		cout << "Be careful about Putting Parameter(Not File Path)" << endl;
		return 0;
	}
	strcpy(output_word, argv[2]);
	writeFile.open(output_word);
	logFile.open(output_wordlog);
	strcpy(output_wordlog, argv[2]);
	strcat(output_word, "word");
	strcat(output_wordlog, "word_log");

	DIR *dir;
	struct dirent *ent;
	dir = opendir(argv[1]);

	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				fileNum++;
			}
		}
		closedir(dir);
	}

	dir = opendir(argv[1]);

	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				currntNum++;
				corp2word(argv[1], ent->d_name);
			}
		}
		closedir(dir);
	}

	writeFile.close();
	logFile.close();
	return 0;
}

void corp2word(const char *path, const char *filename) {
	string string_line, result;
	vector<string> vector_line, vector_word;
	int lineNum = 0;
	ifstream readFile;
	
	const size_t len1 = strlen(path);
	const size_t len2 = strlen(filename);
	char name[100];

	memcpy(name, path, len1);
	memcpy(name + len1, filename, len2 + 1);

	readFile.open(name);

	//ofstream testFile;
	//testFile.open("sentence");
	if (readFile.is_open()) {
		logFile << "====================== " << filename << "("<<currntNum<<"/"<< fileNum << ") file parsing Start." << "======================" << endl;
		vector<string> str;
		string temp;
		bool noun, details;
		int vector_word_size;
		while (getline(readFile, string_line))								//Example '동아대는 동아대/고유명사/116+는/보조사/255	1989년 1989/수관형사/231+년/단위성의존명사/235	전국 전국/일반명사/63'
		{
			lineNum++;
			vector_line = split(string_line, '	');							//Not '\t' Be Careful !!
			for (unsigned i = 0; i < vector_line.size(); ++i) {					//Example '동아대는 동아대/고유명사/116+는/보조사/255'
				//testFile << split(vector_line[i], ' ')[0] << " ";
				if (result.compare("") != 0) 	result = "";
				vector_word = split(vector_line[i], ' ');
				vector_word_size = vector_word.size();
				if (vector_word_size > 1) {
					vector_word = split(vector_word[1], '+');
					vector_word_size = vector_word.size();
					noun = false;
					details = false;
					for (int j = 0; j < vector_word_size; ++j) {				//Example '동아대/고유명사/116' and '는/보조사/255'
						str = split(vector_word[j], '/');
						if (str[1].find("전성어미") != string::npos) {
							//none
						}
						else if (str[1].find("조사") != string::npos) {
							//none
						}
						else if (str[1].find("명사") != string::npos || str[1].find("외국어") != string::npos || str[1].find("서수사") != string::npos) {
							if (noun) {
								temp.append("+");
								temp.append(str[0]);
								result.append(str[0]);
								details = true;
							}
							else {
								temp = str[0];
								result.append(str[0]);
							}
							noun = true;
						}
						else if (str[1].find("형용사") != string::npos) {
							result.append(str[0]);
						}
						else if (str[1].find("부사") != string::npos) {
							result.append(str[0]);
						}
						else if (str[1].find("화폐단위") != string::npos || str[1].find("도량형단위") != string::npos) {
							result.append(str[0]);
						}
						else if (str[1].find("수관형사") != string::npos) {
							for (unsigned k = 0; k < str[0].length(); k++) {
								if (str[0][k] - 48 >= 0 && str[0][k] - 48 <= 9) {
									result.push_back('n');
								}
								else {
									result.push_back(str[0][k]);
								}
							}
						}
						else if (str[1].find("동사") != string::npos) {
							if (str[1].find("동사화접미사") != string::npos) {
								temp.append("+");
								temp.append(str[0]);
								result.append(str[0]);
								details = true;
							}
							else {
								if (result.compare("") != 0) result.append(" ");
								result.append(str[0]);
								break;
							}
						}
						//else{
						// 수사(양수사, 서수사), 한자, 외국어, 부호 싹다, 부사, 감탄사
						//}
					}
					if (details) {
						result.append("(");
						result.append(temp);
						result.append(")");
					}
					if (result.compare("") != 0)	writeFile << result << " ";
				}
				else
				{
					logFile << lineNum << " : " << vector_line[i] << endl;
				}
			}
			//writeFile << endl;
			//testFile << endl;
		}
		logFile << "======================" << filename << "' file parsing Finish." << "======================\n" << endl;
		readFile.close();
		//testFile.close();
	}
	else {
		cout << "File not Exist. Please Check Input File" << endl;
	}
}
vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str);
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

string path(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str);
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	vector<string> external;
	stringstream dd(str);
	string result;

	return tok;
}


