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
		while (getline(readFile, string_line))								//Example '동아대는 동아대/고유명사/116+는/보조사/255	1989년 1989/수관형사/231+년/단위성의존명사/235	전국 전국/일반명사/63'
		{
			vector_line = split(string_line, '	');							//Not '\t' Be Careful !!
			for (int i = 0; i < vector_line.size(); ++i) {					//Example '동아대는 동아대/고유명사/116+는/보조사/255'
				//cout << split(vector_line[i], ' ')[0] << " ";

				if (result.compare("") != 0) 	result = "";
				vector_word = split(split(vector_line[i], ' ')[1], '+');
				vector_word_size = vector_word.size();
				for (int j = 0; j < vector_word_size; ++j) {				//Example '동아대/고유명사/116' and '는/보조사/255'
					str = split(vector_word[j], '/');
					if (str[1].find("명사") != -1 || str[1].find("외국어") != -1 || str[1].find("서수사") != -1) {
						result.append(str[0]);
					}
					else if (str[1].find("수관형사") != -1) {
						for (int k = 0; k < str[0].length(); k++) 
							if (0 <= str[0][k] && str[0][k] <= 9) { result.append("N"); }
							else { 
								temp = str[0][k];
								result.append(temp);
							}
					}
					else if (str[1].find("동사") != -1) {
						if (result.compare("") != 0) result.append("!");
						result.append(str[0]);
						break;
					}
					//else{
					// 수사(양수사, 서수사), 한자, 
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