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
void corp2word(const char *path, const char *filename);
ofstream writeFile, logFile;
int fileNum = 0, currntNum = 0;
int main(int argc, char **argv) {
	if (argc == 1) {
		cout << "Error!! Put the right Parameter" << endl;
		cout << "Put Parameter for pasing : <Corp Folder Path> (ex.'./corp2vec <Corp Folder Path>')" << endl;
		cout << "This Program get file data in folder you add and Parsing All file in folder." << endl;
		cout << "Be careful about Putting Parameter(Not File Path)" << endl;
		return 0;
	}

	
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

	writeFile.open("data");
	logFile.open("data_log");

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
		int vector_word_size, num;
		while (getline(readFile, string_line))								//Example '���ƴ�� ���ƴ�/�������/116+��/������/255	1989�� 1989/��������/231+��/�������������/235	���� ����/�Ϲݸ��/63'
		{
			lineNum++;
			vector_line = split(string_line, '	');							//Not '\t' Be Careful !!
			for (int i = 0; i < vector_line.size(); ++i) {					//Example '���ƴ�� ���ƴ�/�������/116+��/������/255'
																			//testFile << split(vector_line[i], ' ')[0] << " ";
				if (result.compare("") != 0) 	result = "";
				vector_word = split(vector_line[i], ' ');
				vector_word_size = vector_word.size();
				if (vector_word_size > 1) {
					vector_word = split(vector_word[1], '+');
					vector_word_size = vector_word.size();
					noun = false;
					details = false;
					for (int j = 0; j < vector_word_size; ++j) {				//Example '���ƴ�/�������/116' and '��/������/255'
						str = split(vector_word[j], '/');
						if (str[1].find("�������") != -1) {
							//none
						}
						else if (str[1].find("����") != -1) {
							//none
						}
						else if (str[1].find("���") != -1 || str[1].find("�ܱ���") != -1 || str[1].find("������") != -1) {
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
						else if (str[1].find("�����") != -1) {
							result.append(str[0]);
						}
						else if (str[1].find("�λ�") != -1) {
							result.append(str[0]);
						}
						else if (str[1].find("ȭ�����") != -1 || str[1].find("����������") != -1) {
							result.append(str[0]);
						}
						else if (str[1].find("��������") != -1) {
							for (int k = 0; k < str[0].length(); k++) {
								if (str[0][k] - 48 >= 0 && str[0][k] - 48 <= 9) {
									result.push_back('n');
								}
								else {
									result.push_back(str[0][k]);
								}
							}
						}
						else if (str[1].find("����") != -1) {
							if (str[1].find("����ȭ���̻�") != -1) {
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
						// ����(�����, ������), ����, �ܱ���, ��ȣ �ϴ�, �λ�, ��ź��
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


