#include "stdafx.h"
#include <iostream>
#include <time.h>
#include "Sudoku.h"
#include "puzzle_creator.h"

using namespace std;

string get_puzzle() {
	srand((int)time(0)); // set seed

	do {
		/* initial */
		number_code = "";
		for (int i = 0; i < SIZE * SIZE; i++) {
			number_code += '0';
		}

		/* set numbers */
		set_number_randomly();

		/* find solution */
	} while (create_final_sudoku());
	
	/* create puzzle */
	create_puzzle();

	return number_code;
}

void set_number_randomly() {
	int position;
	for (int i = 0; i < SIZE; i++) { // randomly fill 1~9
		position = rand() % (SIZE * SIZE);
		number_code[position] = i + '1';
	}
}

bool create_final_sudoku() {
	Subject_sudoku* sudoku = new Subject_sudoku(number_code);
	if (!fill_sudoku(sudoku)) {
		return false;
	}
	else {
		number_code = sudoku->to_string();
		return true;
	}
}

void create_puzzle() {
	int pos1, pos2;
	pos1 = (rand() % 9) + 1;
	while ((pos2 = (rand() % 9) + 1) != pos1);

	/* 2 freebox each block */
	for (int i = 0; i < SIZE; i++) { // each block
		number_code[(i / 3 + pos1 / 3) * 3 + (i % 3 + pos1 % 3)] = '0';
		number_code[(i / 3 + pos2 / 3) * 3 + (i % 3 + pos2 / 3)] = '0';
	}

	/* the rest of freebox */
	int freebox_num = rand() % 30 + 30 - SIZE * 2;
	int randpos;
	for (int i = 0; i < freebox_num; i++) {
		do {
			randpos = rand() % (SIZE * SIZE);
		} while (number_code[randpos] != '0');
		number_code[randpos] = '0';
	}
}



