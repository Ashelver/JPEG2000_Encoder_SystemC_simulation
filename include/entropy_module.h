#ifndef ENTROPY_ENCODER_MODULE_H
#define ENTROPY_ENCODER_MODULE_H

#include <systemc.h>
#include <iostream>
#include <string> // Added this line to include the definition of std::string
#include "DataStruct.h"

// EntropyEncoderModule.h
class EntropyEncoderModule : public sc_module {
public:
    // Control signals
    sc_in<bool> start_entropy;         // Entropy encoding start signal
    sc_out<bool> start_write;          // Notify write module to start

    // Input data
    sc_fifo_in<BlockData> input_blocks;  // Input quantized blocks

    // Output data
    sc_fifo_out<HuffmanResult> hResult; // Output bitstream

    SC_HAS_PROCESS(EntropyEncoderModule);
    EntropyEncoderModule(sc_module_name name);

    // Define entropy encoding processing method
    void encode();

private:
    // Internal processing method
    HuffmanResult process_blocks(const BlockData& blocks);
};

#endif // ENTROPY_ENCODER_MODULE_H
