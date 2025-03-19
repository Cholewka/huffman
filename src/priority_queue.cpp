#include "priority_queue.h"

void PriorityQueue::enqueue(Node* node)
{
	this->heap.push_back(node);

	// If there exists a node, that has lower priority than passed, then
	// to remain heap properties it need to be moved to the top.
	this->move_up(this->heap.size() - 1);
}

Node* PriorityQueue::dequeue()
{
	size_t heapSize = this->heap.size();

	if (heapSize == 0)
		return nullptr;
	if (heapSize == 1)
	{
		Node* node = this->heap[0];
		this->heap.clear();
		return node;
	}

	// Pop a node from the top and replace it with the last element of the heap.
	Node* returnedNode = this->heap.front();
	Node* lastNode = this->heap.back();
	this->heap[0] = lastNode;
	this->heap.pop_back();

	// If child nodes have lower frequencies than the new root, than to remain
	// heap properties they need to be moved to the top.
	this->move_down(0);
	return returnedNode;
}

size_t PriorityQueue::get_size()
{
	return this->heap.size();
}

void PriorityQueue::move_up(size_t index)
{
	// If there is no parent, stop the recursion.
	if (index == 0)
		return;

	size_t elementFrequency = this->heap[index]->frequency;
	size_t parentIndex = static_cast<size_t>(std::ceil(static_cast<double>(index - 1) / 2.0));
	size_t parentFrequency = this->heap[parentIndex]->frequency;

	if (elementFrequency < parentFrequency)
	{
		std::swap(this->heap[index], this->heap[parentIndex]);

		// Check recursively if there are more parents that need to be switched.
		this->move_up(parentIndex);
	}
}

void PriorityQueue::move_down(size_t index)
{
	size_t leftChildIndex = 2 * index + 1;
	size_t rightChildIndex = 2 * index + 2;
	size_t lowestIndex = leftChildIndex;

	// Is there not a left child?
	if (leftChildIndex >= this->heap.size())
		return;

	// If the right child exists, then doesn't it have lower frequency than the left?
	if (rightChildIndex < this->heap.size())
	{
		size_t leftChildFrequency = this->heap[leftChildIndex]->frequency;
		size_t rightChildFrequency = this->heap[rightChildIndex]->frequency;

		if (rightChildFrequency < leftChildFrequency)
			lowestIndex = rightChildIndex;
	}

	// Swap the parent with the lowest child, if necessary.
	size_t indexFrequency = this->heap[index]->frequency;
	size_t lowestFrequency = this->heap[lowestIndex]->frequency;

	if (lowestFrequency < indexFrequency)
	{
		std::swap(this->heap[index], this->heap[lowestIndex]);

		// Check recursively, if there are more children, that need to be swapped.
		this->move_down(lowestIndex);
	}
}
