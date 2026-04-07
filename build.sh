#!/bin/bash

BUILD_DIR=build

if [ -d "$BUILD_DIR" ]; then
	rm -R $BUILD_DIR
fi

mkdir $BUILD_DIR

cd $BUILD_DIR && cmake -DPDM_BOOT=ON .. && make -j