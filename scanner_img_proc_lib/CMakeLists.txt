cmake_minimum_required(VERSION 3.15)
project(scanner_img_proc)

set(CMAKE_CXX_STANDARD 17)

#find OpenCV
find_package(OpenCV REQUIRED)

add_library(scanner_img_proc STATIC
	"src/scanner_img_proc.cpp"
)

include_directories(
	src
	${OpenCV_INCLUDE_DIRS}
)

add_executable(scanner_img_proc_Test "src/test.cpp" "src/test.h")

target_link_libraries(
	scanner_img_proc_Test
	scanner_img_proc
	${OpenCV_LIBS}
)

target_link_libraries(
	scanner_img_proc
	${OpenCV_LIBS}
)