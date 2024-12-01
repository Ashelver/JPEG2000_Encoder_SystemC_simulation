#ifndef DWT_MODULE_H
#define DWT_MODULE_H

#include <systemc.h>
#include <iostream>
#include "DataStruct.h"

// dwt_module.h
class DWTModule : public sc_module {
public:
    // Control signals
    sc_in<bool> start_dwt;        // DWT start signal
    sc_out<bool> start_quan;
    
    // Input data
    sc_fifo_in<BlockData> input_blocks;   // IMAGE input

    // Output data
    sc_fifo_out<BlockData> output_blocks; // DWT output


    SC_HAS_PROCESS(DWTModule);
    DWTModule(sc_module_name name);

    // Define DWT processing method
    void performDWT();
    
private:

    void process_block(std::vector<std::vector<std::vector<float>>>& block, 
                       std::vector<std::vector<std::vector<int>>>& mask, 
                       int width, int height);
};

#endif // DWT_MODULE_H
