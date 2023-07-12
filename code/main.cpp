
#include <iostream>
#include <chrono>
#include <vector>
#include "include/sequential.hpp"
#include "include/par_thread.hpp"
#include "include/par_fastflow.hpp"
#include "include/utils.hpp"
#include "include/huff_node.hpp"

void runCLI(int nw, std::string input_fname = ""){
    for(;;) {
        
        std::cout << "\nPlease specify which version should be run:\n";
        std::cout <<"\t1) sequential version\n";
        std::cout <<"\t2) c++ threads version\n";
        std::cout <<"\t3) FastFlow version\n";
        std::cout <<"\t4) End the service\n";
        int command;

        std::cin >> command;

        HuffmanCoding* h;
        
        switch (command) {
            case 1:
                std::cout << "Running sequential version ...\n";
                h = (input_fname.compare("")==0) ? new SequentialHuffuman() : new SequentialHuffuman(input_fname);
                break;
            case 2:
                std::cout << "Running c++ threads version ...\n";
                h = (input_fname.compare("")==0) ? new ParThreadHuffuman() : new ParThreadHuffuman(input_fname);
                break;
            case 3:
                std::cout << "Running FastFlow version ...\n";
                h = (input_fname.compare("")==0) ? new ParFastflowHuffuman() : new ParFastflowHuffuman(input_fname);
                break;
            case 4:
                std::cout << "Service terminated, bye.\n";
                exit(0);
                break;
            default:
                std::cout << "Error, unrecognized command\n";
                break;
        }

        h->Run(nw);
        delete h;

    }
}

void runTimingTests(const std::string& version, int iter, std::string input_fname, int nw){
    
    if(version.compare("SEQ")==0){

        SequentialHuffuman s = SequentialHuffuman(input_fname);
        s.MeasureStats(iter);

    } else if(version.compare("TH")==0){
        
        ParThreadHuffuman th = ParThreadHuffuman(input_fname);
        th.MeasureStats(iter, nw);
    }
    else if(version.compare("FF")==0){

        ParFastflowHuffuman ffh = ParFastflowHuffuman(input_fname);
        ffh.MeasureStats(iter, nw);

    } else {
        std::cerr << "Error: wrong version name, try again with {SEQ, TH, FF}." << std::endl;
        return;
    }
}

int main(int argc, const char *argv[]){

    if(argc < 3){
        std::cerr << "Arguments of " << argv[0] << ": {\"TIMING\", \"STANDARD\"} nw [input file]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string mode = argv[1];
    int nw = strtol(argv[2], NULL, 10);

    std::string input_fname;

    if(argc > 3){
        input_fname = argv[3];
    }

    std::cout << "\n****** HUFFMAN CODING ******\n" << std::endl;

    if(mode.compare("STANDARD")==0){
        if(argc == 3)
            runCLI(nw);
        else
            runCLI(nw, input_fname);
    } else if(mode.compare("TIMING")==0){
        if(argc < 6){
            std::cerr << "Arguments of " << argv[0] << " in TIMING mode are: nw {TIMING} inputFile {SEQ, TH, FF} nTimes" << std::endl;
            return EXIT_FAILURE;
        }

        std::string version = argv[4];
        int iter = strtol(argv[5], NULL, 10);
        
        
        std::cout << "Starting timing tests of " << version << " version with " << iter << " iterations and " << nw << " workers on file " << input_fname << "\n";

        runTimingTests(version, iter, input_fname, nw);

        
    } else {
        std::cerr << "wrong execution mode"; 
        return EXIT_FAILURE;
    }

    return 0;
}