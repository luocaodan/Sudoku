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
#define BUFFER_SIZE 1'000'000

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
		next_permutation(line3_position_code.begin(), line3_position_code.end());
		sort(line2_position_code.begin(), line2_position_code.end()); // initial line2 code
		fill_line2();
		fill_line3();
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
		templet->change2next();
		sort(code.begin() + 1, code.end());
	}
	return true;
}

void Template_sudoku::record(FILE* fout, int* index, char buffer[]) {
	int counter;

	for (int i = 0; i < 8; i += 3) { // each big line
		for (int j = 0; j < 3; j++) { // each small line
			counter = 0;
			for (int k = 0; k < 3; k++) { // each block
				for (char &c : templet->line[i + k][j]) {
					if ((*index) >= BUFFER_SIZE - 5) {
						buffer[(*index)] = '\0';
						fputs(buffer, fout);
						(*index) = 0;
					}
					buffer[(*index)++] = code[c - '0'];
					buffer[(*index)++] = (counter++ < 8) ? ' ' : '\n';
				}
			}
		}
	}
}

int create_sudoku(int number) {
	Template_sudoku* tsudo = new Template_sudoku();
	int counter = 0;
	FILE* fout = fopen("sudoku.txt", "w");
	int index = 0;
	char buffer[BUFFER_SIZE];
	tsudo->record(fout, &index, buffer);
	while (tsudo->change2next() && ++counter < number) {
		buffer[index++] = '\n';
		tsudo->record(fout, &index, buffer);
	}
	buffer[index] = '\0';
	fputs(buffer, fout);

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

int Box::make_certain(int value, int members_posvalues[3][9]) {
	int posvalue = this->posvalue;
	this->cervalue = value;
	this->posvalue = 0;
	for (int i = 0; i < 9; i++) {
		members_posvalues[0][i] = this->row->members[i]->posvalue;
		members_posvalues[1][i] = this->column->members[i]->posvalue;
		members_posvalues[2][i] = this->block->members[i]->posvalue;
	}
	this->row->make_certain(this);
	this->column->make_certain(this);
	this->block->make_certain(this);
	return posvalue;
}

void Box::cancel_certain(int posvalue, int members_posvalues[3][9]) {
	this->row->cancel_certain(this);
	this->column->cancel_certain(this);
	this->block->cancel_certain(this);
	for (int i = 0; i < 9; i++) {
		this->row->members[i]->posvalue = members_posvalues[0][i];
		this->column->members[i]->posvalue = members_posvalues[1][i];
		this->block->members[i]->posvalue = members_posvalues[2][i];
	}

	this->posvalue = posvalue;
	this->cervalue = 0;
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

Subject_sudoku::~Subject_sudoku() {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			delete rows[i]->members[j];
		}
		delete rows[i];
		delete columns[i];
		delete blocks[i];
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
	char buffer[200];
	int index = 0;
	for (int i = 0; i < SIZE; i++) {
		buffer[index++] = getbox(i, 0)->cervalue + '0';
		for (int j = 1; j < SIZE; j++) {
			buffer[index++] = ' ';
			buffer[index++] = getbox(i, j)->cervalue + '0';
		}
		buffer[index++] = '\n';
	}
	buffer[index++] = '\n';
	buffer[index++] = '\0';
	fputs(buffer, fout);
}

string Subject_sudoku::to_string() {
	string sudoku = "";
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			sudoku += (getbox(i, j)->cervalue + '0');
		}
	}
	return sudoku;
}


Group::Group(int number, int has) {
	this->number = number;
	this->hasvalues = has;
}

void Group::make_certain(Box* box) {
	hasvalues |= get_valuebit(box);
	refresh_pos();
}

void Group::cancel_certain(Box* box) {
	hasvalues &= ~get_valuebit(box);	// -- reset hasvalue
}

void Group::push_back(Box* new_member) {
	this->members[this->members_num++] = new_member;
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
	for (int i = 0; i < SIZE; i++) {
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
			//Subject_sudoku* new_sudoku = new Subject_sudoku(*sudoku);
			Box* box = sudoku->getbox(rowno, columnno);
			int members_posvalues[3][9];
			int posvalue = box->make_certain(i + 1, members_posvalues);
			if (fill_sudoku(sudoku, fout)) {
				return true;
			}
			//delete(new_sudoku);
			box->cancel_certain(posvalue, members_posvalues);
		}
	}
	return false;
}

bool fill_sudoku(Subject_sudoku* sudoku, FILE* fout) { // -- succeed(true) or failed(false)
	Box* box;
	box = sudoku->get_minpos_box();
	//cout << sudoku->to_string() << endl;
	if (box == NULL) {
		sudoku->show(fout);
		return true;
	}
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
				if (number > 1'000'000) {
					cout << "the number is out of range";
					return 0;
				}
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
}

/*=========================
|	   CREATE PUZZLE      |
=========================*/

bool fill_sudoku(Subject_sudoku* sudoku) { // -- succeed(true) or failed(false)
	Box* box;
	box = sudoku->get_minpos_box();
	if (box == NULL) {
		return true;
	}
	return guess_value(box, sudoku, NULL);
}