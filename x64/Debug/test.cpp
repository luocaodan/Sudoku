#include<iostream>
#include<malloc.h> 

using namespace std; 

int counter = 0;

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
		//Assert::AreNotEqual(*sudoku, *((*p)->sudoku));
		add_sudoku_to_tree(depth + 1, &((*p)->ptrs[(*sudoku)[depth + 1] - '1']), sudoku);
	}
}

int main() {
	//create_sudoku(1'000'000);
	FILE* fout = fopen("sudoku.txt", "r");

	int row_record[9] = { 0 }; // must be 511
	int column_record[9] = { 0 };
	int block_record[9] = { 0 };

	char c;
	int bit;

	string* sudoku;
	Treenode* root = create_treenode(-1, new string(""));

	while (true) {
		/* store box */
		sudoku = new string();
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				for (c = fgetc(fout); !isdigit(c) && c != EOF; c = fgetc(fout));
				if (c == EOF) {
					
					return 0;
				}
				else {
					(*sudoku) += c;
					bit = (1 << (c - '1'));
					row_record[i] |= bit;
					column_record[j] |= bit;
					block_record[(i / 3) * 3 + j / 3] |= bit;
				}
			}
		}

		/* judge & initial*/
		for (int i = 0; i < 9; i++) {
			//Assert::AreEqual(511, row_record[i]);
			//Assert::AreEqual(511, column_record[i]);
			//Assert::AreEqual(511, block_record[i]);
			row_record[i] = 0;
			column_record[i] = 0;
			block_record[i] = 0;
		}
		add_sudoku_to_tree(-1, &root, sudoku);
		
		cout << ++counter << endl;
	}
}
