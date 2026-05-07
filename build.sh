#!/bin/bash


BUILD_DIR=build

if [ -d "$BUILD_DIR" ]; then
	rm -R $BUILD_DIR
fi

mkdir $BUILD_DIR
mkdir bin


cd $BUILD_DIR && cmake -DCMAKE_BUILD_TYPE=Debug-bootloader .. && make -j
# cp -fr keypad_boot.bin ../bin/ && cd ..
# rm -R $BUILD_DIR/*

# cd $BUILD_DIR && cmake -DCMAKE_BUILD_TYPE=Release-loader .. && make -j
# cp -fr keypad.bin ../bin/ && cd ..