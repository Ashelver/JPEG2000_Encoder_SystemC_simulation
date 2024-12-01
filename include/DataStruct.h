#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <map>
#include <string>
#include <iostream>
#include <vector>

// Encapsulate information and data for each block
struct BlockData {
    int option;
    std::vector<std::vector<std::vector<std::vector<int>>>> mask;  // 4D vector for mask data
    std::vector<std::vector<std::vector<std::vector<float>>>> blocks; // 4D vector for image blocks
    int block_width;         // Width of each block
    int block_height;        // Height of each block
    int block_rows;          // Number of rows of image blocks
    int block_cols;          // Number of columns of image blocks
    int real_image_width;    // Actual image width
    int real_image_height;   // Actual image height
};


struct HuffmanResult {
    int option;
    int real_image_width;
    int real_image_height;
    std::string bitstream;  // Encoded bit stream
    std::map<int, std::string> huffmanCodes;  // Huffman coding table
};

// Declare operator<<
std::ostream& operator<<(std::ostream& os, const BlockData& blockData);
std::ostream& operator<<(std::ostream& os, const HuffmanResult& huffmanResult);


#endif // DATASTRUCT_H
