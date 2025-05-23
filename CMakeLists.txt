# https://discord.com/channels/172018499005317120/172018499005317120/807727805080010804
cmake_minimum_required(VERSION 3.10)
project(AECForWebAssembly VERSION 3.2.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(aec AECforWebAssembly.cpp)

enable_testing()
# TODO: Some tests are dependent on others having been already run, I do not
# know enough about CMAKE to denote that.
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
add_test(NAME namedArgumentsTestCompiles
	      COMMAND aec namedArgumentsTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME namedArgumentsTestAssembles
	      COMMAND npx -q -p wabt wat2wasm namedArgumentsTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME namedArgumentsTestRuns
	      COMMAND node namedArgumentsTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME leftHandSideConditionalTestCompiles
	      COMMAND aec leftHandSideConditionalTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME leftHandSideConditionalTestAssembles
	      COMMAND npx -q -p wabt wat2wasm leftHandSideConditionalTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME leftHandSideConditionalTestRuns
	      COMMAND node leftHandSideConditionalTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME multiLineStringTestCompiles
	      COMMAND aec multiLineStringTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME multiLineStringTestAssembles
	      COMMAND npx -q -p wabt wat2wasm multiLineStringTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME multiLineStringTestRuns
	      COMMAND node multiLineStringTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME chainedComparisonsTestCompiles
	      COMMAND aec chainedComparisonsTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME chainedComparisonsTestAssembles
	      COMMAND npx -q -p wabt wat2wasm chainedComparisonsTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME chainedComparisonsTestRuns
	      COMMAND node chainedComparisonsTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME typeOfTestCompiles
	      COMMAND aec typeOfTest.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME typeOfTestAssembles
	      COMMAND npx -q -p wabt wat2wasm typeOfTest.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME typeOfTestRuns
	      COMMAND node typeOfTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME HuffmanCodingCompiles
	      COMMAND aec HuffmanCodingInAEC/HuffmanCodingInAEC.aec
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME HuffmanCodingAssembles
	      COMMAND npx -q -p wabt wat2wasm --output HuffmanCodingInAEC/HuffmanCodingInAEC.wasm HuffmanCodingInAEC/HuffmanCodingInAEC.wat
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(NAME HuffmanCodingRuns
	      COMMAND node automaticTest.js
	      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/HuffmanCodingInAEC)
