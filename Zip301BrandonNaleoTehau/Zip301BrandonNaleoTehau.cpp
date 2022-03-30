// Zip301BrandonNaleoTehau.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstddef>

#include "Node.h"
#include <bitset>

using namespace std;

map<char, int> charmap;
vector<Node*> nodes;
map<char, string> encodings;
string originalContents;

string readFile(ifstream&);
void calculateFrequencies();
void generateTree();
void extractEncodings(Node*, string);
string encodeFile();
void outputHeader(ofstream&);
void outputContent(ofstream&, string);

int main(int argc, char** argv)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto step = std::chrono::high_resolution_clock::now();
	//no command line argument
	if (argc < 2) {
		cout << "Please include the name of the file you wish to compress." << endl;
		return -1;
	}

	string filename = argv[1];
	ifstream ifs;
	ifs.open(filename, ios::in | ios::binary | ios::ate);

	//file not found
	if (!ifs) {
		cout << "Error opening file: " << filename << endl;
		return -1;
	}

	originalContents =  readFile(ifs);
	auto step1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = step1 -  step;
	cout << "Reading in File : " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	//calculate frequencies
	calculateFrequencies();
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Calculating Frequencies: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	//generate tree
	generateTree();
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Generating Tree: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	//convert tree to encodings

	extractEncodings(nodes[0], "");
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Extracting Encodings: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	//encode file
	string output = encodeFile();
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Encoding File: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	ifs.close();

	//output header information
	// Get filename and open file
	string outputfilename;
	stringstream t(filename);
	getline(t, outputfilename, '.');
	outputfilename += ".zip301";
	ofstream ofs;
	ofs.open(outputfilename, ios::out | ios::binary);

	outputHeader(ofs);
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Outputting Header: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	//output encoded file
	outputContent(ofs, output);
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 -  step;
	cout << "Outputting Content: " << elapsed.count() << "s" << endl;
	step = std::chrono::high_resolution_clock::now();

	ofs.close();
	step1 = std::chrono::high_resolution_clock::now();
	elapsed = step1 - start;
	cout << "Total Runtime: " << elapsed.count() << "s" << endl;
}

//read the entire contents of the file at once
string readFile(ifstream& ifs) {
	ifstream::pos_type fileSize = ifs.tellg();
	ifs.seekg(0, ios::beg);

	vector<char> bytes(fileSize);
	ifs.read(bytes.data(), fileSize);

	return string(bytes.data(), fileSize);
}

void calculateFrequencies()
{
	for (int i = 0; i < originalContents.length(); i++) {
		char c = originalContents[i];
		charmap[c] += 1;
	}
}

// used to sort
bool compareNode(Node* n, Node* n2) {
	return (n->getId() < n2->getId());
}

void generateTree() {
	// create nodes and insert into tree.
	for (auto const& val : charmap) {
		char c = val.first;
		int count = val.second;
		Node* n = new Node(count, c);
		nodes.push_back(n);
	}
	// loop through and structure tree:
	while (nodes.size() > 1) {
		sort(nodes.begin(), nodes.end(), compareNode); //sort arraylist (should add O(n log n)
		Node* n1 = nodes[0];
		Node* n2 = nodes[1];
		//create new parent
		Node* n3 = new Node(n1->getId() + n2->getId());
		n3->setLeftChild(n1);
		n3->setRightChild(n2);
		//remove old nodes
		nodes.erase(remove(nodes.begin(), nodes.end(), n1), nodes.end());
		nodes.erase(remove(nodes.begin(), nodes.end(), n2), nodes.end());
		//insert new node with children.
		nodes.push_back(n3);
	}
}

void extractEncodings(Node* n, string cur_encoding) { //traverse the tree.  Should be O(log n) where n is number of nodes in the tree.
	if (n->getLeftChild() == nullptr && n->getRightChild() == nullptr) {
		encodings[n->getLetter()] = cur_encoding;
	}
	else {
		extractEncodings(n->getLeftChild(), cur_encoding + "0");
		extractEncodings(n->getRightChild(), cur_encoding + "1");
	}
}

string encodeFile() {
	string outputstring = "";
	for (int i = 0; i < originalContents.length(); i++) {
		char c = originalContents[i];
		outputstring += encodings[c];
	}
	return outputstring;
}

void outputHeader(ofstream& ofs) {
	for (auto const& val : encodings) {
		char c = val.first;
		string encoding = val.second;
		string cOut;
		if (c == '\n') {
			cOut = "newline";
		}
		else if (c == ' ') {
			cOut = "space";
		}
		else {
			cOut = c;
		}
		ofs << encoding << " " << cOut << endl;
	}
	ofs << "*****" << endl;
}

void outputContent(ofstream& ofs, string output) {
	int size = output.length();
	ofs << size << endl;
	int countOffset = (size % 8 == 0) ? 0 : 8 - (size % 8); //check number of bits we need to add.
	for (int i = 0; i < countOffset; i++) {
		output += "0";
	}
	for (int i = 0; i < output.length() / 8; i++) {
		string singleByte = output.substr(i * 8, 8);
		bitset<8> bits(singleByte);
		char out = (char)bits.to_ulong();
	ofs.write(&out, sizeof(char));
	}

}
