#!/bin/bash

function build_yaml(){
    echo "========build yaml-cpp========="
    curr_path=${PWD}
    cd $curr_path/src/yaml-cpp
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$curr_path ../
    make install
}


function build_rtc(){
    echo "========build rtc base========="
    cd $curr_path/src/rtcbase

    rm -rf build
    mkdir build
    cd build
    cmake ../
    if test $# -gt 0 && test $1 = "clean"
    then
        echo "make clean"
        make clean
    else
        echo "make"
        make
    fi
    cp librtcbase.a ../../../lib/
    cd $curr_path/src/rtcbase
    cp -r third_party/include/absl ../../include
    cp third_party/lib/libabsl_strings.a ../../lib/
    cp third_party/lib/libabsl_throw_delegate.a ../../lib/
}



function main(){
    build_yaml
    build_rtc
}

main