#!/bin/bash

curr_path=${PWD}

function build_yaml(){
    echo "========build yaml-cpp========="
    cd $curr_path/src/yaml-cpp
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$curr_path/yaml-cpp ../
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
    cp librtcbase.a $curr_path/lib/
    cd $curr_path/src/rtcbase
    cp -r third_party/include/* $curr_path/include
    cp third_party/lib/libabsl_strings.a $curr_path/lib/
    cp third_party/lib/libabsl_throw_delegate.a $curr_path/lib/
    cp third_party/lib/libssl.a $curr_path/lib/
    cp third_party/lib/libcrypto.a $curr_path/lib/
    cp third_party/lib/libabsl_bad_optional_access.a $curr_path/lib/
}

function build_libev(){
    echo "========build libev========="
    cd $curr_path/src/libev
    ./configure --prefix=$curr_path/ev
    make 
    make install
}

function build_jsoncpp(){
    echo "========build jsoncpp========="
    cd $curr_path/src/jsoncpp
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$curr_path/jsoncpp ../
    make
    make install
}

function build_openssl(){
    echo "========build openssl========="
    cd $curr_path/src/openssl
    ./Configure --prefix=$curr_path/ssl
    make clean
    make 
    make install
}

build_srtp(){
    echo "========build srtp========="
    cd $curr_path/src/libsrtp
    ./configure --prefix=$curr_path/libsrtp
    make clean
    make 
    make install
}

function install(){
    cd $curr_path
    cp -r yaml-cpp/include/yaml-cpp include/
    if [ -d yaml-cpp/lib64 ]; then
        cp yaml-cpp/lib64/lib*.a lib/
    fi
    if [ -d yaml-cpp/lib ]; then
        cp yaml-cpp/lib/lib*.a lib/
    fi

    cp -r ev/include/* include/
    if [ -d ev/lib64 ]; then
        cp ev/lib64/lib*.a lib/
    fi
    if [ -d ev/lib ]; then
        cp ev/lib/lib*.a lib/
    fi

    cp -r jsoncpp/include/json include/
    if [ -d jsoncpp/lib64 ]; then
        cp jsoncpp/lib64/lib*.a lib/
    fi
    if [ -d jsoncpp/lib ]; then
        cp jsoncpp/lib/lib*.a lib/
    fi

    cp -r libsrtp/include/* include/
    if [ -d libsrtp/lib64 ]; then
        cp libsrtp/lib64/lib*.a lib/
    fi
    if [ -d libsrtp/lib ]; then
        cp libsrtp/lib/lib*.a lib/
    fi

    rm -rf ssl
    rm -rf jsoncpp
    rm -rf ev
    rm -rf yaml-cpp 
    rm -rf libsrtp
}

function main(){
    mkdir -p $curr_path/include
    mkdir -p $curr_path/lib
    build_yaml
    build_rtc
    build_libev
    build_jsoncpp
    build_srtp
    install
}

main