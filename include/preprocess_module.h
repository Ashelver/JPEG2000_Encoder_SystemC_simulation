#ifndef IMAGE_MODULE_H
#define IMAGE_MODULE_H

#include <systemc.h>
#include <vector>
#include <string>
#include "DataStruct.h"

class ImagePreprocessModule : public sc_module {
public:
    // Control signals
    sc_in<bool> start;           // Start signal to control image processing
    sc_out<bool> start_dwt;      // Output signal to control the DWT module

    // Input parameters
    sc_in<const char *> input_file;  // Input file path

    // Output data
    sc_fifo_out<BlockData> output_blocks; // Output image blocks

    // Constructor
    SC_HAS_PROCESS(ImagePreprocessModule);
    ImagePreprocessModule(sc_module_name name);

    // Module processing logic
    void process_image();

private:
    // Helper functions
    void RGBtoYCbCr(float r, float g, float b,
                    float &y, float &cb, float &cr);

    BlockData readPngAndsplitIntoBlocks(
        const char *filename, int &width, int &height, int block_width, int block_height);

    void performDCShiftAndRGBtoYCbCr(BlockData &block_data);
};

#endif // IMAGE_MODULE_H
