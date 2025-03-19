// Some helper functions, that can generate prefix codes and compress
// and decompress files.

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>

#include "huffman_tree.h"
#include "priority_queue.h"

// Create a Huffman tree and return its root.
Node* create_huffman_tree(std::string string);

// Create prefix codes.
void generate_huffman_codes(Node* node, std::string code, std::unordered_map<char, std::string>& codes);

// Save Huffman tree into a file.
void save_huffman_tree(Node* node, std::ofstream& outFile);

// Save compressed text into a file. Returns -1 in case of an error.
int compress_to_file(Node* root, const std::string& text, const std::string& filepath);

// Recursively deallocate memory from the nodes.
void destroy_tree(Node* node);

// Load Huffman tree from a file.
Node* load_huffman_tree(std::ifstream& inFile);

// Read compressed text from a file.
std::string decompress_file(const std::string& filepath);