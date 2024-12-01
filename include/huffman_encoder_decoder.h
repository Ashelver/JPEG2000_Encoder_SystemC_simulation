#ifndef HUFFMAN_ENCODER_DECODER_H
#define HUFFMAN_ENCODER_DECODER_H

#include <vector>
#include <map>
#include <queue>
#include <string>
#include <bitset>
#include <unordered_map>

const int Placeholder = 512;

// Huffman Tree Node
struct Node {
    int value;  // Node value
    int frequency;  // Node frequency
    Node* left;  // Left child node
    Node* right;  // Right child node

    Node(int v, int f);
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency; // Min heap: prioritize nodes with smaller frequency
    }
};

// Function declarations
void printMinHeap(std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap);
std::string stringToBitstream(const std::string& bitString);
std::string bitstreamToString(const std::string& bitstream);
Node* buildHuffmanTree(const std::map<int, int>& frequency);
void generateHuffmanCodes(Node* root, std::string code, std::map<int, std::string>& huffmanCodes);
std::string encodeWithHuffman(const std::vector<std::vector<int>>& image, const std::map<int, std::string>& huffmanCodes);
std::vector<int> huffmanDecodeImage(const std::string& encodedStream, const std::map<int, std::string>& huffmanCodes);

#endif // HUFFMAN_ENCODER_DECODER_H
