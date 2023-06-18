# Drink Cooling Visualizer

![status](https://badgen.net/badge/status/finished/green) ![license](https://badgen.net/github/license/Ma-Pe-Ma/Drink-Cooling-Visualizer)

![browser](https://badgen.net/badge/browser/working/green) ![desktop](https://badgen.net/badge/desktop/working/green) 

A simple visualizer and numerical solver for determining the temperature distribution inside a can of drink during a cooling process.

The application was written in C++ for browsers and desktop (Windows/Linux). It uses a simple physical model, only the conduction inside the drink ([Fourier's law](https://en.wikipedia.org/wiki/Thermal_conduction#Fourier's_law)) and the heat transfer ([Newton's law of coooling](https://en.wikipedia.org/wiki/Newton%27s_law_of_cooling)) at the boundary is modelled, the can is neglected, and the environment temperature is constant.

The physics of the calculations are explained [here](https://studylib.net/doc/11409665/project-no.-2.-cooling-of-beer-1-introduction-may-8--2003) but I modified much of it for my own likings.

## Browser port

**The app can be used in browser [here](https://mapema.hu/en/hobby/drink-cooling-visualizer/application/)!**

## Video demonstration

[![video demonstration](https://img.youtube.com/vi/BLWBwTyDCcw/0.jpg)](https://www.youtube.com/watch?v=BLWBwTyDCcw)

## Manual

### Process Types

You can create as many processes as you want, this is useful if you want to compare different configurations.

There are two type of processes: 
- with the first type you can calculate how much time is needed to cool down the drink to a target temperature
- with the second type you can determine the average temperature of the drink after a given amount of time has elapsed

### Parameters

There are four type of parameters which have to be specified:
* can's geometric properties - height and radius and their division factors,  the radiuses are not divided equally but they are diveded in a way that the generated volumes are equal.
* drink's material properties - self-explanatory (the default values are the water's parameters)
* temperature properties - self explanatory
* time properties: the time-step is the time-discretization value, while the snapshot value is the time difference between two stored states (Note: not all states are stored as they would need much memory and at low time-step values technically no differences can be seen between to neighbouring states)

### Using the app 

You can manipulate the camera at any time. The camare can move in a cylindrical coordinate system and it always looks at the can/origo.

After setting up the process, press start and the calculation begins. The length of the calculation depends on the geometry/time discretization and on the temperature conditions, obviously. The default process should only take a moment to finish.

When post-processing, you can set the elapsed time of the selected process and the heat distribution at the specified time is visualized then. Also a [viridis](https://cran.r-project.org/web/packages/viridis/vignettes/intro-to-viridis.html) heat map is generated which helps you to read the required temperature values.

The temperature-time distribution of the process is also shown to the user as a plot.

### How to run it

Try the [browser port](#browser-port) or you can download the application from the release section. On Windows you have to install Visual Studio [redistributables](https://aka.ms/vs/16/release/vc_redist.x64.exe) first.

## Compilation help

Clone the project with its submodules:

	git clone --recurse-submodules -j8 https://github.com/Ma-Pe-Ma/Drink-Cooling-Visualizer.git

A CMake project is added to the repository which should configure and then build without problems.

The app requires the following dependecies (which do not need further configuring):
* [GLFW](https://www.glfw.org/) - utility used to create OpenGL contexts, and windows
* [glm](https://github.com/g-truc/glm) - a maths library for graphics
* [Dear ImGui](https://github.com/ocornut/imgui) - an immediate mode GUI library
* [ImPlot](https://github.com/epezent/implot) - an immediate mode plotting library for Dear ImGui
* [Tiny Color Map](https://github.com/yuki-koyama/tinycolormap) - a simple header-only library used to create color maps 
* [GLAD](https://glad.dav1d.de/) - an OpenGL Loading Library

The app should also compile without problems to WebAssemby build with Emscripten.

## Developer notes
This is a simple hobby project which should not be taken seriously. I do not plan to improve the functionality of this application.

The weakest part of it is the essence of the app: the numerical solver. I think some parameters are not okay and there may be problems with the solver too. If I will be in the mood then I will try to improve it in the future...