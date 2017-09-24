#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Sudoku/Sudoku.h"
#include "../Sudoku/Sudoku.cpp"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SudokuTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		typedef struct node{
			bool isbottom;
			int depth;
			string* sudoku;
			struct node* ptrs[9];
		}Treenode;

		Treenode* create_treenode(int depth, string* sudoku) {
			Treenode* p = (Treenode*)malloc(sizeof(Treenode));
			p->isbottom = true;
			p->depth = depth;
			p->sudoku = sudoku;
			for (int i = 0; i < 9; i++) {
				p->ptrs[i] = NULL;
			}
			return p;
		}

		void add_sudoku_to_tree(int depth, Treenode** p, string* sudoku) {
			if ((*p) == NULL) {
				(*p) = create_treenode(depth, sudoku);
			}
			else {
				if ((*((*p)->sudoku)).length() > 0) {
					Assert::AreNotEqual(*sudoku, *((*p)->sudoku));
					add_sudoku_to_tree(depth + 1, &((*p)->ptrs[(*((*p)->sudoku))[depth + 1] - '1']), ((*p)->sudoku));
					*((*p)->sudoku) = "";
				}
				add_sudoku_to_tree(depth + 1, &((*p)->ptrs[(*sudoku)[depth + 1] - '1']), sudoku);
			}
		}

		void test_c(char para[], int number) {
			char* argv[3] = {
				"Sudoku.exe",
				"-c",
				para
			};
			main(3, argv);

			FILE* fout = fopen("sudoku.txt", "r");

			int row_record[9] = { 0 }; // must be 511
			int column_record[9] = { 0 };
			int block_record[9] = { 0 };

			char c;
			int bit;

			string* sudoku;
			Treenode* root = create_treenode(-1, new string(""));
			int counter = 0;

			while (true) {
				// store box 
				sudoku = new string();
				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9; j++) {
						for (c = fgetc(fout); !isdigit(c) && c != EOF; c = fgetc(fout));
						if (c == EOF) {
							if (number != -1) {
								Assert::AreEqual(number, counter);
							}	
							return;
						}
						else {
							//cout << 1;
							(*sudoku) += c;
							bit = (1 << (c - '1'));
							row_record[i] |= bit;
							column_record[j] |= bit;
							block_record[(i / 3) * 3 + j / 3] |= bit;
						}
					}
				}

				// judge & initial
				for (int i = 0; i < 9; i++) {
					Assert::AreEqual(511, row_record[i]);
					Assert::AreEqual(511, column_record[i]);
					Assert::AreEqual(511, block_record[i]);
					row_record[i] = 0;
					column_record[i] = 0;
					block_record[i] = 0;
				}
				add_sudoku_to_tree(-1, &root, sudoku);
				counter++;
			}

			fclose(fout);
		}

		void test_s(char path[]) {
			/*char* argv[3] = {
				"Sudoku.exe",
				"-s",
				"C:\\Users\\65486\\Desktop\\subject.txt"
			};
			main(3, argv);*/
			solve_sudoku(fopen("C:\\Users\\65486\\Desktop\\subject.txt", "r"));

			FILE* fout = std::fopen("sudoku.txt", "r");

			int row_record[9] = { 0 }; // must be 511
			int column_record[9] = { 0 };
			int block_record[9] = { 0 };

			char c;
			int bit;

			while (true) {
				// store box 
				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9; j++) {
						for (c = fgetc(fout); !isdigit(c) && c != EOF; c = fgetc(fout));
						if (c == EOF) {
							return;
						}
						else {
							bit = (1 << (c - '1'));
							row_record[i] |= bit;
							column_record[j] |= bit;
							block_record[(i / 3) * 3 + j / 3] |= bit;
						}
					}
				}

				// judge & initial
				for (int i = 0; i < 9; i++) {
					Assert::AreEqual(511, row_record[i]);
					Assert::AreEqual(511, column_record[i]);
					Assert::AreEqual(511, block_record[i]);
					row_record[i] = 0;
					column_record[i] = 0;
					block_record[i] = 0;
				}
			}

			fclose(fout);
		}

		TEST_METHOD(c0) {
			test_c("1", 1);
		}

		TEST_METHOD(c1) {
			test_c("10", 10);
		}

		TEST_METHOD(c2) {
			test_c("100", 100);
		}

		TEST_METHOD(c3) {
			test_c("1000", 1000);
		}

		TEST_METHOD(c4) {
			test_c("10000", 10000);
		}

		TEST_METHOD(c5) {
			test_c("100000", 100000);
		}

		TEST_METHOD(c6) {
			test_c("1000000", 1000000);
		}

		TEST_METHOD(c_zero) {
			test_c("000005", 5);
		}

		TEST_METHOD(s1) {
			test_s("subject.txt");
		}
	};
}