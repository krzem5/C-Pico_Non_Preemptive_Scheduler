#!/bin/bash
if [ ! -d "build" ]; then
	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Release ..
	cd ..
fi
cd build
make -j16&&[[ -d "$PICO_DRIVE_PATH" ]]&&cp scheduler.uf2 "$PICO_DRIVE_PATH/scheduler.uf2"
cd ..
