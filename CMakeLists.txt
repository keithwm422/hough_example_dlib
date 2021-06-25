# This is a CMake makefile.  CMake is a tool that helps you build C++ programs.
# You can download CMake from http://www.cmake.org.  This CMakeLists.txt file
# you are reading builds dlib's example programs. 
#


cmake_minimum_required(VERSION 2.8.12)
# Every project needs a name.  We call this the "examples" project.
project(dlib_cmake_example)
# Tell cmake we will need dlib.  This command will pull in dlib and compile it
# into your project.  Note that you don't need to compile or install dlib.  All
# cmake needs is the dlib source code folder and it will take care of everything.
#include(FetchContent)
#set(BUILD_SHARED_LIBS ON CACHE INTERNAL "build dlib shared library" FORCE)
#message(${BUILD_SHARED_LIBS})
#FetchContent_Declare(dlib
#)
#set(BUILD_SHARED_LIBS 1 CACHE INTERNAL "build dlib shared library" FORCE)
#FetchContent_MakeAvailable(dlib)
include(ExternalProject)
ExternalProject_Add(
      dlib
      GIT_REPOSITORY      https://github.com/davisking/dlib.git
      GIT_TAG             v19.22
      GIT_SHALLOW         ON
      GIT_PROGRESS        ON
      SOURCE_DIR          ${CMAKE_CURRENT_BINARY_DIR}/dlib-src
      BINARY_DIR          ${CMAKE_CURRENT_BINARY_DIR}/dlib-build
      INSTALL_DIR         ${CMAKE_CURRENT_BINARY_DIR}/dlib-install
      UPDATE_COMMAND      ""
      PATCH_COMMAND       ""
      #INSTALL_COMMAND     ""
      CMAKE_ARGS          -DBUILD_SHARED_LIBS=1 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/dlib-install
      #BUILD_COMMAND      ${MAKE}
      #INSTALL_COMMAND     ""
      UPDATE_DISCONNECTED OFF
      #S${CMAKE_CURRENT_SOURCE_DIR}/lib
      
)

# need these paths to match what you pick above for SOURCE_DIR and BINARY_DIR
set(dlib_src ${CMAKE_CURRENT_BINARY_DIR}/dlib-src)
set(dlib_build ${CMAKE_CURRENT_BINARY_DIR}/dlib-build)
set(dlib_install ${CMAKE_CURRENT_BINARY_DIR}/dlib-install)
# configure a library to correspond to what gets built by helixtools
add_library(libdlib SHARED IMPORTED)
add_dependencies(libdlib dlib)
set_target_properties(libdlib PROPERTIES
    IMPORTED_LOCATION ${dlib_install}/lib64/libdlib.so
    INCLUDE_DIRECTORIES ${dlib_install}/include)

#set(BUILD_SHARED_LIBS 1 CACHE INTERNAL "build dlib shared library" FORCE)
# The next thing we need to do is tell CMake about the code you want to
# compile.  We do this with the add_executable() statement which takes the name
# of the output executable and then a list of .cpp files to compile.  Here we
# are going to compile one of the dlib example programs which has only one .cpp
# file, assignment_learning_ex.cpp.  If your program consisted of multiple .cpp
# files you would simply list them here in the add_executable() statement.  
include_directories(${dlib_install}/include)
add_executable(hough_transform_dct_image hough_transform_dct_image.cpp)
add_executable(hough_transform_ex hough_transform_ex.cpp)
add_executable(hough_transform_line_properties hough_transform_line_properties.cpp)
add_executable(image_ex image_ex.cpp)
add_executable(image_dct_ex image_dct_ex.cpp)
#add_dependencies(hough_transform_dct_image dlib)
# Finally, you need to tell CMake that this program, assignment_learning_ex,
# depends on dlib.  You do that with this statement: 
target_link_libraries(hough_transform_dct_image libdlib)
target_link_libraries(hough_transform_ex libdlib)
target_link_libraries(hough_transform_line_properties libdlib)
target_link_libraries(image_ex libdlib)
target_link_libraries(image_dct_ex libdlib)
