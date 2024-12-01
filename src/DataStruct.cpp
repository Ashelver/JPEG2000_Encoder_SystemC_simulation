#include "DataStruct.h"

std::ostream& operator<<(std::ostream& os, const BlockData& blockData) {
    os << "BlockData Information:" << std::endl;
    os << "  Block Width: " << blockData.block_width << std::endl;
    os << "  Block Height: " << blockData.block_height << std::endl;
    os << "  Block Rows: " << blockData.block_rows << std::endl;
    os << "  Block Cols: " << blockData.block_cols << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const HuffmanResult& huffmanResult) {
    os << "Huffman Result" << std::endl;
    return os;
}


