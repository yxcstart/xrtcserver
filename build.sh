#!/bin/bash

project_path="$(pwd)"
build_path=${project_path}/build
echo "build path = ${build_path}"
# rm -rf ${build_path}
# if [ $? -ne 0 ]; then ret=$?; echo "failed, ret=$ret, rm path=${build_path}"; exit 1; fi
# mkdir -p ${build_path}
# if [ $? -ne 0 ]; then ret=$?; echo "failed, ret=$ret, mkdir path=${build_path}"; exit 1; fi


# cd ${project_path}/third_party/
# ./build_linux.sh
# echo "build submodles success"

cd ${project_path} 

cd ${build_path} && cmake ../

if test $# -gt 0 && test $1 = "clean"
then
    echo "make clean"
    make clean
else
    echo "make"
    make
fi