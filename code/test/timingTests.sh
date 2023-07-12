#!/bin/bash

echo "Testing timing ..."

# execFile executionMode nw fileName codeVersion nIterations

# tests for sequential version
./../build-dir/HuffmanCoding TIMING 1 toy-test.txt SEQ 1

# tests for c++ threads version
./../build-dir/HuffmanCoding TIMING 1 toy-test.txt TH 12


# tests for FastFlow version
./../build-dir/HuffmanCoding TIMING 1 toy-test.txt FF 12
