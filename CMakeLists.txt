cmake_minimum_required (VERSION 2.8)
project (split_module_files)
enable_language(CXX)
set (CMAKE_CXX_STANDARD 11)

if (MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

add_executable (split_module_files "${CMAKE_SOURCE_DIR}/src/split_module_files.cpp")

target_link_libraries(split_module_files 
  ${Boost_LIBRARIES}
) 

install (TARGETS split_module_files	DESTINATION ${CMAKE_SOURCE_DIR}/bin/)



