cmake_minimum_required (VERSION 3.0)

project(LightBulbChallenge)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(
    -std=c++14
)

add_library(
    ColorUtils
    ColorUtils/ColorUtils.h
    ColorUtils/ColorUtils.cpp
)

include_directories(ColorUtils)

add_executable(
    ColorAnalyser
    ColorAnalyser/main.cpp
    ColorAnalyser/CommandLineParser.h
    ColorAnalyser/CommandLineParser.cpp
)

target_link_libraries(ColorAnalyser ColorUtils)

option(Viewer "Build CIE Diagram Viewer" ON)

if(Viewer)
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
        set(CMAKE_AUTOMOC ON)
	
	find_package(Qt5Widgets)
	find_package(Qt5Charts)

	add_executable(
            Viewer
            WIN32
            MACOSX_BUNDLE
            Viewer/main.cpp
            Viewer/CIEWidget.h
            Viewer/CIEWidget.cpp
            Viewer/ColorEvolutionWidget.h
            Viewer/ColorEvolutionWidget.cpp
            Viewer/Resources.rc
	)
	
	target_link_libraries(Viewer Qt5::Widgets Qt5::Charts ColorUtils)
endif(Viewer)
