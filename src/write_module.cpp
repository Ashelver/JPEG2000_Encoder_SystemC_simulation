#include "write_module.h"

// Constructor to initialize the module
WriteModule::WriteModule(sc_module_name name) : sc_module(name) {
    SC_THREAD(write);  // Bind the write method as a SystemC thread
    sensitive << start_write;
}

// Write method
void WriteModule::write() {
    wait();
    if (start_write.read()) {
        // Get the storage path
        const char* path = store_path.read();
        std::string file_path(path);

        HuffmanResult HResult;
        hResult.read(HResult);

        writehResult(HResult, file_path);
    }

    std::cout << "[WRITE MODULE] Finished" << std::endl;
}

// Internal write method: write the Huffman result to a file
void WriteModule::writehResult(const HuffmanResult& hResult, const std::string& file_path) {
    // Call the HuffmanResultIO class's write function to save the Huffman result to the specified path
    bool success = HuffmanResultIO::writeHuffmanResultToFile(file_path, hResult);
    if (!success) {
        std::cerr << "Failed to write Huffman result to file: " << file_path << std::endl;
    } else {
        std::cout << "[WRITE MODULE] Successfully wrote Huffman result to file: " << file_path << std::endl;
    }
}
