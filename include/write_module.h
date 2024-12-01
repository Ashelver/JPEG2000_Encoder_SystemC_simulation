#ifndef WRITE_MODULE_H
#define WRITE_MODULE_H

#include <systemc.h>
#include <iostream>
#include <string>
#include "DataStruct.h"
#include "huffmanIO.h"

// WriteEncoderModule.h
class WriteModule : public sc_module {
public:
    // Control signals
    sc_in<bool> start_write;         // Start signal for entropy encoding

    // Input parameters
    sc_in<const char *> store_path;  // Path to store the output file

    // Input data
    sc_fifo_in<HuffmanResult> hResult;  // Input Huffman result data

    SC_HAS_PROCESS(WriteModule);
    WriteModule(sc_module_name name);

    // Define the method for writing the Huffman result
    void write();

private:
    // Internal method for writing the Huffman result to a file
    void writehResult(const HuffmanResult& hResult, const std::string& file_path);
};

#endif // WRITE_MODULE_H
