// The file contains an implementation of a priority queue, that is base on
// a minheap. It's used in the process of creating a Huffman tree.

#pragma once

#include <vector>
#include <cmath>

#include "huffman_tree.h"

struct PriorityQueue
{
	void enqueue(Node* node);
	Node* dequeue();
	size_t get_size();

private:
	std::vector<Node*> heap;
	void move_up(size_t index);
	void move_down(size_t index);
};