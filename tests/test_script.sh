#!/bin/bash

# Compile llvm ir to bitcode,
# run the bitcode and print the return value
# need to install llvm tool: sudo apt install llvm

SOURCE=$1
BCODE="${SOURCE%.ll}.bc"
llvm-as $SOURCE -o $BCODE
lli $BCODE
echo $?