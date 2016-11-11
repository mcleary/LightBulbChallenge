cmake_minimum_required (VERSION 3.0)

project(LightBulbChallenge)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
    LightBulbChallenge
    main.cpp	
	CommandLineParser.cpp
	CommandLineParser.h
)
