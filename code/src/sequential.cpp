
#include "./../include/sequential.hpp"


SequentialHuffuman::SequentialHuffuman(std::string input_file) {
    INPUT_FILE_PATH = input_file;
}

void SequentialHuffuman::Run(int nw) {
    
    cleanOutputFile();

    std::string file_string;
    const int fsize = std::filesystem::file_size(INPUT_DATA_PATH+INPUT_FILE_PATH);

    if(fsize==0){
        std::cout << "empty file\n";
        return;
    }


    file_string.reserve(fsize);

    bool flag = readFile(INPUT_FILE_PATH, &file_string, fsize);


    std::unordered_map<char, int> alphabet;

    countSymbolFrequencies(file_string, 0, fsize, alphabet);
            

    HuffNode* tree = buildHuffTree(&alphabet);


    std::unordered_map<char, std::string> compression_table;

    compression_table.reserve(alphabet.size());

    compression_table = buildCompressionTable(*tree);

    printCompressionTable(compression_table);

    deleteTree(tree);


    std::string compressed_bit_string;

    compressed_bit_string.reserve(fsize);
        
    for(int i=0; i<fsize; ++i)
        compressed_bit_string += compression_table[file_string[i]];

    std::vector<unsigned char> writable_bit_string;

    convertToWritableBitString(compressed_bit_string, writable_bit_string);

    bool success = writeConvertedBitStringToFile(writable_bit_string, INPUT_FILE_PATH);

    std::cout << "Compression of " << INPUT_FILE_PATH << " completed.\n";
    std::cout << "Output written in " << OUTPUT_DATA_PATH + INPUT_FILE_PATH << "\n";   
}

void SequentialHuffuman::MeasureStats(int iter, int nw) {

    createStatisticsFile("SEQ");

    for(int i=0; i<iter; ++i){
        long statistics[6];
        
        cleanOutputFile();

        {
            utimer  timer(&statistics[0]);

            std::string file_string;
            const int fsize = std::filesystem::file_size(INPUT_DATA_PATH+INPUT_FILE_PATH);

            if(fsize==0){
                std::cout << "empty file\n";
                return;
            }

            file_string.reserve(fsize);

            {
                utimer  timer(&statistics[1]);
                
                bool flag = readFile(INPUT_FILE_PATH, &file_string, fsize);
            }


            std::unordered_map<char, int> alphabet;
            
            {
                utimer timer(&statistics[2]);
               
                countSymbolFrequencies(file_string, 0, fsize, alphabet);
            }
                
            std::unordered_map<char, std::string> compression_table;
            compression_table.reserve(alphabet.size());
            
            {
                utimer  timer(&statistics[3]);
                
                HuffNode* tree = buildHuffTree(&alphabet);

                compression_table = buildCompressionTable(*tree);

                //printCompressionTable(compression_table); // DEBUG PURPOSES

                deleteTree(tree);
            }

            std::string compressed_bit_string;

            compressed_bit_string.reserve(fsize);
            {
                utimer  timer(&statistics[4]);

                for(int i=0; i<fsize; ++i)
                    compressed_bit_string += compression_table[file_string[i]];
            }
    
            {
                utimer  timer(&statistics[5]);
                std::vector<unsigned char> writable_bit_string;

                convertToWritableBitString(compressed_bit_string, writable_bit_string);

                bool success = writeConvertedBitStringToFile(writable_bit_string, INPUT_FILE_PATH);
            }

        }

        saveStatistics(statistics, "SEQ");
        printStatistics(statistics);

        std::cout << "written file size: " << std::filesystem::file_size(OUTPUT_DATA_PATH+INPUT_FILE_PATH+".bin") << "\n";
    }         
}

