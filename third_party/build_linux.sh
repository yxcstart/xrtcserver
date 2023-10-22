#!/bin/bash

curr_path=${PWD}

function build_yaml(){
    echo "========build yaml-cpp========="
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

function build_libev(){
    echo "========build libev========="
    cd $curr_path/src/libev
    ./configure --prefix=$curr_path
    make 
    make install
}

function build_jsoncpp(){
    echo "========build jsoncpp========="
    cd $curr_path/src/jsoncpp
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$curr_path ../
    make
    make install
}

function main(){
    build_yaml
    build_rtc
    build_libev
    build_jsoncpp
}

main