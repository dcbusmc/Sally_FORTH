// File: Sally.cpp
//
// CMSC 341 Fall 2018 Project 2
//
// Implementation of member functions of Sally Forth interpreter
//

#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <stdexcept>
#include <cstdlib>
using namespace std;

#include "Sally.h"


// Basic Token constructor. Just assigns values.
//
Token::Token(TokenKind kind, int val, string txt) {
	m_kind = kind;
	m_value = val;
	m_text = txt;
}


// Basic SymTabEntry constructor. Just assigns values.
//
SymTabEntry::SymTabEntry(TokenKind kind, int val, operation_t fptr) {
	m_kind = kind;
	m_value = val;
	m_dothis = fptr;
}


// Constructor for Sally Forth interpreter.
// Adds built-in functions to the symbol table.
//
Sally::Sally(istream& input_stream) :
	istrm(input_stream)  // use member initializer to bind reference
{

	symtab["DUMP"] = SymTabEntry(KEYWORD, 0, &doDUMP);

	symtab["+"] = SymTabEntry(KEYWORD, 0, &doPlus);
	symtab["-"] = SymTabEntry(KEYWORD, 0, &doMinus);
	symtab["*"] = SymTabEntry(KEYWORD, 0, &doTimes);
	symtab["/"] = SymTabEntry(KEYWORD, 0, &doDivide);
	symtab["%"] = SymTabEntry(KEYWORD, 0, &doMod);
	symtab["NEG"] = SymTabEntry(KEYWORD, 0, &doNEG);

	symtab["."] = SymTabEntry(KEYWORD, 0, &doDot);
	symtab["SP"] = SymTabEntry(KEYWORD, 0, &doSP);
	symtab["CR"] = SymTabEntry(KEYWORD, 0, &doCR);

	// Added operations
	symtab["DUP"] = SymTabEntry(KEYWORD, 0, &doDUP);
	symtab["DROP"] = SymTabEntry(KEYWORD, 0, &doDROP);
	symtab["SWAP"] = SymTabEntry(KEYWORD, 0, &doSWAP);
	symtab["ROT"] = SymTabEntry(KEYWORD, 0, &doROT);

	symtab["SET"] = SymTabEntry(KEYWORD, 0, &doSET);
	symtab["@"] = SymTabEntry(KEYWORD, 0, &doFETCH);
	symtab["!"] = SymTabEntry(KEYWORD, 0, &doSTORE);

	symtab["<"] = SymTabEntry(KEYWORD, 0, &doLESSTHAN);
	symtab["<="] = SymTabEntry(KEYWORD, 0, &doLESSOREQUAL);
	symtab["=="] = SymTabEntry(KEYWORD, 0, &doISEQUAL);
	symtab["!="] = SymTabEntry(KEYWORD, 0, &doISNOTEQUAL);
	symtab[">="] = SymTabEntry(KEYWORD, 0, &doGREATEROREQUAL);
	symtab[">"] = SymTabEntry(KEYWORD, 0, &doGREATERTHAN);

	symtab["AND"] = SymTabEntry(KEYWORD, 0, &doAND);
	symtab["OR"] = SymTabEntry(KEYWORD, 0, &doOR);
	symtab["NOT"] = SymTabEntry(KEYWORD, 0, &doNOT);

	symtab["IFTHEN"] = SymTabEntry(KEYWORD, 0, &doIFTHEN);
	symtab["ELSE"] = SymTabEntry(KEYWORD, 0, &doELSE);
	symtab["ENDIF"] = SymTabEntry(KEYWORD, 0, &doENDIF);

	symtab["DO"] = SymTabEntry(KEYWORD, 0, &doDO);
	symtab["UNTIL"] = SymTabEntry(KEYWORD, 0, &doUNTIL);

	// isRecording = false;
	isRecording = 0;
}


// This function should be called when tkBuffer is empty.
// It adds tokens to tkBuffer.
//
// This function returns when an empty line was entered 
// or if the end-of-file has been reached.
//
// This function returns false when the end-of-file was encountered.
// 
// Processing done by fillBuffer()
//   - detects and ignores comments.
//   - detects string literals and combines as 1 token
//   - detetcs base 10 numbers
// 
//
bool Sally::fillBuffer() {
	string line;     // single line of input
	int pos;         // current position in the line
	int len;         // # of char in current token
	long int n;      // int value of token
	char *endPtr;    // used with strtol()


	while (true) {    // keep reading until empty line read or eof

					  // get one line from standard in
					  //
		getline(istrm, line);

		// if "normal" empty line encountered, return to mainLoop
		//
		if (line.empty() && !istrm.eof()) {
			return true;
		}

		// if eof encountered, return to mainLoop, but say no more
		// input available
		//
		if (istrm.eof()) {
			return false;
		}


		// Process line read

		pos = 0;                      // start from the beginning

									  // skip over initial spaces & tabs
									  //
		while (line[pos] != '\0' && (line[pos] == ' ' || line[pos] == '\t')) {
			pos++;
		}

		// Keep going until end of line
		//
		while (line[pos] != '\0') {

			// is it a comment?? skip rest of line.
			//
			if (line[pos] == '/' && line[pos + 1] == '/') break;

			// is it a string literal? 
			//
			if (line[pos] == '.' && line[pos + 1] == '"') {

				pos += 2;  // skip over the ."
				len = 0;   // track length of literal

						   // look for matching quote or end of line
						   //
				while (line[pos + len] != '\0' && line[pos + len] != '"') {
					len++;
				}

				// make new string with characters from
				// line[pos] to line[pos+len-1]
				string literal(line, pos, len);  // copy from pos for len chars

												 // Add to token list
												 //
				tkBuffer.push_back(Token(STRING, 0, literal));

				// Different update if end reached or " found
				//
				if (line[pos + len] == '\0') {
					pos = pos + len;
				}
				else {
					pos = pos + len + 1;
				}

			}
			else {  // otherwise "normal" token

				len = 0;  // track length of token

						  // line[pos] should be an non-white space character
						  // look for end of line or space or tab
						  //
				while (line[pos + len] != '\0' && line[pos + len] != ' ' && line[pos + len] != '\t') {
					len++;
				}

				string literal(line, pos, len);   // copy form pos for len chars
				pos = pos + len;

				// Try to convert to a number
				//
				n = strtol(literal.c_str(), &endPtr, 10);

				if (*endPtr == '\0') {
					tkBuffer.push_back(Token(INTEGER, n, literal));
				}
				else {
					tkBuffer.push_back(Token(UNKNOWN, 0, literal));
				}
			}

			// skip over trailing spaces & tabs
			//
			while (line[pos] != '\0' && (line[pos] == ' ' || line[pos] == '\t')) {
				pos++;
			}

		}
	}
}



// Return next token from tkBuffer.
// Call fillBuffer() if needed.
// Checks for end-of-file and throws exception 
//
Token Sally::nextToken() {
	Token tk;
	bool more = true;

	while (more && tkBuffer.empty()) {
		more = fillBuffer();
	}

	if (!more && tkBuffer.empty()) {
		throw EOProgram("End of Program");
	}

	tk = tkBuffer.front();
	tkBuffer.pop_front();
	return tk;
}


// The main interpreter loop of the Sally Forth interpreter.
// It gets a token and either push the token onto the parameter
// stack or looks for it in the symbol table.
//
//
void Sally::mainLoop() {

	Token tk;
	map<string, SymTabEntry>::iterator it;

	try {
		while (1) {
			tk = nextToken();

			
			if (isRecording != 0) {
				//recordBuffer.push_back(tk);
				nestedDOStack.top().push_back(tk);
			}
			

			if (tk.m_kind == INTEGER || tk.m_kind == STRING) {

				// if INTEGER or STRING just push onto stack
				params.push(tk);

			}
			else {
				it = symtab.find(tk.m_text);

				if (it == symtab.end()) {   // not in symtab

					params.push(tk);

				}
				else if (it->second.m_kind == KEYWORD) {

					// invoke the function for this operation
					//
					it->second.m_dothis(this);

				}
				else if (it->second.m_kind == VARIABLE) {

					// variables are pushed as tokens
					//
					tk.m_kind = VARIABLE;
					params.push(tk);

				}
				else {

					// default action
					//
					params.push(tk);

				}
			}
		}

	}
	catch (EOProgram& e) {

		cerr << "End of Program\n";
		if (params.size() == 0) {
			cerr << "Parameter stack empty.\n";
		}
		else {
			cerr << "Parameter stack has " << params.size() << " token(s).\n";
		}

	}
	catch (out_of_range& e) {

		cerr << "Parameter stack underflow??\n" << e.what();

	}
	catch (...) {

		cerr << "Unexpected exception caught\n";

	}
}

// -------------------------------------------------------


void Sally::doPlus(Sally *Sptr) {
	Token p1, p2;

	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for +.");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	int answer = p2.m_value + p1.m_value;
	Sptr->params.push(Token(INTEGER, answer, ""));
}


void Sally::doMinus(Sally *Sptr) {
	Token p1, p2;

	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for -.");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	int answer = p2.m_value - p1.m_value;
	Sptr->params.push(Token(INTEGER, answer, ""));
}


void Sally::doTimes(Sally *Sptr) {
	Token p1, p2;

	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for *.");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	int answer = p2.m_value * p1.m_value;
	Sptr->params.push(Token(INTEGER, answer, ""));
}


void Sally::doDivide(Sally *Sptr) {
	Token p1, p2;

	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for /.");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	int answer = p2.m_value / p1.m_value;
	Sptr->params.push(Token(INTEGER, answer, ""));
}


void Sally::doMod(Sally *Sptr) {
	Token p1, p2;

	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for %.");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	int answer = p2.m_value % p1.m_value;
	Sptr->params.push(Token(INTEGER, answer, ""));
}


void Sally::doNEG(Sally *Sptr) {
	Token p;

	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for NEG.");
	}
	p = Sptr->params.top();
	Sptr->params.pop();
	Sptr->params.push(Token(INTEGER, -p.m_value, ""));
}


void Sally::doDot(Sally *Sptr) {

	Token p;
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for .");
	}

	p = Sptr->params.top();
	Sptr->params.pop();

	if (p.m_kind == INTEGER) {
		cout << p.m_value;
	}
	else {
		cout << p.m_text;
	}
}


void Sally::doSP(Sally *Sptr) {
	cout << " ";
}


void Sally::doCR(Sally *Sptr) {
	cout << endl;
}

void Sally::doDUMP(Sally *Sptr) {
	// do whatever for debugging
	Sally copy;
	Token p;
	copy.params = Sptr->params;

	while (!copy.params.empty()) {
		p = copy.params.top();
		if (p.m_kind == INTEGER) {
			cout << p.m_value;
		}
		else {
			cout << p.m_text;
		}
		copy.params.pop();
	}
}

//
// Added Operations
//

void Sally::doDUP(Sally *Sptr) {
	Token p;
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for DUP");
	}

	p = Sptr->params.top();
	Sptr->params.push(p);
}

void Sally::doDROP(Sally *Sptr) {
	Sptr->params.pop();
}

void Sally::doSWAP(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for SWAP");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	Sptr->params.push(p1);
	Sptr->params.push(p2);
}

void Sally::doROT(Sally *Sptr) {
	Token p1, p2, p3;
	if (Sptr->params.size() < 3) {
		throw out_of_range("Need three parameters for ROT");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();
	p3 = Sptr->params.top();
	Sptr->params.pop();

	Sptr->params.push(p2);
	Sptr->params.push(p1);
	Sptr->params.push(p3);
}

void Sally::doSET(Sally *Sptr) {
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for SET");
	}
	Token p1, p2;
	map<string, SymTabEntry>::iterator it;
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	it = Sptr->symtab.find(p1.m_text);
	if (it == Sptr->symtab.end()) {
		Sptr->symtab[p1.m_text] = SymTabEntry(VARIABLE, p2.m_value);
	}
	else {
		cout << "This variable already exists!" << endl;
	}	
}

void Sally::doFETCH(Sally *Sptr) {

	if (Sptr->params.size() < 1) {
		throw out_of_range("Need two parameters for SET");
	}
	Token p1;
	map<string, SymTabEntry>::iterator it;
	p1 = Sptr->params.top();
	Sptr->params.pop();

	it = Sptr->symtab.find(p1.m_text);
	if (it == Sptr->symtab.end()) {
		cout << "No such variable exists!" << endl;
	}
	else {
		Sptr->params.push(Token(INTEGER, Sptr->symtab.at(p1.m_text).m_value, ""));
	}
}

void Sally::doSTORE(Sally *Sptr) {
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need two parameters for SET");
	}
	Token p1, p2;
	map<string, SymTabEntry>::iterator it;
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	it = Sptr->symtab.find(p1.m_text);
	if (it == Sptr->symtab.end()) {
		cout << "No such variable exists!" << endl;
	}
	else {
		Sptr->symtab[p1.m_text].m_value = p2.m_value;
	}
}

void Sally::doLESSTHAN(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	<");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value < p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doLESSOREQUAL(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	<=");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value <= p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doISEQUAL(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	==");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value == p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doISNOTEQUAL(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	!=");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value != p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doGREATEROREQUAL(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	>=");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value >= p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doGREATERTHAN(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	>");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p2.m_value > p1.m_value) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doAND(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	AND");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p1.m_value && p2.m_value != 0) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doOR(Sally *Sptr) {
	Token p1, p2;
	if (Sptr->params.size() < 2) {
		throw out_of_range("Need two parameters for	OR");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	p2 = Sptr->params.top();
	Sptr->params.pop();

	if (p1.m_value || p2.m_value != 0) {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
}

void Sally::doNOT(Sally *Sptr) {
	Token p1;
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for	NOT");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();

	if (p1.m_value != 0) {
		Sptr->params.push(Token(INTEGER, 0, ""));
	}
	else {
		Sptr->params.push(Token(INTEGER, 1, ""));
	}
}

void Sally::doIFTHEN(Sally *Sptr) {
	Token p1, p2;
	int count_IF = 0;
	int count_END = 0;
	bool keep_going = true;
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for IFTHEN");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();

	if (p1.m_value == 0) {
		while (keep_going) {
			p2 = Sptr->nextToken();
			if (p2.m_text == "IFTHEN") {
				count_IF++;
			}
			if (p2.m_text == "ENDIF") {
				count_END++;
			}
			if ((count_IF == count_END) && (p2.m_text == "ELSE")) {
				keep_going = false;
				Sptr->symtab["ELSE"].m_value = 1;
			}
		}		
	}
	else {
		Sptr->symtab["ELSE"].m_value = 0;
	}
	
}

void Sally::doELSE(Sally *Sptr) {
	Token p1;
	int count_IF = 1;
	int count_END = 0;
	bool keep_going = true;
	if (Sptr->symtab["ELSE"].m_value == 0) {
		while (keep_going) {
			p1 = Sptr->nextToken();
			if (p1.m_text == "IFTHEN") {
				count_IF++;
			}
			if (p1.m_text == "ENDIF") {
				count_END++;
			}
			if ((count_IF == count_END) && (p1.m_text == "ENDIF")) {
				keep_going = false;
			}
		}
	}
}

void Sally::doENDIF(Sally *Sptr) {
	Sptr->symtab["ELSE"].m_value = 0;
}

void Sally::doDO(Sally *Sptr) {
	
	//Sptr->isRecording = true;
	Sptr->isRecording++;
	list<Token> temp;
	Sptr->nestedDOStack.push(temp);
}

void Sally::doUNTIL(Sally *Sptr) {
	Token p1;
	if (Sptr->params.size() < 1) {
		throw out_of_range("Need one parameter for UNTIL");
	}
	p1 = Sptr->params.top();
	Sptr->params.pop();
	if (p1.m_value == 1) {
		//Sptr->recordBuffer.clear();
		//Sptr->isRecording = false;
		if (Sptr->isRecording > 1) {
			list<Token> temp = Sptr->nestedDOStack.top();
			Sptr->nestedDOStack.pop();
			Sptr->nestedDOStack.top().splice(Sptr->nestedDOStack.top().end(), temp);
		}
		Sptr->isRecording--;
	}
	else {
		list<Token>::iterator it = Sptr->tkBuffer.begin();
		//Sptr->tkBuffer.splice(it, Sptr->recordBuffer);
		Sptr->tkBuffer.splice(it, Sptr->nestedDOStack.top());
		Sptr->nestedDOStack.top().clear();
		
	}
}