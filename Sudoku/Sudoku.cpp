// Sudoku.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>  
#include <algorithm>  
#include <string>  
#define ADDR 

using namespace std;

class Templet {
public:
	string line[9][3]; // line[block][row]

	const string line1_position_code = "012"; // not change
	string line2_position_code = "012"; // 3! = 6 types
	string line3_position_code = "012"; // 3! = 6 types

	Templet() {
		fill_line1();
		fill_line2();
		fill_line3();
	}

	void fill_line(const string linetemp[], const string code, const int blockbegin) {
		int blockno = blockbegin;
		for (int i = 0; i < 3; i++, blockno++) { // each block
			for (int j = 0; j < 3; j++) { // each line, begin with linetemp1
				line[blockno][(code[i] + j) % 3] = linetemp[j];
			}
		}
	}

	Templet* fill_line1() {
		const string linetemp[3] = { 
			"012", 
			"345",
			"678"
		};
		fill_line(linetemp, line1_position_code, 0);
		return this;
	}

	Templet* fill_line2() {
		const string linetemp[3] = {
			"201",
			"534",
			"867"
		};
		fill_line(linetemp, line2_position_code, 3);
		return this;
	}

	Templet* fill_line3() {
		const string linetemp[3] = {
			"120",
			"453",
			"786"
		};
		fill_line(linetemp, line3_position_code, 6);
		return this;
	}

	bool change2next() {
		if (!next_permutation(line2_position_code.begin(), line2_position_code.end())) {
			if (!next_permutation(line3_position_code.begin(), line3_position_code.end())) {
				return false;
			}
			else {
				sort(line2_position_code.begin(), line2_position_code.end()); // initial line2 code
				fill_line2();
				fill_line3();
			}
		}
		else {
			fill_line2();
		}
		return true;
	}

	void show() {
		for (int i = 0; i < 8; i += 3) { // each big line
			for (int j = 0; j < 3; j++) { // each small line
				for (int k = 0; k < 3; k++) { // each block
					cout << line[i + k][j] << ' ';
				}
				cout << '\n';
			}
			cout << '\n';
		}
		cout << '\n';
	}
};

class Template_sudoku {
public:
	string code = "312456789";
	Templet* templet = new Templet();

	bool change2next() {
		if (!next_permutation(code.begin() + 1, code.end())) {
			if (!templet->change2next()) {
				return false;
			}
			else {
				sort(code.begin() + 1, code.end());
			}
		}
		return true;
	}

	void record(FILE* fout) {
		int counter;
		for (int i = 0; i < 8; i += 3) { // each big line
			for (int j = 0; j < 3; j++) { // each small line
				counter = 0;
				for (int k = 0; k < 3; k++) { // each block
					for (char &c : templet->line[i + k][j]) {
						fputc(code[c - '0'], fout);
						fputc((counter++ < 8) ? ' ' : '\n', fout);
					}
				}
			}
		}
	}

};

int demo() {
	string str;
	cin >> str;
	sort(str.begin(), str.end());
	
	while (next_permutation(str.begin(), str.end()))
		cout << str << endl;
	return 0;
}


int create_sudoku(int number) {
	Template_sudoku* tsudo = new Template_sudoku();
	int counter = 0;
	FILE* fout = fopen("sudoku.txt", "w");
	tsudo->record(fout);
	while (tsudo->change2next() && ++counter < number) {
		fputc('\n', fout);
		tsudo->record(fout);
	}

	return 0;
}

int solve_sudoku(FILE* subject) {
	cout << "solve " << subject;
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "invalid parameter number";
		return 0; // over
	}

	char* func = argv[1];
	char* para = argv[2];
	if (strcmp(func, "-c") == 0) {
		int number = 0;
		char c;
		for (int i = 0; (c = para[i]) != '\0'; i++) {
			if (isdigit(c)) {
				number *= 10;
				number += c - '0';
			}
			else {
				cout << "invalid number";
				return 0;
			}
		}
		if (number <= 0) {
			cout << "number should be positive";
			return 0;
		}
		create_sudoku(number);
	}
	else if (strcmp(func, "-s") == 0) {
		FILE* subject;
		if ((subject = fopen(para, "r")) != NULL) {
			solve_sudoku(subject);
		}
		else {
			cout << "cannot open the file";
			return 0;
		}
	}
	else {
		cout << "unknown function";
		return 0;
	}

	return 0;
}

