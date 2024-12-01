#ifndef HUFFMAN_RESULT_IO_H
#define HUFFMAN_RESULT_IO_H

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "DataStruct.h" // Assuming HuffmanResult and BlockData are defined in this header file

class HuffmanResultIO {
public:
    // Serialize HuffmanResult to a file
    static bool writeHuffmanResultToFile(const std::string& filename, const HuffmanResult& result);

    // Read HuffmanResult from a file
    static bool readHuffmanResultFromFile(const std::string& filename, HuffmanResult& result);
};

#endif // HUFFMAN_RESULT_IO_H
