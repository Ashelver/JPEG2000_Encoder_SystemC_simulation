#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "huffman_encoder_decoder.h"
#include "huffmanIO.h"
#include <png.h>

template <typename T>
T clamp(const T& value, const T& min, const T& max) {
    return (value < min) ? min : (value > max) ? max : value;
}

int get_actual_size(int real_image_size, int block_size) {
    if (real_image_size % block_size == 0) {
        return real_image_size;
    } else {
        return ((real_image_size / block_size) + 1) * block_size;
    }
}


void saveImage(const std::vector<int>& decodedImage, int width, int height, const std::string& filename) {
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Error opening output file for writing!" << std::endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Error creating PNG write structure!" << std::endl;
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Error creating PNG info structure!" << std::endl;
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return;
    }

    jmp_buf png_jmpbuf;  
    if (setjmp(png_jmpbuf)) {
        std::cerr << "Error during PNG creation!" << std::endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_set_error_fn(png, nullptr, nullptr, nullptr);
    png_set_error_fn(png, nullptr, nullptr, nullptr);

    png_init_io(png, fp);

    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    std::vector<png_byte> row(3 * width);  
    for (int y = 0; y < height; ++y) {
        int startIdx = y * get_actual_size(width, 128) * 3;

        for (int x = 0; x < width; ++x) {
            row[3 * x] = static_cast<unsigned char>(decodedImage[startIdx + 3 * x]);     // R
            row[3 * x + 1] = static_cast<unsigned char>(decodedImage[startIdx + 3 * x + 1]); // G
            row[3 * x + 2] = static_cast<unsigned char>(decodedImage[startIdx + 3 * x + 2]); // B
        }

        png_write_row(png, row.data());
    }

    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(fp);

    std::cout << "Image saved as " << filename << std::endl;
}

void quan_inverse(std::vector<int>& decodedImage, int option, int image_width, int image_height, int block_size) {
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

    image_width = get_actual_size(image_width, 128);
    image_height = get_actual_size(image_height, 128);

    int block_cols = image_width / block_size;
    int block_rows = image_height / block_size;

    
    for (int block_row = 0; block_row < block_rows; ++block_row) {
        for (int block_col = 0; block_col < block_cols; ++block_col) {
            for (int row = 0; row < block_size; ++row) {
                for (int col = 0; col < block_size; ++col) {
                    int global_row = block_row * block_size + row;
                    int global_col = block_col * block_size + col;

                    int pixel_index = (global_row * image_width + global_col) * 3;
                    if (block_row * block_size >= image_height || block_col * block_size >= image_width || global_row >= image_height || global_col >= image_width) {
                        continue;
                    }

                    if (row < block_size / 2 && col < block_size / 2) {
                        decodedImage[pixel_index] *= 1 * qf_ll;
                        decodedImage[pixel_index + 1] *= 2 * qf_ll;
                        decodedImage[pixel_index + 2] *= 2 * qf_ll;
                    } else if (row < block_size / 2 && col >= block_size / 2) {
                        decodedImage[pixel_index] *= 1 * qf_hl;
                        decodedImage[pixel_index + 1] *= 2 * qf_hl;
                        decodedImage[pixel_index + 2] *= 2 * qf_hl;
                    } else if (row >= block_size / 2 && col < block_size / 2) {
                        decodedImage[pixel_index] *= 1 * qf_lh;
                        decodedImage[pixel_index + 1] *= 2 * qf_lh;
                        decodedImage[pixel_index + 2] *= 2 * qf_lh;
                    } else if (row >= block_size / 2 && col >= block_size / 2) {
                        decodedImage[pixel_index] *= 1 * qf_hh;
                        decodedImage[pixel_index + 1] *= 2 * qf_hh;
                        decodedImage[pixel_index + 2] *= 2 * qf_hh;
                    }
                }
            }
        }
    }
}

void dwt_inverse(std::vector<int>& decodedImage, int image_width, int image_height, int block_size) {
    image_width = get_actual_size(image_width, 128);
    image_height = get_actual_size(image_height, 128);

    int block_cols = image_width / block_size;
    int block_rows = image_height / block_size;

    std::vector<int> temp(decodedImage);
    for (int block_row = 0; block_row < block_rows; ++block_row) {
        for (int block_col = 0; block_col < block_cols; ++block_col) {
            for (int row = 0; row < block_size; ++row) {
                for (int col = 0; col < block_size; ++col) {
                    int half_block_height_size = block_size / 2;
                    int half_block_width_size = block_size / 2;                    
                    int global_row = block_row * block_size + row;
                    int global_col = block_col * block_size + col;
                    int pixel_index = (global_row * image_width + global_col) * 3;
                    if (pixel_index >= decodedImage.size()) {
                        continue;
                    }
                    if (row < half_block_height_size && col < half_block_width_size) {
                        for (int i = 0; i < 3; ++i){
                            int channel_index = pixel_index + i;
                            int inverse_value_lu = 0;
                            int inverse_value_ru = 0;
                            int inverse_value_ld = 0;
                            int inverse_value_rd = 0;
                            if (channel_index <= decodedImage.size()) {
                                inverse_value_lu += temp[channel_index];
                                inverse_value_ru += temp[channel_index];
                                inverse_value_ld += temp[channel_index];
                                inverse_value_rd += temp[channel_index];
                            }
                            if (channel_index + (half_block_width_size)*3 <= decodedImage.size()) {
                                inverse_value_lu += temp[channel_index + (half_block_width_size)*3];
                                inverse_value_ru -= temp[channel_index + (half_block_width_size)*3];
                                inverse_value_ld += temp[channel_index + (half_block_width_size)*3];
                                inverse_value_rd -= temp[channel_index + (half_block_width_size)*3];
                            }
                            if (channel_index + (image_width*half_block_height_size)*3 <= decodedImage.size()) {
                                inverse_value_lu += temp[channel_index + (image_width*half_block_height_size)*3];
                                inverse_value_ru += temp[channel_index + (image_width*half_block_height_size)*3];
                                inverse_value_ld -= temp[channel_index + (image_width*half_block_height_size)*3];
                                inverse_value_rd -= temp[channel_index + (image_width*half_block_height_size)*3];
                            }
                            if (channel_index + (half_block_width_size + image_width*half_block_height_size)*3 <= decodedImage.size()) {
                                inverse_value_lu += temp[channel_index + (half_block_width_size + image_width*half_block_height_size)*3];
                                inverse_value_ru -= temp[channel_index + (half_block_width_size + image_width*half_block_height_size)*3];
                                inverse_value_ld -= temp[channel_index + (half_block_width_size + image_width*half_block_height_size)*3];
                                inverse_value_rd += temp[channel_index + (half_block_width_size + image_width*half_block_height_size)*3];
                            }
                            decodedImage[channel_index + (col + row*image_width)*3] = inverse_value_lu/2;
                            decodedImage[channel_index + (col + 1 + row*image_width)*3] = inverse_value_ru/2;
                            decodedImage[channel_index + (col + (row + 1)*image_width)*3] = inverse_value_ld/2;
                            decodedImage[channel_index + (col + 1 + (row + 1)*image_width)*3] = inverse_value_rd/2;
                        }                        
                    }
                }
            }
        }
    }    
}

void preprocess_inverse(std::vector<int>& decodedImage) {
    for (int pixel_id = 0; pixel_id < decodedImage.size()/3; ++pixel_id) {
        float Y = static_cast<float>(decodedImage[pixel_id*3]);
        float Cb = static_cast<float>(decodedImage[pixel_id*3 + 1]);
        float Cr = static_cast<float>(decodedImage[pixel_id*3 + 2]);
        decodedImage[pixel_id * 3] = static_cast<int>(
            clamp(Y + 1.402 * (Cr - 128) + 128, 0.0, 255.0)
        );
        decodedImage[pixel_id * 3 + 1] = static_cast<int>(
            clamp(Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128) + 128, 0.0, 255.0)
        );
        decodedImage[pixel_id * 3 + 2] = static_cast<int>(
            clamp(Y + 1.772 * (Cb - 128) + 128, 0.0, 255.0)
        );
    }   
}

void decodeHuffmanFile(const std::string& input_filename, const std::string& output_filename) {
    HuffmanResult result;

    if (!HuffmanResultIO::readHuffmanResultFromFile(input_filename, result)) {
        std::cerr << "Error reading Huffman result from file." << std::endl;
        return;
    }

    std::cout << "Decoded file details:" << std::endl;

    if (result.option == 1) {
        std::cout << "Option: Low Quality" << std::endl;
    } else if (result.option == 2) {
        std::cout << "Option: Medium Quality" <<std::endl;
    } else if (result.option == 3) {
        std::cout << "Option: High Quality" << std::endl;
    } else {
        std::cerr << "Unsupported option!" << std::endl;
    }
    std::cout << "Image width: " << result.real_image_width << std::endl;
    std::cout << "Image height: " << result.real_image_height << std::endl;

    std::vector<int> decodedImage = huffmanDecodeImage(result.bitstream, result.huffmanCodes);

    std::cout << "Decoded " << (decodedImage.size()-decodedImage.size()%3) << " bytes." << std::endl;

    quan_inverse(decodedImage, result.option, result.real_image_width, result.real_image_height, 128);

    dwt_inverse(decodedImage, result.real_image_width, result.real_image_height, 128);

    preprocess_inverse(decodedImage);

    saveImage(decodedImage, result.real_image_width, result.real_image_height, output_filename);

}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << " <output_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    decodeHuffmanFile(inputFile, outputFile);

    return 0;
}
