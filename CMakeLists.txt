cmake_minimum_required (VERSION 3.0)

project(LightBulbChallenge)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
    LightBulbChallenge
    main.cpp
	CommandLineParser.cpp
	CommandLineParser.h
)

option(Viewer "Build CIE Diagram Viewer" ON)

if(Viewer)
	SET(CMAKE_INCLUDE_CURRENT_DIR ON)
	SET(CMAKE_AUTOMOC ON)
	
	FIND_PACKAGE(Qt5Widgets)
	find_package(Qt5Charts)

	add_executable(
		Viewer
		WIN32
		Viewer/main.cpp
		Viewer/CIEWidget.h
		Viewer/CIEWidget.cpp
		Viewer/ColorEvolutionWidget.h
		Viewer/ColorEvolutionWidget.cpp
	)
	
	TARGET_LINK_LIBRARIES(Viewer Qt5::Widgets Qt5::Charts)
endif(Viewer)
