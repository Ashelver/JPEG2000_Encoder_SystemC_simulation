#ifndef IMAGE_WRITER_MODULE_H
#define IMAGE_WRITER_MODULE_H

#include <systemc>
#include <png.h>
#include "DataStruct.h"

using namespace sc_core;

class ImageWriterModule : public sc_module {
public:
    sc_in<bool> start_write;

    sc_fifo_in<BlockData> input_blocks; 

    SC_HAS_PROCESS(ImageWriterModule);

    ImageWriterModule(sc_module_name name);

private:
    void write_png(const char* filename, BlockData image_data);
    void process_write();
};

#endif // IMAGE_WRITER_MODULE_H
