#include "tree_methods.h"

Node* create_huffman_tree(std::string string)
{
    if (string.empty())
    {
        std::cerr << "An empty string was entered.\n";
        return nullptr;
    }

    // Find character frequencies.
    std::unordered_map<char, size_t> frequencies;
    for (auto symbol : string)
        frequencies[symbol]++;

    // Create tree nodes and enter them to the priority queue.
    PriorityQueue pq;

    for (auto mapElement : frequencies)
        pq.enqueue(new Node(mapElement.first, mapElement.second));

    // Create a tree from queue elements.
    while (pq.get_size() > 1)
    {
        Node* firstNode = pq.dequeue();
        Node* secondNode = pq.dequeue();
        Node* newNode = new Node(NULL, firstNode->frequency + secondNode->frequency);
        newNode->left = firstNode;
        newNode->right = secondNode;
        pq.enqueue(newNode);
    }

    // Last queue element is the root of Huffman tree.
    return pq.dequeue();
}

void generate_huffman_codes(Node* node, std::string code, std::unordered_map<char, std::string>& codes)
{
    if (node->left == nullptr && node->right == nullptr)
    {
        // Encountered a node, that contains a symbol. Save in a dictionary and stop the recursion.
        codes[node->symbol] = code;
        return;
    }

    if (node->left != nullptr)
        // If there is a left node, add zero to the code and enter it.
        generate_huffman_codes(node->left, code + '0', codes);
    if (node->right != nullptr)
        // If there is a right node, add one to the code and enter it.
        generate_huffman_codes(node->right, code + '1', codes);
}

void save_huffman_tree(Node* node, std::ofstream& outFile)
{
    if (node == nullptr)
    {
        // The node is empty - enter '0'.
        outFile.put(0);
        return;
    }

    // The node is not empty, enter '1' and symbol character.
    outFile.put(1);
    outFile.put(node->symbol);

    // Recursively do the same to the left and right node.
    save_huffman_tree(node->left, outFile);
    save_huffman_tree(node->right, outFile);
}

int compress_to_file(Node* root, const std::string& text, const std::string& filepath)
{
    // Enter the file.
    std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
    if (!outFile.good())
    {
        std::cerr << "Unable to read \"" << filepath << "\" for compression.\n";
        return -1;
    }

    save_huffman_tree(root, outFile);

    // Generate prefix codes.
    std::unordered_map<char, std::string> codes;
    generate_huffman_codes(root, "", codes);

    // Compress text.
    std::string compressedText;
    for (auto symbol : text)
        compressedText.append(codes[symbol]);

    // Length of the string is also the number of bits that need to be taken care of
    // during decompression, so we also save them.
    size_t compressedTextLength = compressedText.length();
    outFile.write(reinterpret_cast<const char*>(&compressedTextLength), sizeof(size_t));

    // If the length of the compressed text is not a multiple of 8, than I add padding
    // using zeroes, so we can later treat substrings of 8 characters as bytes.
    if (compressedTextLength % 8 != 0)
        compressedText.append(8 - (compressedTextLength % 8), '0');

    for (int i = 0; i < compressedText.length(); i += 8)
    {
        // Slice the string to substrings of bytes, and then save them into a file.
        std::string byte = compressedText.substr(i, 8);
        unsigned char byteAscii = static_cast<unsigned char>(std::stoi(byte, nullptr, 2));
        outFile.write(reinterpret_cast<const char*>(&byteAscii), sizeof(unsigned char));
    }

    outFile.close();
    return 0;
}

void destroy_tree(Node* node)
{
    if (!node)
        return;

    destroy_tree(node->left);
    destroy_tree(node->right);

    delete node;
    node = nullptr;
}

Node* load_huffman_tree(std::ifstream& inFile)
{
    // Make sure there is something to read.
    if (inFile.eof())
    {
        std::cerr << "Unexpected end of a file.\n";
        return nullptr;
    }

    // Check if the node contains a symbol.
    bool containsSymbol = false;
    inFile.read(reinterpret_cast<char*>(&containsSymbol), sizeof(bool));

    // If the symbol is empty, do not continue recursion in this node.
    if (!containsSymbol)
        return nullptr;

    char symbol;
    inFile.get(symbol);

    // Create new node. The information of frequency is no longer relevant. Next,
    // recursively read its child nodes. First will be the instruction of reading
    // left child nodes, than right ones, that's why the file will be read correctly.
    Node* node = new Node(symbol, NULL);
    node->left = load_huffman_tree(inFile);
    node->right = load_huffman_tree(inFile);

    return node;
}

std::string decompress_file(const std::string& filepath)
{
    // Enter a file.
    std::ifstream inFile(filepath, std::ios::in | std::ios::binary);
    if (!inFile.good())
    {
        std::cerr << "Unable to read file \"" << filepath << "\" for decompression.\n";
        return std::string();
    }

    Node* root = load_huffman_tree(inFile);
    if (root == nullptr)
    {
        std::cerr << "Cannot read Huffman tree.\n";
        return std::string();
    }

    std::unordered_map<char, std::string> codes;
    generate_huffman_codes(root, "", codes);

    // Make sure there is something to read.
    if (inFile.eof())
    {
        std::cerr << "Unexpected end of a file.\n";
        return std::string();
    }

    // Read the number of bytes of compressed text and count how many bytes need to be read.
    size_t compressedTextBits;
    inFile.read(reinterpret_cast<char*>(&compressedTextBits), sizeof(size_t));
    size_t bytesToRead = static_cast<size_t>(std::ceil(static_cast<double>(compressedTextBits) / 8.0));
    
    std::string decompressedText;

    // Convert each byte into a symbol.
    for (int i = 0; i < bytesToRead; i++)
    {
        // Make sure there is something to read.
        if (inFile.eof())
        {
            std::cerr << "Unexpected end of a file.\n";
            return std::string();
        }

        char ascii;
        inFile.get(ascii);

        // Convert ASCII char into a string of bytes using bitset library.
        decompressedText.append(std::bitset<8>(ascii).to_string());
    }

    inFile.close();

    // Get rid of the zeroes-padding.
    decompressedText = decompressedText.substr(0, compressedTextBits);

    // Iterate through every bit and, if encounter 0, go left in loaded Huffman
    // tree, and if 1 - go right. If you encounter a symbol, append it to the
    // decoded string.
    std::string decodedText;
    Node* currentNode = root;

    for (char bit : decompressedText)
    {
        if (currentNode == nullptr)
        {
            std::cerr << "Encountered invalid pointer.\n";
            return std::string();
        }

        if (bit == '0')
            currentNode = currentNode->left;
        else if (bit == '1')
            currentNode = currentNode->right;

        if (currentNode && currentNode->left == nullptr && currentNode->right == nullptr)
        {
            decodedText += currentNode->symbol;
            currentNode = root;
        }
    }

    // Text should be decompressed.
    destroy_tree(root);
    return decodedText;
}
