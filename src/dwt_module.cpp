#include "dwt_module.h"
#include <iostream>


// DWTModule constructor
DWTModule::DWTModule(sc_module_name name) : sc_module(name) {
    SC_THREAD(performDWT);  // Define thread execution performDWT
    sensitive << start_dwt;  // When the start signal is activated, perform the DWT calculation
}


// Haar DWT
void DWTModule::process_block(std::vector<std::vector<std::vector<float>>>& block, 
                              std::vector<std::vector<std::vector<int>>>& mask, 
                              int width, int height) {
    // Create a two-dimensional vector to temporarily store the DWT results
    std::vector<std::vector<float>> temp(height, std::vector<float>(width));

    // Process each channel
    for (int c = 0; c < block[0][0].size(); c++) { 
        // Horizontal DWT calculation (low and high frequencies)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < (width+1) / 2; j++) {
                // Calculate the low and high frequency parts
                temp[i][j] = (block[i][2 * j][c] + block[i][2 * j + 1][c]) / sqrt(2);  // L
                temp[i][j + width / 2] = (block[i][2 * j][c] - block[i][2 * j + 1][c]) / sqrt(2);  // H
            }
        }


        // Vertical DWT calculation (low and high frequencies)
        for (int j = 0; j < width; j++) {
            for (int i = 0; i < (height+1) / 2; i++) {
                // Calculate the low and high frequency parts
                block[i][j][c] = (temp[2 * i][j] + temp[2 * i + 1][j]) / sqrt(2);  // HL
                block[i + height / 2][j][c] = (temp[2 * i][j] - temp[2 * i + 1][j]) / sqrt(2);  // HH
            }
        }
    }
}




void DWTModule::performDWT() {
    wait();
    BlockData block_data;
    input_blocks.read(block_data); 

    for (size_t i = 0; i < block_data.blocks.size(); ++i) {
        process_block(block_data.blocks[i], block_data.mask[i], block_data.block_width, block_data.block_height);  // 处理当前块
    }

    output_blocks.write(block_data);
    
    std::cout << "[DWT MODULE] Finshed" << std::endl;

    start_quan.write(true);
}
