# CMakeList.txt : CMake project for test_xpath_xalan, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TestXalanDom")

# Add source to this project's executable.
add_executable (TestXalanDom "TestXalanDom.cpp" "TestXalanDom.h")

# Set include paths
include_directories(
    #On Windows, xerces config header is saved in cmake cache
    #This line is required to include the config
    ${CMAKE_BINARY_DIR}/lib/xerces-c-3.2.2/src
    ${CMAKE_SOURCE_DIR}/lib/xerces-c-3.2.2/src
    ${CMAKE_SOURCE_DIR}/lib/xalan-c-1.12.0/src
)

target_link_libraries(${PROJECT_NAME} xerces-c xalan-c)
