#include "preprocess_module.h"
#include <png.h>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <cstdio>

ImagePreprocessModule::ImagePreprocessModule(sc_module_name name) : sc_module(name) {
    SC_THREAD(process_image);
    sensitive << start;  // Trigger the thread when the start signal changes
}

void ImagePreprocessModule::RGBtoYCbCr(float r, float g, float b,
                             float &y, float &cb, float &cr) {
    y = static_cast<float>(0.299 * r + 0.587 * g + 0.114 * b);
    cb = static_cast<float>(-0.168736 * r - 0.331264 * g + 0.5 * b + 128);
    cr = static_cast<float>(0.5 * r - 0.418688 * g - 0.081312 * b + 128);
}

BlockData ImagePreprocessModule::readPngAndsplitIntoBlocks(
    const char *filename, int &width, int &height, int block_width, int block_height) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        throw std::runtime_error("Failed to open file.");
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        throw std::runtime_error("Failed to create png_struct.");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Failed to create png_info.");
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        throw std::runtime_error("Error during PNG read.");
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    png_read_update_info(png, info);

    std::vector<png_bytep> row_pointers(height);
    std::vector<unsigned char> buffer(width * height * 4);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = buffer.data() + y * width * 4;
    }

    png_read_image(png, row_pointers.data());
    fclose(fp);


    // Split into chunks
    int block_rows = (height + block_height - 1) / block_height; // Round up
    int block_cols = (width + block_width - 1) / block_width;   // Round up

    BlockData block_data;
    block_data.block_width = block_width;
    block_data.block_height = block_height;
    block_data.block_rows = block_rows;
    block_data.block_cols = block_cols;
    block_data.real_image_width = width;
    block_data.real_image_height = height;

    // Initialize blocks and mask
    block_data.blocks.resize(block_rows * block_cols);
    block_data.mask.resize(block_rows * block_cols);
    for (int i = 0; i < block_rows * block_cols; ++i) {
        block_data.blocks[i].resize(block_height, std::vector<std::vector<float>>(block_width, std::vector<float>(3, 0.0f)));
        block_data.mask[i].resize(block_height, std::vector<std::vector<int>>(block_width, std::vector<int>(3, 0)));
    }


    // Fill block data and mask
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int block_row = y / block_height;
            int block_col = x / block_width;
            int block_id = block_row * block_cols + block_col;

            int local_y = y % block_height;
            int local_x = x % block_width;

            block_data.blocks[block_id][local_y][local_x][0] = static_cast<float>(row_pointers[y][4 * x + 0]); // R
            block_data.blocks[block_id][local_y][local_x][1] = static_cast<float>(row_pointers[y][4 * x + 1]); // G
            block_data.blocks[block_id][local_y][local_x][2] = static_cast<float>(row_pointers[y][4 * x + 2]); // B

            block_data.mask[block_id][local_y][local_x][0] = 1;
            block_data.mask[block_id][local_y][local_x][1] = 1;
            block_data.mask[block_id][local_y][local_x][2] = 1;
        }
    }

    // Process the fill area
    for (int block_id = 0; block_id < block_rows * block_cols; ++block_id) {
        for (int y = 0; y < block_height; ++y) {
            for (int x = 0; x < block_width; ++x) {
                if ((block_id / block_cols == block_rows - 1 && y + (block_id / block_cols) * block_height >= height) ||
                    (block_id % block_cols == block_cols - 1 && x + (block_id % block_cols) * block_width >= width)) {
                    block_data.blocks[block_id][y][x][0] = 0.0f;
                    block_data.blocks[block_id][y][x][1] = 0.0f;
                    block_data.blocks[block_id][y][x][2] = 0.0f;

                    block_data.mask[block_id][y][x][0] = 0;
                    block_data.mask[block_id][y][x][1] = 0;
                    block_data.mask[block_id][y][x][2] = 0;
                }
            }
        }
    }

    png_destroy_read_struct(&png, &info, nullptr);
    return block_data;
}


void ImagePreprocessModule::performDCShiftAndRGBtoYCbCr(BlockData &block_data) {
    for (int block_id = 0; block_id < block_data.blocks.size(); ++block_id) {
        auto &block = block_data.blocks[block_id];
        int block_height = block.size();
        int block_width = block[0].size();

        // Initialize the mean of the three channels
        float mean_r = 0.0f, mean_g = 0.0f, mean_b = 0.0f;
        int valid_pixel_count = 0;

        //Perform zero-frequency shift operation
        for (int y = 0; y < block_height; ++y) {
            for (int x = 0; x < block_width; ++x) {
                block[y][x][0] -= 128; // R channel
                block[y][x][1] -= 128; // G channel
                block[y][x][2] -= 128; // B channel
                RGBtoYCbCr(block[y][x][0], block[y][x][1], block[y][x][2] ,block[y][x][0], block[y][x][1], block[y][x][2]);
            }
        }

    }
}



void ImagePreprocessModule::process_image() {
    // Determine whether the start signal is valid, and there must be a signal change to trigger this method
    if (start.read()) {
        try {
            int width, height;
            const char *filename = input_file.read();  // Read the input file path

            int block_width = 128, block_height = 128;
            BlockData blocks = readPngAndsplitIntoBlocks(filename, width, height, block_width, block_height);  // 转换图像到 YCbCr
            performDCShiftAndRGBtoYCbCr(blocks);

            // Write the generated image blocks to the output_blocks FIFO signal
            output_blocks.write(blocks);  

            std::cout << "[PREPROCCESS MODULE] Finshed" << std::endl;
            
            start_dwt.write(true);

        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
}

