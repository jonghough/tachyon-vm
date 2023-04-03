cmake_minimum_required(VERSION 2.8)
project (tachyon_vm) 
 
set(SOURCE_DIR  ./src/)
set(INCLUDE_DIR  ./src/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB_RECURSE TACHYON_SRC "${SOURCE_DIR}*.c")

file(GLOB_RECURSE TACHYON_INC "${INCLUDE_DIR}*.h")

message("SRC DIR is located at ${TACHYON_SRC}")
message("INC DIR is located at ${TACHYON_INC}")
 
include_directories(./src/) 
include_directories(/usr/include) 
include_directories(/usr/local/include)  

find_library(ZIP zip)
message("z library is located at ${ZIP}")


add_compile_options(-Wall -Wextra -pedantic -Werror)
add_definitions(-DDEBUG_BUILD=0) 
add_executable(tachyon  ${TACHYON_SRC} ${TACHYON_INC})

target_link_libraries(tachyon PUBLIC ${ZIP} PRIVATE m)



  