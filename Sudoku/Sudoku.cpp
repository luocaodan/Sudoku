// Sudoku.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Sudoku.h"

#include <iostream>  
#include <algorithm>  
#include <string> 
#include <vector>
#define SIZE 9
#define GET_BLOCKNO(ROWNO, COLUMNNO) (ROWNO / 3) * 3 + (COLUMNNO / 3)

using namespace std;

/*=========================
|	    CREATE MODE       |
=========================*/
Templet::Templet() {
	fill_line1();
	fill_line2();
	fill_line3();
}

void Templet::fill_line(const string linetemp[], const string code, const int blockbegin) {
	int blockno = blockbegin;
	for (int i = 0; i < 3; i++, blockno++) { // each block
		for (int j = 0; j < 3; j++) { // each line, begin with linetemp1
			line[blockno][(code[i] + j) % 3] = linetemp[j];
		}
	}
}

Templet* Templet::fill_line1() {
	const string linetemp[3] = { 
		"012", 
		"345",
		"678"
	};
	fill_line(linetemp, line1_position_code, 0);
	return this;
}

Templet* Templet::fill_line2() {
	const string linetemp[3] = {
		"201",
		"534",
		"867"
	};
	fill_line(linetemp, line2_position_code, 3);
	return this;
}

Templet* Templet::fill_line3() {
	const string linetemp[3] = {
		"120",
		"453",
		"786"
	};
	fill_line(linetemp, line3_position_code, 6);
	return this;
}

bool Templet::change2next() {
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

void Templet::show() {
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

bool Template_sudoku::change2next() {
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

void Template_sudoku::record(FILE* fout) {
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

int create_sudoku(int number) {
	Template_sudoku* tsudo = new Template_sudoku();
	int counter = 0;
	FILE* fout = fopen("sudoku.txt", "w");
	tsudo->record(fout);
	while (tsudo->change2next() && ++counter < number) {
		fputc('\n', fout);
		tsudo->record(fout);
	}
	fclose(fout);
	return 0;
}

/*=========================
|	    SOLVE MODE        |
=========================*/
Box::Box(Subject_sudoku* sdk, Group* r, Group* c, Group* b, int value) {
	this->sudoku = sdk;
	this->block = b;
	this->row = r;
	this->column = c;
	if (value == 0) { // unknown
		this->cervalue = 0;
		this->posvalue = 511;
	}
	else { // certain
		this->cervalue = value;
		this->posvalue = 0;
	}
}

Box::Box(Subject_sudoku* sdk, Group* r, Group* c, Group* b, int pos, int cer) {
	this->sudoku = sdk;
	this->block = b;
	this->row = r;
	this->column = c;
	this->posvalue = pos;
	this->cervalue = cer;
}

bool Box::iscertain() {
	return cervalue != 0;
}

void Box::make_certain(int value) {
	this->cervalue = value;
	this->posvalue = 0;
	this->row->make_certain(this);
	this->column->make_certain(this);
	this->block->make_certain(this);
}

Subject_sudoku::Subject_sudoku(string sudoku_str) {
	//cout << sudoku_str << endl;
	for (int i = 0; i < SIZE; i++) {
		rows[i] = new Group(i);
		columns[i] = new Group(i);
		blocks[i] = new Group(i);
	}
	int counter = 0;
	int rowno, columnno, blockno;
	for (char &c : sudoku_str) { // -- create boxes and put into groups
		rowno = counter / 9;
		columnno = counter % 9;
		blockno = GET_BLOCKNO(rowno, columnno);
		Box* box = new Box(this, rows[rowno], columns[columnno], blocks[blockno], (c - '0'));
		rows[rowno]->push_back(box);
		columns[columnno]->push_back(box);
		blocks[blockno]->push_back(box);
		counter++;
	}
	initial();
}

/* [copy construction] */
Subject_sudoku::Subject_sudoku(const Subject_sudoku& sudoku) {
	//cout << "create" << endl;
	for (int i = 0; i < SIZE; i++) {
		rows[i] = new Group(i,sudoku.rows[i]->hasvalues);
		columns[i] = new Group(i, sudoku.columns[i]->hasvalues);
		blocks[i] = new Group(i, sudoku.blocks[i]->hasvalues);
	}
	int counter = 0;
	int blockno;
	for (int rowno = 0; rowno < SIZE; rowno++) {
		for (int columnno = 0; columnno < SIZE; columnno++) {
			blockno = GET_BLOCKNO(rowno, columnno);
			Box* oldbox = sudoku.getbox(rowno, columnno);
			Box* box = new Box(this, rows[rowno], columns[columnno], blocks[blockno], oldbox->posvalue, oldbox->cervalue);
			rows[rowno]->push_back(box);
			columns[columnno]->push_back(box);
			blocks[blockno]->push_back(box);
			counter++;
		}
	}
}

Box* Subject_sudoku::getbox(int rowno, int columnno) const {
	return this->rows[rowno]->members[columnno];
}

Box* Subject_sudoku::get_minpos_box() const {
	int minpos = SIZE + 1;
	int pos;
	Box* minbox = NULL;
	Box* box;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			box = getbox(i, j);
			if (!box->iscertain()) {
				pos = count_one(box->posvalue);
				//cout << pos << endl;
				if (pos < minpos) {
					minpos = pos;
					minbox = box;
				}
			}
		}
	}	
	return minbox;
}

void Subject_sudoku::initial() {
	for (int i = 0; i < SIZE; i++) {
		rows[i]->initial();
		columns[i]->initial();
		blocks[i]->initial();
	}
}

void Subject_sudoku::show(FILE* fout) {
	for (int i = 0; i < SIZE; i++) {
		fputc(getbox(i, 0)->cervalue + '0', fout);
		for (int j = 1; j < SIZE; j++) {
			fputc(' ', fout);
			fputc(getbox(i, j)->cervalue + '0', fout);
		}
		fputc('\n', fout);
	}
	fputc('\n', fout);
}


Group::Group(int number, int has) {
	this->number = number;
	this->hasvalues = has;
}

void Group::make_certain(Box* box) {
	hasvalues |= get_valuebit(box);
	refresh_pos();

	/* it might cause same numbers, because function make_certain could 
	be execused before posibilities(rows, columns, blocks) being refreshed */

	/*int posvalue_counter[SIZE];
	for (int i = 0; i < SIZE; i++) {
		members[i]->posvalue &= (~hasvalues);
		posvalue_counter[i] = 0;
	}
	int bit = 1;
	for (int i = 0; i < SIZE; i++) { // -- each member
		if (!members[i]->iscertain()) {
			bit = 1;
			for (int j = 0; j < SIZE; j++) { // -- each bit
				posvalue_counter[j] += ((members[i]->posvalue & bit) != 0);
				bit = bit << 1;
			}
		}
	}
	bit = 1;
	for (int i = 0; i < SIZE; i++) {
		if (posvalue_counter[i] == 1) {
			for (int j = 0; j < SIZE; j++) {
				if (members[j]->posvalue & (bit << i)) {
					members[j]->make_certain(i + 1);
					break;
				}
			}
		}
	}*/
}

void Group::push_back(Box* new_member) {
	this->members.push_back(new_member);
}

void Group::initial() {
	/* collect values */
	for (size_t i = 0; i < SIZE; i++) {
		if (members[i]->iscertain()) {
			hasvalues |= get_valuebit(members[i]); // -- add cervalue to hasvalue
		}
	}
	/* initial members' posvalue */
	refresh_pos();
}

void Group::refresh_pos() {
	for (size_t i = 0; i < SIZE; i++) {
		members[i]->posvalue &= (~hasvalues); // -- remove impossible value bit
	}
}

int count_one(int value) {
	int counter = 0;
	while (value) {
		value &= value - 1;
		counter++;
	}
	return counter;
}

int get_valuebit(Box* member) {
	return (1 << ((member->cervalue) - 1));
}
int get_valuebit(int value) {
	return (1 << value);
}

bool guess_value(Box* box, Subject_sudoku* sudoku, FILE* fout) {
	//cout << "guess" << endl;
	int rowno = box->row->number;
	int columnno = box->column->number;
	for (int i = 0; i < SIZE; i++) {
		if (box->posvalue & get_valuebit(i)) { // -- value i+1 is possible
			Subject_sudoku* new_sudoku = new Subject_sudoku(*sudoku);
			new_sudoku->getbox(rowno, columnno)->make_certain(i+1);
			if (fill_sudoku(new_sudoku, fout)) {
				return true;
			}
			delete(new_sudoku);
		}
	}
	return false;
}

bool fill_sudoku(Subject_sudoku* sudoku, FILE* fout) { // -- succeed(true) or failed(false)
	Box* box;
	
	box = sudoku->get_minpos_box();
	
	if (box == NULL) {
		sudoku->show(fout);
		return true;
	}
	//cout << box->row->number << ',' << box->column->number << endl;
	return guess_value(box, sudoku, fout);
}

int solve_sudoku(FILE* subject) {
	Subject_sudoku* sudoku;
	string code = "";
	int number_counter = 0;
	char c;

	FILE* fout;
	fout = fopen("sudoku.txt", "w");

	while ((c = fgetc(subject)) != EOF) {
		if (isdigit(c)) {
			code += c;
			number_counter++;
		}
		if (number_counter == SIZE * SIZE) {
			number_counter = 0;
			sudoku = new Subject_sudoku(code);
			if (!fill_sudoku(sudoku, fout)) {
				cout << "no solutions" << endl;
			};
			delete(sudoku);
			code = "";
		}
	}
	fclose(fout);
	return 0;
}

/*=========================
|	     ENTRANCE         |
=========================*/
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
			fclose(subject);
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