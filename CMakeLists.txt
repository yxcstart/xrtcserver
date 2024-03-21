cmake_minimum_required(VERSION 3.5)

project(xrtcserver)

add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX
    -DWEBRTC_LINUX")

include_directories(
    "./src"
    "./third_party/include"
    "./third_party/src/rtcbase/src"
)

link_directories(
    "./third_party/lib"
)

file(GLOB all_src 
    "./src/*.cpp" 
    "./src/base/*.cpp" 
    "./src/server/*.cpp"  
    "./src/stream/*.cpp"
    "./src/pc/*.cpp"
    "./src/ice/*.cpp"
    "./src/module/rtp_rtcp/*.cpp"
)

add_executable(xrtcserver ${all_src})

target_link_libraries(xrtcserver libyaml-cpp.a librtcbase.a
    libabsl_strings.a libabsl_throw_delegate.a libev.a libjsoncpp.a libsrtp2.a
    libssl.a libcrypto.a -lpthread -ldl libabsl_bad_optional_access.a -no-pie
)