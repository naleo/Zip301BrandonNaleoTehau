#pragma once
class Node
{
private:
	int id;
	char letter;
	Node* leftChild;
	Node* rightChild;
public:
	int getId();
	Node* getLeftChild();
	Node* getRightChild();
	void setLeftChild(Node*);
	void setRightChild(Node*);
	void setLetter(char);
	char getLetter();
	Node(int, char);
	Node(int);
};

