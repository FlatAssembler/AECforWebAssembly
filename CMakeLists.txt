# https://discord.com/channels/172018499005317120/172018499005317120/807727805080010804
cmake_minimum_required(VERSION 3.10)
project(AECForWebAssembly VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(aec AECforWebAssembly.cpp)

enable_testing()
# TODO: Some tests are dependent on others being already run, I do not know
# enough about CMAKE to denote that.
add_test(NAME globalVariablesDeclarationTestCompiles
	      COMMAND aec globalVariablesDeclarationTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME globalVariablesDeclarationTestAssembles
	      COMMAND npx -q -p wabt wat2wasm globalVariablesDeclarationTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# TODO: Maybe we can add some test whether global variables are dealt with
# correctly, rather than whether the test just compiles and assembles.
# Of course, it is unlikely the "structureDeclarationTestRuns" would pass
# if that is not the case.
add_test(NAME emptyFunctionTestCompiles
	      COMMAND aec emptyFunctionTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME emptyFunctionTestAssembles
	      COMMAND npx -q -p wabt wat2wasm emptyFunctionTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# TODO: There is a small chance empty functions compile and assemble, but
# improperly. Maybe we should check for that somehow.
add_test(NAME structureDeclarationTestCompiles
	      COMMAND aec structureDeclarationTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME structureDeclarationTestAssembles
	      COMMAND npx -q -p wabt wat2wasm structureDeclarationTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME structureDeclarationTestRuns
	      COMMAND node structureDeclarationTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# TODO: Test coverage is far from complete. For example, the if-branching is
# not being tested at all.
