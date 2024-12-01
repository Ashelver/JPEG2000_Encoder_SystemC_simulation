#ifndef QUAN_MODULE_H
#define QUAN_MODULE_H

#include <systemc.h>
#include <iostream>
#include "DataStruct.h"

// quan_module.h
class QuanModule : public sc_module {
public:
    // Control signals
    sc_in<bool> start_quan;    // Start signal for quantization
    sc_in<int> option;         // Option for quantization (could be for choosing different strategies)
    sc_out<bool> start_entro;  // Output signal to start entropy encoding

    // Input data
    sc_fifo_in<BlockData> input_blocks; // Frequency band input

    // Output data
    sc_fifo_out<BlockData> output_blocks; // Output image blocks

    SC_HAS_PROCESS(QuanModule);
    QuanModule(sc_module_name name);

    void performQuan();

private:
    // Internal processing methods
    void process_block();

    void process_block(std::vector<std::vector<std::vector<float>>>& block, 
                       std::vector<std::vector<std::vector<int>>>& mask, 
                       float qf_ll, float qf_hl, float qf_lh, float qf_hh); // Process the quantization of a block
};

#endif // QUAN_MODULE_H
