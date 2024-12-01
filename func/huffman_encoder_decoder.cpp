#include "huffman_encoder_decoder.h"
#include <iostream>

void printMinHeap(std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap) {
    while (!minHeap.empty()) {
        Node* node = minHeap.top();
        minHeap.pop();
        std::cout << "Value: " << static_cast<int>(node->value) 
                  << ", Frequency: " << node->frequency << std::endl;
    }
}

// Convert a string like "01010101" into an actual bitstream
std::string stringToBitstream(const std::string& bitString) {
    std::string bitstream;
    size_t length = bitString.length();
    for (size_t i = 0; i < length; i += 8) {
        std::string byteString = bitString.substr(i, 8);
        while (byteString.length() < 8) {
            byteString += '0';  // Pad with '0' to make it 8 bits
        }
        char byte = static_cast<char>(std::bitset<8>(byteString).to_ulong());
        bitstream.push_back(byte);
    }
    return bitstream;
}

// Convert an actual bitstream back to a string like "01010101"
std::string bitstreamToString(const std::string& bitstream) {
    std::string bitString;
    for (unsigned char byte : bitstream) {
        bitString += std::bitset<8>(byte).to_string();
    }
    return bitString;
}

// Constructor for the Huffman encoding node
Node::Node(int v, int f) : value(v), frequency(f), left(nullptr), right(nullptr) {}

// Build the Huffman tree
Node* buildHuffmanTree(const std::map<int, int>& frequency) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    
    // Insert each color value and frequency into the priority queue
    for (const auto& entry : frequency) {
        minHeap.push(new Node(entry.first, entry.second));
    }

    // printMinHeap(minHeap);   
     
    // Build the Huffman tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();
        
        Node* internalNode = new Node(Placeholder, left->frequency + right->frequency);
        internalNode->left = left;
        internalNode->right = right;
        
        minHeap.push(internalNode);
    }
    
    return minHeap.top(); // Return the root node of the Huffman tree
}

// Generate Huffman codes
void generateHuffmanCodes(Node* root, std::string code, std::map<int, std::string>& huffmanCodes) {
    if (!root) return;
    
    if (root->value != Placeholder) { // If it's a leaf node, record the code
        huffmanCodes[root->value] = code;
    }
    
    // Traverse left and right subtrees
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Perform Huffman encoding on each channel of the image
std::string encodeWithHuffman(const std::vector<std::vector<int>>& image, const std::map<int, std::string>& huffmanCodes) {
    std::string encodedStream;
    // Replace each pixel value with its corresponding Huffman code
    for (const auto& row : image) {
        for (int pixelValue : row) {
            encodedStream += huffmanCodes.at(pixelValue); // Get the code
        }
    }

    return stringToBitstream(encodedStream);
}

// Decode the encoded stream
std::vector<int> huffmanDecodeImage(const std::string& encodedStream, const std::map<int, std::string>& huffmanCodes) {
    std::vector<int> decodedImage;
    
    // Create a reverse mapping of Huffman codes to symbols
    std::unordered_map<std::string, int> reverseHuffmanCodes;
    for (const auto& entry : huffmanCodes) {
        reverseHuffmanCodes[entry.second] = entry.first;
    }

    std::string currentCode;

    for (char bit : bitstreamToString(encodedStream)) {
        currentCode += bit;
        
        // If the current path exists in the reverse map, find the corresponding symbol
        if (reverseHuffmanCodes.find(currentCode) != reverseHuffmanCodes.end()) {
            // Add the found symbol to the decoded result
            decodedImage.push_back(reverseHuffmanCodes[currentCode]);
            currentCode.clear();  // Clear the current path and continue decoding the next part
        }
    }

    return decodedImage;
}
