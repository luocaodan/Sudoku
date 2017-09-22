#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Sudoku/stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SudokuTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1) {
			
			Assert::AreEqual(1, 1);
		}

		TEST_METHOD(TestMethod2) {

		}

	};
}