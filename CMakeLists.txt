cmake_minimum_required(VERSION 3.23)
project( DIP_2 )
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} include)
add_executable( DIP_2 src/lzw2.cpp src/binaryread.cpp src/lzwu.cpp src/quant.cpp)
target_link_libraries( DIP_2 ${OpenCV_LIBS} )

