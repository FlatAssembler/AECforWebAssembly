# https://discord.com/channels/172018499005317120/172018499005317120/807727805080010804
cmake_minimum_required(VERSION 3.10)
project(AECForWebAssembly VERSION 1.2)

set(CMAKE_CXX_STANDARD 17)

add_executable(aec AECforWebAssembly.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
