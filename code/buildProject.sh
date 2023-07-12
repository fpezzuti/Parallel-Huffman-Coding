COLOR='\033[0;36m'

echo "Building & running Huffman Coding ..."

rm -r build-dir

mkdir build-dir && cd build-dir

cd build-dir

cmake ..

cmake --build .

echo -e "\n- ${COLOR}To run the project: ./HuffmanCoding STANDARD nw [inputFileName]"

echo -e "- ${COLOR}To test the project: ./HuffmanCoding TIMING nw inputFileName {SEQ, TH, FF} nIter\n"