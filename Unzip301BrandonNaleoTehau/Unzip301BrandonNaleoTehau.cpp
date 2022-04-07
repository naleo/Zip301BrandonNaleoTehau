// Unzip301BrandonNaleoTehau.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <sstream>
#include "../Zip301BrandonNaleoTehau/Node.h"

using namespace std;

map<string, char> encodings;

void readHeader(ifstream&);
void insertCodeIntoTree(string, char);
void generateTree();
string readEncodedContent(ifstream&);
string decode();
void outputFile(ofstream&, string);

Node* head = new Node(0);
string encodedContents, decodedContents;
int encodedContentSize;

int main(int argc, char** argv)
{

	auto start = std::chrono::high_resolution_clock::now();
	auto step = std::chrono::high_resolution_clock::now();
	//no command line argument
	if (argc < 2) {
		cout << "Please include the name of the file you wish to decompress." << endl;
		return -1;
	}

	string filename = argv[1];
	ifstream ifs;
	ifs.open(filename, ios::in | ios::binary);

	//file not found
	if (!ifs) {
		cout << "Error opening file: " << filename << endl;
		return -1;
	}
	
	readHeader(ifs);
	auto step1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = step1 -  step;
	cout << "Reding in Header: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();


	encodedContents = readEncodedContent(ifs);
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Read Encoded Data From File: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	decodedContents = decode();
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Decode File: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	string outputfilename;
	stringstream t(filename);
	getline(t, outputfilename, '.');
	outputfilename += "2.txt";
	ofstream ofs;
	ofs.open(outputfilename, ios::out | ios::binary);

	outputFile(ofs, decodedContents);
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Outputting Content: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 - start;
	cout << "Total Runtime: " << elapsed.count() << "s" << endl;
}

void readHeader(ifstream& ifs) {
	string line;
	string code, character;
	while (true) {
		ifs >> line;
		if (line == "*****") {
			break;
		}
		code = line;
		ifs >> character;
		if (character == "newline") {
			character = "\n";
		}
		else if (character == "return") {
			character = "\r";
		}
		else if (character == "space") {
			character = " ";
		}
		else if (character == "tab") {
			character = "\t";
		}
		insertCodeIntoTree(code, character[0]);
	}
}


void insertCodeIntoTree(string code, char c) {
	Node* n = head;
	for (int i = 0; i < code.length(); i++) {
		if (i != code.length() - 1) {
			if (code[i] == '0') {
				if (n->getLeftChild() == nullptr) {
					n->setLeftChild(new Node(0));
				}
				n = n->getLeftChild();
			}
			else if (code[i] == '1') {
				if (n->getRightChild() == nullptr) {
					n->setRightChild(new Node(0));
				}
				n = n->getRightChild();
			}
		}
		else {
			if (code[i] == '0') {
				if (n->getLeftChild() == nullptr) {
					n->setLeftChild(new Node(0,c));
				}
			}
			else if (code[i] == '1') {
				if (n->getRightChild() == nullptr) {
					n->setRightChild(new Node(0, c));
				}
			}
		}
	}
}


string readEncodedContent(ifstream& ifs) {
	string s;
	ifs >> s;
	encodedContentSize = stoi(s);

	ifstream::pos_type start = ifs.tellg();
	ifs.seekg(0, ios::end);
	ifstream::pos_type end = ifs.tellg();
	ifs.seekg(start);

	vector<char> bytes(end - start);
	ifs.read(bytes.data(), end - start);


	return string(bytes.data(), end - start);
}

string decode() {
	string output = "";
	Node* n = head;
	int count = 0;
	for (int i = 0; i < encodedContents.size(); i++) {
		if (i < 2 && (encodedContents[i] == '\n' || encodedContents[i] == '\r')) { //skip first newline stuffs
			continue;
		}
		bitset<8> b(encodedContents.c_str()[i]); //bitset parses backwards?
		for (int j = 7; j >= 0; j--) {
			if (count < encodedContentSize) {
				if (b[j] == 0) {
					n = n->getLeftChild();
					if (n->isLeaf()) {
						output += n->getLetter();
						n = head;
					}
				}
				else if (b[j] == 1) {
					n = n->getRightChild();
					if (n->isLeaf()) {
						output += n->getLetter();
						n = head;
					}
				}
				count++;
			}
		}
		b.reset();
	}
	return output;
}

void outputFile(ofstream& ofs, string contents) {
	ofs << contents;
	ofs.close();
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
