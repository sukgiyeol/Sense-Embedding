#include<iostream>
#include<fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> split(string str, char delimiter);

int main(int argc, char **argv) {
	string string_line, result;
	vector<string> vector_line, vector_word;

	ifstream readFile;
	readFile.open("11");

	if (readFile.is_open()) {
		vector<string> str;
		string temp;
		int vector_word_size;
		while (getline(readFile, string_line))								//Example '���ƴ�� ���ƴ�/�������/116+��/������/255	1989�� 1989/��������/231+��/�������������/235	���� ����/�Ϲݸ��/63'
		{
			vector_line = split(string_line, '	');							//Not '\t' Be Careful !!
			for (int i = 0; i < vector_line.size(); ++i) {					//Example '���ƴ�� ���ƴ�/�������/116+��/������/255'
				//cout << split(vector_line[i], ' ')[0] << " ";

				if (result.compare("") != 0) 	result = "";
				vector_word = split(split(vector_line[i], ' ')[1], '+');
				vector_word_size = vector_word.size();
				for (int j = 0; j < vector_word_size; ++j) {				//Example '���ƴ�/�������/116' and '��/������/255'
					str = split(vector_word[j], '/');
					if (str[1].find("���") != -1 || str[1].find("�ܱ���") != -1 || str[1].find("������") != -1) {
						result.append(str[0]);
					}
					else if (str[1].find("��������") != -1) {
						for (int k = 0; k < str[0].length(); k++) 
							if (0 <= str[0][k] && str[0][k] <= 9) { result.append("N"); }
							else { 
								temp = str[0][k];
								result.append(temp);
							}
					}
					else if (str[1].find("����") != -1) {
						if (result.compare("") != 0) result.append("!");
						result.append(str[0]);
						break;
					}
					//else{
					// ����(�����, ������), ����, 
					//}
				}
				if(result.compare("")!=0)	cout << result;
			}
			cout << endl;
		}
		readFile.close();
	}
	else {
		cout << "File not Exist. Please Check Input File.\n";
	}
	return 0;
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