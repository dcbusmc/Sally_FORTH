// File: driver3.cpp
//
// CMSC 341 Fall 2018 Project 2
//
// Simple driver program to call the Sally Forth interpreter
// This version accepts user input for filename of Sally Forth
// source code via command line arguments
//

#include <iostream>
#include <fstream>
using namespace std;

#include "Sally.h"

int main(int argc, char* args[]) {
	if (argc != 2) {
		std::cout << "Invalid number of arguments provided. Expected 2, got " << argc << std::endl;
		std::cout << "This program expects a single argument of <filename>" << std::endl;
	}
	ifstream ifile(args[1]);

	Sally S(ifile);

	S.mainLoop();

	ifile.close();
	return 0;
}