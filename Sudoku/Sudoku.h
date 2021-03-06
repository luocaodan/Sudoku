#pragma once

#include <vector>
#include <iostream>
#define SIZE 9

using namespace std;
#ifndef __TEXT_H__
#define __TEXT_H__

/* MAIN */

int main(int argc, char* argv[]);

/* SOLVE */

class Templet {
public:
	string line[9][3]; // line[block][row]
	const string line1_position_code = "012"; // not change
	string line2_position_code = "345"; // 3! = 6 types
	string line3_position_code = "678"; // 3! = 6 types

	Templet();
	void fill_line(const string linetemp[], const string code, const int blockbegin);
	Templet* fill_line1();
	Templet* fill_line2();
	Templet* fill_line3();
	bool change2next();
	void show();
};

class Template_sudoku {
public:
	string code = "312456789";
	
	Templet* templet = new Templet();

	bool change2next();
	void record(FILE* fout, int* index, char buffer[]);
};

int create_sudoku(int number);

/* CREATE */

class Subject_sudoku;
class Group;
class Box;

class Subject_sudoku {
public:
	Group* rows[9];
	Group* columns[9];
	Group* blocks[9];

	Subject_sudoku(string sudoku_str);
	Subject_sudoku(const Subject_sudoku& sudoku);
	~Subject_sudoku();
	Box* getbox(int rowno, int columnno) const;
	Box* get_minpos_box() const;
	void initial();
	void show(FILE* fout);
	string to_string();
};

class Box {
public:
	Subject_sudoku* sudoku;
	Group*	row;
	Group*	column;
	Group*	block;
	int		posvalue = 1023;	// --- a binary number, 000000000 means certain value
	int		cervalue = 0;		// --- range from 1 to 9

	Box(Subject_sudoku* sdk, Group* r, Group* c, Group* b, int value);
	Box(Subject_sudoku* sdk, Group* r, Group* c, Group* b, int pos, int cer);
	bool iscertain();
	int make_certain(int value, int members_posvalues[3][9]);
	void cancel_certain(int posvalue, int members_posvalues[3][9]);
};

class Group {
public:
	int		number;						// --- which group?
	int		hasvalues = 0;				// --- a binary number
	Box*	members[SIZE];
	int		members_posvalues[SIZE];	// --- store members' posvalue (in case of cancel_certain)
	int		members_num = 0;			// --- only when store members 

	Group(int number, int has = 0);
	void make_certain(Box* box);
	void cancel_certain(Box* box);
	void push_back(Box* new_member);
	void initial();
	void refresh_pos();
};

int count_one(int value);
int get_valuebit(Box* member);
int get_valuebit(int value);
bool guess_value(Box* box, Subject_sudoku* sudoku, FILE* fout);
bool fill_sudoku(Subject_sudoku* sudoku, FILE* fout);
bool fill_sudoku(Subject_sudoku* sudoku);
int solve_sudoku(FILE* subject);

#endif