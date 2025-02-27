cmake_minimum_required(VERSION 3.30)
project(Client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)

add_executable(Client src/main.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(${PROJECT_SOURCE_DIR}/include)

set(FFMPEG_LIBS_DIR E:/ffmpeg-master-latest-win64-gpl-shared/lib)
set(FFMPEG_HEADERS_DIR E:/ffmpeg-master-latest-win64-gpl-shared/include)
include_directories(${FFMPEG_HEADERS_DIR})
link_directories(${FFMPEG_LIBS_DIR})
set(FFMPEG_LIBS
        "${FFMPEG_LIBS_DIR}/avcodec.lib"
        "${FFMPEG_LIBS_DIR}/avformat.lib"
        "${FFMPEG_LIBS_DIR}/avutil.lib"
        "${FFMPEG_LIBS_DIR}/avdevice.lib"
        "${FFMPEG_LIBS_DIR}/swscale.lib"
)

target_link_libraries(Client ${FFMPEG_LIBS})
