#include "quan_module.h"

// Constructor
QuanModule::QuanModule(sc_module_name name) : sc_module(name) {
    SC_THREAD(performQuan);
    sensitive << start_quan;
}

// Main logic for quantization
void QuanModule::performQuan() {
    wait();
    BlockData blocks;
    BlockData output_data;

    // Read block data from input channel
    input_blocks.read(blocks);

    // Get quantization option
    int option_ = option.read();

    // Set quantization step sizes
    float qf_ll, qf_hl, qf_lh, qf_hh;
    if (option == 1) {
        qf_ll = 20; qf_hl = 40; qf_lh = 40; qf_hh = 80;
    } else if (option == 2) {
        qf_ll = 10; qf_hl = 20; qf_lh = 20; qf_hh = 40;
    } else if (option == 3) {
        qf_ll = 5; qf_hl = 10; qf_lh = 10; qf_hh = 20;
    } else {
        std::cerr << "Unsupported option!" << std::endl;
    }

    // Process block data
    for (size_t i = 0; i < blocks.blocks.size(); ++i) {
        process_block(blocks.blocks[i], blocks.mask[i], qf_ll, qf_hl, qf_lh, qf_hh);
    }

    blocks.option = option_;
    // Write quantized blocks to output channel
    output_blocks.write(blocks);

    std::cout << "[QUANTIZATION MODULE] Finished" << std::endl;

    // Send completion signal
    start_entro.write(true);
}

// Helper function: Quantize a single image block
void QuanModule::process_block(std::vector<std::vector<std::vector<float>>>& block, 
                               std::vector<std::vector<std::vector<int>>>& mask, 
                               float qf_ll, float qf_hl, float qf_lh, float qf_hh) {
    int height = block.size();
    int width = block[0].size();
    int channels = block[0][0].size();

    // Iterate over each channel
    for (int c = 0; c < channels; ++c) {
        float coe = 1;
        if (c != 0) {
            coe = 0.5;
        }
        // Quantize LL region (top-left)
        for (int i = 0; i < height / 2; ++i) {
            for (int j = 0; j < width / 2; ++j) {
                block[i][j][c] = round(block[i][j][c] * coe / qf_ll);
            }
        }

        // Quantize HL region (top-right)
        for (int i = 0; i < height / 2; ++i) {
            for (int j = width / 2; j < width; ++j) {
                block[i][j][c] = round(block[i][j][c] * coe / qf_hl);
            }
        }

        // Quantize LH region (bottom-left)
        for (int i = height / 2; i < height; ++i) {
            for (int j = 0; j < width / 2; ++j) {
                block[i][j][c] = round(block[i][j][c] * coe / qf_lh);
            }
        }

        // Quantize HH region (bottom-right)
        for (int i = height / 2; i < height; ++i) {
            for (int j = width / 2; j < width; ++j) {
                block[i][j][c] = round(block[i][j][c] * coe / qf_hh);
            }
        }
    }
}
