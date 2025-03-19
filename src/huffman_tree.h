#pragma once

// Huffman node.
struct Node
{
	Node* left;
	Node* right;
	char symbol;
	size_t frequency;

	Node(char symbol, size_t frequency)
		: symbol(symbol), frequency(frequency), left(nullptr), right(nullptr) {};
};
