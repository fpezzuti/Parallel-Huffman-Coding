#ifndef HUFFMANCODING_H // include guards
#define HUFFMANCODING_H

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include "utils.hpp"

const std::string STATISTICS_FILE_PATH = "./../data/statistics/statistics-";

class HuffmanCoding {
    protected:
        std::string INPUT_FILE_PATH = "test5.txt";
    
    public:
        
        virtual ~HuffmanCoding() = default;

        virtual void Run(int nw = 1) = 0;

        virtual void MeasureStats(int iter, int nw = 1) = 0;

        bool saveStatistics(long statistics[], const std::string& test_name) const{
            std::ofstream stats_file;
            stats_file.open(STATISTICS_FILE_PATH+test_name+".xls",  std::ios_base::app);

            stats_file << statistics[0];

            for(int i=1; i<6; ++i)
                stats_file << "\t" << statistics[i];

            stats_file << "\n";

            stats_file.close();

            return true;
        }

        bool createStatisticsFile(const std::string& test_name) const {
            std::ofstream stats_file;
            stats_file.open(STATISTICS_FILE_PATH+test_name+".xls",  std::ios_base::trunc);
            stats_file << "Input file:\t" + INPUT_FILE_PATH << "\n";
            stats_file << "Completion\tRead File\tGet Symbol Freqs.\tTree Construction\tFile Compression\tWrite File\n";
            stats_file.close();
            return true;
        }

        void printStatistics(long statistics[]) const {
            std::cout << "\nTest Statistics (μs):\n";
            std::cout << "\tCompletion time:\t\t\t" << statistics[0] << "\n";
            std::cout << "\t--------------------------------------------\t\t\t\n";
            std::cout << "\tInput File Read.:\t\t\t" << statistics[1] << "\n";
            std::cout << "\tCount Symbol Freq.:\t\t\t" << statistics[2] << "\n";
            std::cout << "\tTree Construction:\t\t\t" << statistics[3] << "\n";
            std::cout << "\tFile Compression:\t\t\t" << statistics[4] << "\n";
            std::cout << "\tOutput File Write:\t\t\t" << statistics[5] << "\n";
        }
        
        void cleanOutputFile(){
            std::ofstream ofs;
            ofs.open(OUTPUT_DATA_PATH+INPUT_FILE_PATH+".bin",  std::ios_base::trunc);
            ofs.close();   
        }
};

#endif