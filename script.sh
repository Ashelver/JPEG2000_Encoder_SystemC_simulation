#!/bin/bash

# Clean up compilation files
make clean

# Compile the project
make

# Decoder
g++ -std=c++11 -I./include -o jpeg2000_decoder ./func/decoder.cpp ./func/huffman_encoder_decoder.cpp ./func/huffmanIO.cpp -lpng



# Run test program
# Lenna
./jpeg2000_encoder.x ./testcase/Lenna.png ./high_lenna.jp2 highQF
./jpeg2000_encoder.x ./testcase/Lenna.png ./medium_lenna.jp2 mediumQF
./jpeg2000_encoder.x ./testcase/Lenna.png ./low_lenna.jp2 lowQF

./jpeg2000_decoder ./high_lenna.jp2 ./high_lenna.png
./jpeg2000_decoder ./medium_lenna.jp2 ./medium_lenna.png
./jpeg2000_decoder ./low_lenna.jp2 ./low_lenna.png


# Larger image
./jpeg2000_encoder.x ./testcase/Large_image.png ./high_Large_image.jp2 highQF
./jpeg2000_encoder.x ./testcase/Large_image.png ./medium_Large_image.jp2 mediumQF
./jpeg2000_encoder.x ./testcase/Large_image.png ./low_Large_image.jp2 lowQF

./jpeg2000_decoder ./high_Large_image.jp2 ./high_Large_image.png
./jpeg2000_decoder ./medium_Large_image.jp2 ./medium_Large_image.png
./jpeg2000_decoder ./low_Large_image.jp2 ./low_Large_image.png