#include "./../include/par_thread.hpp"
#include "./../include/utimer.hpp"

template <typename Tout> Tout MapAndReduceFreqs(
    int nw,
    std::string& input,
    std::function<Tout (const std::string&, int, int)> mapfun
    ){

    bool end = false;

    // synchronization/protection mechanisms
    std::mutex mux_results;
    std::condition_variable cond_results;

    // shared-protected variable
    int processed = 0;
    std::vector<Tout> results; // vector of mappers' results
    
   
    auto body = [&] (int i){ // producer function (mapper)

        int chunk_size = input.size() / nw; // chunk size
        int start = i*chunk_size; // starting element of assigned chunk

        if(i==(nw-1)) // correct size of last chunk
            chunk_size += (input.size() % nw);
        
        Tout lres = mapfun(input, start, chunk_size);

        { // lock the mutex until the function ends
            std::lock_guard my_lock(mux_results);
            results.push_back(lres); // compute map function over the chunk
            //std::cout << "thread: "<< i << "\tstart: " << start << "\tchunk size: " << chunk_size <<"\n"; 
        }
       
        // notify the consumer (the reducer, so the main)
        cond_results.notify_one();
    };


    thread tids[nw]; // fork join 
    for(int i=0; i<nw; ++i)
        tids[i] = thread(body, i);

        do{
            // lock the mutex until the function ends
            std::unique_lock res_lock(mux_results);

            // wait on the condition variable specifying the lock acquired
            // and pass the condition we wait when awaken (we test again the condition)
            cond_results.wait(
                res_lock,
                [&] { // check condition 
                    return (results.size() >=2 || (results.size()==1 && processed == nw) || (results.size()==1 && nw==1));
                });

            // now the consumer is awaken and owns the mutex
            processed += results.size()*(processed==0)+((results.size()-1)*(processed>0));

            reducePartialresults(results);

        } while(processed<nw);

    for(int i=0; i<nw; ++i)
        tids[i].join();

    return results.front();
}

void reducePartialresults(std::vector<std::unordered_map<char, int>>& partial_maps){
    std::unordered_map<char, int> merged_map;

    for(int i=0; i<partial_maps.size(); ++i){
        auto map = partial_maps.at(i);
        for(auto& symb : map)
            merged_map[symb.first] += symb.second;
    }
    
    partial_maps.clear();
    partial_maps.push_back(merged_map);
}

template <typename ToutMap, typename ToutRed> bool PipeMapAndReduceCompression(
    int nw,
    const std::string& fname,
    std::string& input,
    std::function<ToutMap (const std::string&, int, int)> mapfun,
    std::function<bool(const std::string&, const std::string&, ToutRed&, int&)> redfun
    ){

    bool success = true; // ret value
    std::map<int, ToutMap> results; // vector of mappers' results

    int processed_chunks = 0;

    auto fsize = std::filesystem::file_size(INPUT_DATA_PATH+fname);

    // synchronization/protection mechanisms
    std::mutex mux_results;
    std::condition_variable cond_results;
   
    auto body = [&] (int i){ // producer function (mapper)

        int chunk_size = fsize / nw; // chunk size
        int start = i*chunk_size; // starting element of assigned chunk;

        if(i==(nw-1)) // correct size of last chunk
            chunk_size += (fsize % nw);
        
        ToutMap lres = mapfun(input, start, chunk_size); // compute map function over the chunk

        { // lock the mutex until the function ends
            std::lock_guard res_lock(mux_results);
            results[i] = lres; // write the result
        }
       
        // notify the consumer (the reducer, so the main)
        cond_results.notify_one();
    };

    
    thread tids[nw]; // fork join 

    for(int i=0; i<nw; ++i)
        tids[i] = thread(body, i);


    ToutMap lres;

    int wbits = 0;

    ToutRed writable_bitstring;

    do{
        
        {
            // lock the mutex until the function ends
            std::unique_lock res_lock(mux_results);

            // wait on the condition variable specifying the lock acquired
            // and pass the condition we wait when awaken (we test again the condition)
            cond_results.wait(
                res_lock,
                [&] { // check condition (the next chunk to be processed is ready)
                    return (results.size() > 0 && ((results.begin())->first == processed_chunks));
                });

            // now the consumer is awaken and owns the mutex

            lres = results[processed_chunks];

            results.extract(results.begin());    
        }

        ++processed_chunks;

        success = std::invoke(redfun, fname, lres, writable_bitstring, wbits);

    } while(processed_chunks<nw && success);

    if(writable_bitstring.size()>0 && success) // check if there are remaining bits to write
        success = writeConvertedBitStringToFile(writable_bitstring, fname, 0);

    for(int i=0; i<nw; ++i)
        tids[i].join();

    return success;
}

ParThreadHuffuman::ParThreadHuffuman(const std::string& input_file) {
    INPUT_FILE_PATH = input_file;
}

void ParThreadHuffuman::Run(int nw) {
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


    std::function<std::unordered_map<char, int>(const std::string&, int, int)> fmapfreqs =
        [&] (const std::string& s, int from, int size) {
            std::unordered_map<char, int> lalphabet;
            countSymbolFrequencies(s, from, size, lalphabet);
            return lalphabet;
        };

    alphabet = MapAndReduceFreqs<std::unordered_map<char, int>>(nw, file_string, fmapfreqs);
                
                
    std::unordered_map<char, std::string> compression_table;

            

    HuffNode* tree = buildHuffTree(&alphabet);

    compression_table = buildCompressionTable(*tree);

    printCompressionTable(compression_table);

    deleteTree(tree);
          
                
    std::function<std::string(const std::string&, int, int)> fmapcompress = [&] (const std::string& s, int from, int size) {
    
        std::string compressed_file_string;
        compressed_file_string.reserve(s.size());

        for(int i=0; i<size; ++i)
            compressed_file_string += compression_table[s[from+i]];

        return compressed_file_string;
    };

    std::function<bool(const std::string&, const std::string&, std::vector<unsigned char>&, int&)> fred =
        [&] (const std::string& fname,
                const std::string& bitstring,
                std::vector<unsigned char>& writable_bitstring,
                int& wbits
                ) {
            
                wbits = convertToWritableBitString(bitstring, writable_bitstring, wbits);

                return writeConvertedBitStringToFile(writable_bitstring, fname, wbits);
        };

    bool success = PipeMapAndReduceCompression<std::string, std::vector<unsigned char>>(nw, INPUT_FILE_PATH, file_string, fmapcompress, fred);

    std::cout << "Compression of " << INPUT_FILE_PATH << " completed.\n";
    std::cout << "Output written in " << OUTPUT_DATA_PATH + INPUT_FILE_PATH << "\n";   
}

void ParThreadHuffuman::MeasureStats(int iter, int nw) { 

    createStatisticsFile("TH");

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

                std::function<std::unordered_map<char, int>(const std::string&, int, int)> fmapfreqs =
                    [&] (const std::string& s, int from, int size) {
                        std::unordered_map<char, int> lalphabet;
                        countSymbolFrequencies(s, from, size, lalphabet);
                        return lalphabet;
                    };

                alphabet = MapAndReduceFreqs<std::unordered_map<char, int>>(nw, file_string, fmapfreqs);
                
            }
                
            std::unordered_map<char, std::string> compression_table;

            {
                utimer  timer(&statistics[3]);

                HuffNode* tree = buildHuffTree(&alphabet);

                compression_table = buildCompressionTable(*tree);

                //printCompressionTable(compression_table); DEBUG PURPOSES

                deleteTree(tree);
            }

            {
                utimer  timer(&statistics[4]);
                
                std::function<std::string(const std::string&, int, int)> fmapcompress = [&] (const std::string& s, int from, int size) {
                
                    std::string compressed_file_string;
                    compressed_file_string.reserve(s.size());

                    for(int i=0; i<size; ++i)
                        compressed_file_string += compression_table[s[from+i]];

                    return compressed_file_string;
                };

                std::function<bool(const std::string&, const std::string&, std::vector<unsigned char>&, int&)> fred =
                    [&] (const std::string& fname,
                         const std::string& bitstring,
                         std::vector<unsigned char>& writable_bitstring,
                         int& wbits
                         ) {
                        
                            wbits = convertToWritableBitString(bitstring, writable_bitstring, wbits);

                            return writeConvertedBitStringToFile(writable_bitstring, fname, wbits);
                    };

                bool success = PipeMapAndReduceCompression<std::string, std::vector<unsigned char>>(nw, INPUT_FILE_PATH, file_string, fmapcompress, fred);
                
            }
    
            {
                utimer  timer(&statistics[5]);
            }
        }

        saveStatistics(statistics, "TH");
        printStatistics(statistics);

        std::cout << "written file size: " << std::filesystem::file_size(OUTPUT_DATA_PATH+INPUT_FILE_PATH+".bin") << "\n";
    }     
}