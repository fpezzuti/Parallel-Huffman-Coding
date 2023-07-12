#include "./../include/par_fastflow.hpp"
#include "./../include/utimer.hpp"

using namespace ff;

std::string fstring_ff;
std::unordered_map<char, std::string> compression_table_ff;

ParFastflowHuffuman::ParFastflowHuffuman(const std::string& input_file) {
    INPUT_FILE_PATH = input_file;
}

Task* Worker_compression::svc(Task* t){
    char c;
    for(int i=0; i<t->csize; ++i){
        c = fstring_ff[t->start+i];
        t->compressed_bstring += compression_table_ff[c]; // compress the chunk
    }
    return t;
}

Task* Emitter_compression::svc(Task* ){
    int csize = fstring_ff.size() / nw;

    for(int i=0; i<nw; ++i){ // split the file in chunks
        int start = i*csize;

        if(i==(nw-1)) // correct size of last chunk
            csize += (fstring_ff.size() % nw);

        Task* t = new Task(start, csize);

        ff_send_out(t);
    }
    return EOS;
}

Task* Collector_compression::svc(Task* t){ 
    
    --to_collect;

    wbits = convertToWritableBitString(t->compressed_bstring, writable_bitstring, wbits);

    bool success = writeConvertedBitStringToFile(writable_bitstring, fname, wbits);

    if(to_collect == 0 && writable_bitstring.size()>0 && success) // check if there are remaining bits to write
        success = writeConvertedBitStringToFile(writable_bitstring, fname, 0);

    delete t;
    return GO_ON;
}

bool MapFreqsCount(const std::string& fname, const int nw, std::unordered_map<char, int>& alphabet, const std::string& fstring){

    ParallelForReduce<std::unordered_map<char, int> > pfr(nw);
    
    auto fred = [](std::unordered_map<char, int> &merged_alphabet, const std::unordered_map<char, int>  &local_alphabet) { // reduce function

        for(const std::pair<char, int>& symb : local_alphabet)
            merged_alphabet[symb.first] += symb.second;
    };

    auto ffor = [&](const long i, std::unordered_map<char, int> &local_alphabet) { // iteration function
            local_alphabet[fstring[i]] +=1;   
    };
  
    pfr.parallel_reduce_static(alphabet, alphabet, 0, fstring.size(), 1, 0, ffor, fred, nw);

    return true;
}

bool PipeMapCompression(const std::string& fname, const int nw){
    Emitter_compression emitter(nw);
    Worker_compression worker;
    Collector_compression collector(fname, nw);

    std::vector<std::unique_ptr<ff_node>> workers;

    // prepare workers vector
    for(int i=0; i<nw; ++i)
        workers.push_back(make_unique<Worker_compression>(worker));

    // declare an ordered farm with customized emitter and collector
    ff_OFarm<Task> farm_compression(move(workers));
    farm_compression.add_emitter(emitter);    
    farm_compression.add_collector(collector);

    // run and wait the farm
    if(farm_compression.run_and_wait_end()<0){
        std::cout << "error running frequency count\n";
        return false;
    }
    return true;
}

void ParFastflowHuffuman::Run(int nw) {

    cleanOutputFile();

    const int fsize = std::filesystem::file_size(INPUT_DATA_PATH+INPUT_FILE_PATH);

    if(fsize==0){
        std::cout << "empty file\n";
        return;
    }

    fstring_ff.reserve(fsize);

    fstring_ff.clear();


    bool flag = readFile(INPUT_FILE_PATH, &fstring_ff, fsize);


    std::unordered_map<char, int> alphabet;

    MapFreqsCount(INPUT_FILE_PATH, nw, alphabet, fstring_ff);


    HuffNode* tree = buildHuffTree(&alphabet);

    compression_table_ff = buildCompressionTable(*tree);

    printCompressionTable(compression_table_ff);

    deleteTree(tree);

    PipeMapCompression(INPUT_FILE_PATH, nw); 

    std::cout << "Compression of " << INPUT_FILE_PATH << " completed.\n";
    std::cout << "Output written in " << OUTPUT_DATA_PATH + INPUT_FILE_PATH << "\n";       
}

void ParFastflowHuffuman::MeasureStats(int iter, int nw) { 

    createStatisticsFile("FF");

    for(int i=0; i<iter; ++i){
        long statistics[6];
        cleanOutputFile();

        {
            utimer  timer(&statistics[0]);

            const int fsize = std::filesystem::file_size(INPUT_DATA_PATH+INPUT_FILE_PATH);

            if(fsize==0){
                std::cout << "empty file\n";
                return;
            }

            fstring_ff.reserve(fsize);

            fstring_ff.clear();

            {
                utimer  timer(&statistics[1]);
                bool flag = readFile(INPUT_FILE_PATH, &fstring_ff, fsize);
            }


            std::unordered_map<char, int> alphabet;
            {
                utimer timer(&statistics[2]);
                MapFreqsCount(INPUT_FILE_PATH, nw, alphabet, fstring_ff);
            }

            {
                utimer  timer(&statistics[3]);

                HuffNode* tree = buildHuffTree(&alphabet);

                compression_table_ff = buildCompressionTable(*tree);

                deleteTree(tree);
            }

            {
                utimer  timer(&statistics[4]);
                PipeMapCompression(INPUT_FILE_PATH, nw);
            }
    
            { 
                utimer  timer(&statistics[5]);
            }
        }

        saveStatistics(statistics, "FF");
        printStatistics(statistics);

        std::cout << "written file size: " << std::filesystem::file_size(OUTPUT_DATA_PATH+INPUT_FILE_PATH+".bin") << "\n";
    }     
}