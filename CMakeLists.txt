# https://discord.com/channels/172018499005317120/172018499005317120/807727805080010804
cmake_minimum_required(VERSION 3.10)
project(AECForWebAssembly VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(aec AECforWebAssembly.cpp)

enable_testing()
add_test(NAME structureDeclarationTestCompiles
	      COMMAND aec structureDeclarationTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME structureDeclarationTestAssembles
	      COMMAND npx -q -p wabt wat2wasm structureDeclarationTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME structureDeclarationTestRuns
	      COMMAND node structureDeclarationTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
