#include <systemc.h>
#include <string>
#include "DataStruct.h"
#include "preprocess_module.h"
#include "dwt_module.h"
#include "quan_module.h"
#include "entropy_module.h"
#include "write_module.h"


int sc_main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] 
                  << " <image_path> <output_path> <compression_option>" << std::endl;
        std::cerr << "compression_option: lowQF, mediumQF, highQF" << std::endl;
        return 1;
    }

    int option = 0;
    // Validate compression option
    std::string option_str = argv[3]; // Get the compression option string

    if (option_str == "lowQF") {
        option = 1;
    } else if (option_str == "mediumQF") {
        option = 2;
    } else if (option_str == "highQF") {
        option = 3;
    } else {
        std::cerr << "Invalid compression_option. Valid values are: lowQF, mediumQF, highQF" << std::endl;
        return 1;
    }

    // Define signals
    sc_signal<const char *> file_path_signal;  // Image path signal
    sc_signal<const char *> store_path_signal; // Storage path signal
    sc_signal<int> option_signal;
    sc_signal<bool> start_signal;              // Start signal
    sc_signal<bool> start_dwt_signal;          // Signal to control DWT module
    sc_signal<bool> start_quan_signal;         // Signal to control Quan module
    sc_signal<bool> start_entro_signal;        // Signal to control Encode module
    sc_signal<bool> start_write_signal;        // Signal to control Write module

    // Define FIFOs for image block transfer
    sc_fifo<BlockData> image_blocks_fifo;
    sc_fifo<BlockData> dwt_blocks_fifo;
    sc_fifo<BlockData> quan_blocks_fifo;
    sc_fifo<HuffmanResult> entro_result_fifo;

    // Create module instances
    ImagePreprocessModule img_proc("ImageProcessor");
    DWTModule dwt_proc("DWTProcessor");
    QuanModule quan_proc("QuanProcessor");
    EntropyEncoderModule entro_proc("EntroProcessor");
    WriteModule write_proc("WriteProcessor");

    // Bind ports to signals
    //---------------[1. Preprocess]---------------
    img_proc.start(start_signal);
    img_proc.input_file(file_path_signal);
    img_proc.output_blocks(image_blocks_fifo);  // Output image blocks to FIFO
    img_proc.start_dwt(start_dwt_signal);

    //---------------[2. DWT]---------------
    dwt_proc.start_dwt(start_dwt_signal);        // DWT module start signal
    dwt_proc.input_blocks(image_blocks_fifo);    // Get image blocks from FIFO
    dwt_proc.output_blocks(dwt_blocks_fifo);
    dwt_proc.start_quan(start_quan_signal);

    //---------------[3. Quan]---------------
    quan_proc.start_quan(start_quan_signal);
    quan_proc.option(option_signal);
    quan_proc.input_blocks(dwt_blocks_fifo);
    quan_proc.output_blocks(quan_blocks_fifo);
    quan_proc.start_entro(start_entro_signal);

    //---------------[4. Entropy]---------------
    entro_proc.start_entropy(start_entro_signal);
    entro_proc.input_blocks(quan_blocks_fifo);
    entro_proc.hResult(entro_result_fifo);
    entro_proc.start_write(start_write_signal);

    //---------------[5. Write]---------------
    write_proc.store_path(store_path_signal);
    write_proc.start_write(start_write_signal);  // Connect start_write signal
    write_proc.hResult(entro_result_fifo);      // Connect input FIFO signal

    // Start simulation by writing paths and start signals
    file_path_signal.write(argv[1]); // Write file path
    store_path_signal.write(argv[2]); // Write storage path
    option_signal.write(option);
    start_signal.write(true);         // Activate the start signal for image processing module

    // Start the simulation
    sc_start();

    // Stop the simulation (if no additional tasks)
    sc_stop();
    return 0;
}
