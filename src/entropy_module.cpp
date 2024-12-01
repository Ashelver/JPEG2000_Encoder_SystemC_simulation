#include "entropy_module.h"
#include "huffman_encoder_decoder.h"

// Huffman Encoder

// Constructor
EntropyEncoderModule::EntropyEncoderModule(sc_module_name name): sc_module(name){
    SC_THREAD(encode);
    sensitive << start_entropy;
}

// Entropy encoding main thread
void EntropyEncoderModule::encode() {
    wait();
    // Read a data block from the FIFO
    BlockData blocks;
    input_blocks.read(blocks);

    // Process the data block and return the bitstream
    HuffmanResult HResult = process_blocks(blocks);

    // Write the bitstream to the output FIFO
    hResult.write(HResult);

    std::cout << "[ENTROPY MODULE] Finished" << std::endl;

    // Notify the write module to start
    start_write.write(true);
}


HuffmanResult EntropyEncoderModule::process_blocks(const BlockData& blocks) {
    HuffmanResult result;
    int real_image_height = blocks.real_image_height;
    int real_image_width = blocks.real_image_width;

    // Create a flattened image data (each pixel has 3 channels)
    std::vector<std::vector<int>> flatten_image(blocks.block_height*blocks.block_rows, std::vector<int>(blocks.block_width*blocks.block_cols * 3));

    // Fill the flatten_image array
    for (int i = 0; i < blocks.block_height*blocks.block_rows; ++i) {
        for (int j = 0; j < blocks.block_width*blocks.block_cols; ++j) {
            // Access the data of each block
            int block_id = (i / blocks.block_height) * blocks.block_cols + (j / blocks.block_width);
            int y = i % blocks.block_height;
            int x = j % blocks.block_width;

            // Get the Y, Cb, Cr channel pixel values
            flatten_image[i][j*3] = static_cast<int>(blocks.blocks[block_id][y][x][0]);  // Y
            flatten_image[i][j*3 + 1] = static_cast<int>(blocks.blocks[block_id][y][x][1]);  // Cb
            flatten_image[i][j*3 + 2] = static_cast<int>(blocks.blocks[block_id][y][x][2]);  // Cr
        }
    }

    // Calculate the frequencies and build the Huffman tree
    std::map<int, int> frequency;
    for (const auto& row : flatten_image) {
        for (int pixelValue : row) {
            frequency[pixelValue]++;
        }
    }

    // Build the Huffman tree
    Node* root = buildHuffmanTree(frequency);

    // Generate Huffman codes
    std::map<int, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Encode the flatten_image using the Huffman codes
    std::string encodedStream = encodeWithHuffman(flatten_image, huffmanCodes);

    // Store the encoded stream and Huffman codes table in the result
    result.bitstream = encodedStream;
    result.huffmanCodes = huffmanCodes;
    result.option = blocks.option;
    result.real_image_width = real_image_width;
    result.real_image_height = real_image_height;
    return result;
}
