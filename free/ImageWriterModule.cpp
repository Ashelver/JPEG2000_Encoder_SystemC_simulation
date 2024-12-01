#include "ImageWriterModule.h"
#include <iostream>
#include <fstream>

// Constructor, initialize the module and define the processing thread
ImageWriterModule::ImageWriterModule(sc_module_name name) : sc_module(name) {
    SC_THREAD(process_write);  // Create a thread for image writing process using SC_THREAD
    sensitive << start_write;  // Listen for the start_write signal
}

// Write image data to a PNG file
void ImageWriterModule::write_png(const char* filename, BlockData image_data) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "Error: could not open file for writing!" << std::endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        std::cerr << "Error: png_create_write_struct failed!" << std::endl;
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Error: png_create_info_struct failed!" << std::endl;
        png_destroy_write_struct(&png, (png_infopp)NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error: libpng setjmp failed!" << std::endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(png, info, image_data.real_image_width, image_data.real_image_height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    for (int i = 0; i < image_data.real_image_height; ++i) {
        png_byte row[image_data.real_image_width * 3];
        for (int j = 0; j < image_data.real_image_width; ++j) {
            // Access the data for each block (blocks is a 4D array)
            int block_id = (i / image_data.block_height) * image_data.block_cols + (j / image_data.block_width);
            int y = i % image_data.block_height;
            int x = j % image_data.block_width;

            // Get the RGB channel values
            row[j * 3] = static_cast<unsigned char>(image_data.blocks[block_id][y][x][0]);     // Y channel
            row[j * 3 + 1] = static_cast<unsigned char>(image_data.blocks[block_id][y][x][1]); // Cb channel
            row[j * 3 + 2] = static_cast<unsigned char>(image_data.blocks[block_id][y][x][2]); // Cr channel
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

// Logic to handle image writing
void ImageWriterModule::process_write() {
    while (true) {
        wait();  // Wait for the start_write signal to trigger

        // Read image blocks from FIFO
        BlockData image_data = input_blocks.read();

        // Create a filename and call the write function
        const char* filename = "output_image.png";  // The filename can be dynamically generated if needed
        // write_png(filename, image_data);

        std::cout << "[IMAGE WRITER MODULE] Image written to " << filename << std::endl;
    }
}
