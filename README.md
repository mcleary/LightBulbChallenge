# Thalmic Labs Light Bulb Challenge

## Requirements
- A **C++14** compiler
- **CMake** 3.0+

## Optional
- **Qt** to display the GUI
- An **OpenMP** compatible compiler

## Introduction and Context
This is an implementation of the Thalmic Labs C++ Challenge for the 2016 VanHackaton.

The challenge consists in analyzing the data from various spectrometers readings from a company that is developing a new light bulb technology. The company wants to know how stable is each light bulb over time.

The first part of the challenge consists in writing a program that calculates the perceived color of the light as a (x, y) coordinates on CIE-1931 chromaticity diagram.

The second part consists in determining how stable is the perceived color over time.

## Color analyser command line tool
To calculate the perceived color as (x, y) coordinates on a CIE-1931 diagram, first we need to calculate the tristimulus values for a color response read with the spectrometer at each timestamp.

To perform this step I created a command line tool to process the data.

This tool can be used like the following:

    ColorAnalyser
        --intensities [CSV Filepath]
        --wavelengths [CSV Filepath]
        --color-matching [CSV Filepath]
        {--stats}

- `--intensites` is the file path for the intensities read by the spectrometer
- `--wavelengths` is the file path for the wavelengths values that the spectrometer is sensible.
- `--color-matching` is the file for the color matching function [defined by the CIE Commision](http://cvrl.ioo.ucl.ac.uk/cmfs.htm).
- `--stats` is an optional parameter to print the color statistics at the end. The statistics includes *averages*, *abosule and relative standard deviations* and *distante to pure white*. This statistics are used to determine how stable is a light bulb over time.

### Features
- **Noise removal**: Noise is expected in the data provided by the spectrometer. To reduce the impact of noise in the result a **moving average filter** is used.
- **Parallel Execution**: Due to the embarrassingly parallel nature of the calculations, the tool is able to use multiple CPUs to reduce the processing time.
- **Stats calculation**: If desired, the tool can calculate useful color statistic that can be used to determine how stable a series of colors behave over time.
- **Independence**: The only true requirement to build the tool is a C++14 compiler. A CMake configuration is provided but it is not really necessary. There is no third party library required.

### Results
    Dataset \a\:
        Average: (x,y) = (0.33569, 0.335381)
        Std Dev: (x,y) = (1.32136e-7, 1.02374e-7)
        Distance to white: 0.00312155

    Dataset \b\:
        Average: (x,y) = (0.333294, 0.333224)
        Std Dev: (x,y) = (1.01294e-12, 4.36739e-12)
        Distance to white: 0.000116089

    Dataset \b\:
        Average: (x,y) = (0.333274, 0.333182)
        Std Dev: (x,y) = (3.17344e-12, 1.25834e-11)
        Distance to white: 0.000162174

Light bulbs b and c have a very small standard deviation, which means that the color variation over time is smaller compared to light bulb a. Also, bulb b has a slightly smaller standard deviation than bulb c, which should represent a good choice for bulb technology. Moreover, in the GUI application the user can see the RGB representation of the spectrometer samples over time. It is noticeable that the bulb a has more abrupt changes in the color one can see with a naked eye.

**Obs**: The *Distance to white* represents the distance from the *Average* color to the true white point in the CIE-1931 diagram `(x,y) = (1/3, 1/3)`.

## GUI Application

Despite not being required, I decided to write a GUI application to give to the user a visual interpretation of the data being analyzed. The GUI application was written with **Qt** and C++.

### Features
- **Interactive charts**: Qt provides interactive charts components. The user can zoom in and out and select a region of interest.
- **Non-blocking interface**: The GUI makes uses of the ColorAnalyser command line tool to process the data, so the interface is never blocked and multiple datasets can be processed in parallel.
- **Color evolution**: Below the charts there is a series of rectangles where the user can see the sRGB representation of each timestamp. The color variation if very subtle, but they are particularly visible for dataset `\a\`.
- **Statistics**: The same statistics the command line tool calculates are displayed with graphical labels in the Viewer application.
- **Dark theme**: Dark themes are everywhere!
- **CIE-1931 plot**: The charts displays the CIE xyY color space. Each circle represents a timestamp in the chromaticity diagram. The user can se how the color is varying with this kind of chart.

![](https://raw.githubusercontent.com/mcleary/LightBulbChallenge/master/Screenshots/Viewer1.png)
