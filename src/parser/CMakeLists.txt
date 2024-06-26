# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

set(target ${PROJ_LIBRARY}_parser)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
# set(ANTLR4_TAG 4.13.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.13.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/bin/antlr-4.13.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(AlgebraicExpressionLexer ${CMAKE_CURRENT_SOURCE_DIR}/grammar/AlgebraicExpressionLexer.g4 LEXER
        PACKAGE ANTLRAlgebraicExpressionParser)
antlr_target(AlgebraicExpressionParser ${CMAKE_CURRENT_SOURCE_DIR}/grammar/AlgebraicExpressionParser.g4 PARSER
        VISITOR LISTENER
        PACKAGE ANTLRAlgebraicExpressionParser
        DEPENDS_ANTLR AlgebraicExpressionLexer
        COMPILE_FLAGS -lib ${ANTLR_AlgebraicExpressionLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_AlgebraicExpressionLexer_OUTPUT_DIR})
include_directories(${ANTLR_AlgebraicExpressionParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_library(${target} STATIC parser.cpp
        ${ANTLR_AlgebraicExpressionLexer_CXX_OUTPUTS}
        ${ANTLR_AlgebraicExpressionParser_CXX_OUTPUTS})
target_link_libraries(${target} antlr4_static)