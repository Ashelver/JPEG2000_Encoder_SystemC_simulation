#include "huffmanIO.h"

bool HuffmanResultIO::writeHuffmanResultToFile(const std::string& filename, const HuffmanResult& result) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(&result.option), sizeof(result.option));

    ofs.write(reinterpret_cast<const char*>(&result.real_image_width), sizeof(result.real_image_width));
    ofs.write(reinterpret_cast<const char*>(&result.real_image_height), sizeof(result.real_image_height));

    size_t bitstream_length = result.bitstream.size();
    ofs.write(reinterpret_cast<const char*>(&bitstream_length), sizeof(bitstream_length));
    ofs.write(result.bitstream.c_str(), bitstream_length);

    size_t map_size = result.huffmanCodes.size();
    ofs.write(reinterpret_cast<const char*>(&map_size), sizeof(map_size));
    for (const auto& entry : result.huffmanCodes) {
        ofs.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
        size_t code_size = entry.second.size();
        ofs.write(reinterpret_cast<const char*>(&code_size), sizeof(code_size));
        ofs.write(entry.second.c_str(), code_size); 
    }

    std::cout << "[WRITE MODULE] The number of key-value pairs in the map is: " << map_size << std::endl;
    std::cout << "[WRITE MODULE] The length of the bitstream is: " << bitstream_length << std::endl;
    
    ofs.close();
    return true;
}

bool HuffmanResultIO::readHuffmanResultFromFile(const std::string& filename, HuffmanResult& result) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "[WRITE MODULE] Error opening file for reading: " << filename << std::endl;
        return false;
    }

    ifs.read(reinterpret_cast<char*>(&result.option), sizeof(result.option));

    ifs.read(reinterpret_cast<char*>(&result.real_image_width), sizeof(result.real_image_width));
    ifs.read(reinterpret_cast<char*>(&result.real_image_height), sizeof(result.real_image_height));

    size_t bitstream_length;
    ifs.read(reinterpret_cast<char*>(&bitstream_length), sizeof(bitstream_length));
    result.bitstream.resize(bitstream_length);
    ifs.read(&result.bitstream[0], bitstream_length);

    size_t map_size;
    ifs.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));
    result.huffmanCodes.clear();
    for (size_t i = 0; i < map_size; ++i) {
        int key;
        ifs.read(reinterpret_cast<char*>(&key), sizeof(key));

        size_t code_size;
        ifs.read(reinterpret_cast<char*>(&code_size), sizeof(code_size));

        std::string code;
        code.resize(code_size);
        ifs.read(&code[0], code_size);

        result.huffmanCodes[key] = code;
    }

    ifs.close();
    return true;
}
