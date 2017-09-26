#pragma once

#include <iostream>
#define SIZE 9

using namespace std;

string number_code = "";

string get_puzzle();
void set_number_randomly();
bool create_final_sudoku();
void create_puzzle();