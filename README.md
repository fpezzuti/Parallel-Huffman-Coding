# Parallelization of Huffman Coding

## Project structure
The project is organized in the `/code/` directory as follows:
- directory *data*
- directory *include*
- directory *src*
- directory *test*
- main directory 

## How to compile the project
To correctly run the project, first make sure that *CMake* is already installed, then follow the steps described in the following.

Move to directory `/code/`:
```bash
    cd /code/
```
Launch the script to build the project:
```bash
    . ./buildProject.sh
```

## How to run the project
To execute the project, from directory `/build-dir/` use the following command:
```bash
./HuffmanCoding STANDARD nw [inputFileName]
```
The arguments of the program are:
- mode: {STANDARD, TIMING} (STANDARD stands for run, TIMING stands for tests)
- nw: integer (parallelism degree, it is ignored in case of run of sequential code)
- inputFileName (optional): string (input file name)

Once the program started, the list of the possible commands is shown to the user which in turn can type the number corresponding to the command they wants to execute. 

> Note: the input file must be previously inserted in the directory *"./data/inputs"*.

## How to test the project

### Collect timing performances
To collect the temporal performances of the program in all of its versions, move to directory `/test/` and run:
```bash
./timingTests.sh
```
### Test the performances of a specific version
To test a specific version of the program in terms of temporal performances, from directory `/build-dir/` simply run:
```bash
./HuffmanCoding TIMING nw inputFileName {SEQ, TH, FF} nIter {IOINLCUDED, IOEXCLUDED}
```
The arguments of the program are:
- mode: {STANDARD, TIMING} (STANDARD stands for run, TIMING stands for tests)
- nw: integer (parallelism degree, it is ignored in case of run of sequential code)
- inputFileName: string (input file name)
- version: {SEQ, TH, FF} (sequential, c++ threads, FastFlow)
- nIter: integer (number of iterations)

