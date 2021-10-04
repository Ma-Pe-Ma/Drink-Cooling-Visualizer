# Drink Cooling Visualizer
A simple visualizer and numerical solver for temperature distribution inside a can of drink during a cooling process (written in C++ for Windows).
The app uses a simple model, only the conduction inside the drink ([Fourier's law](https://en.wikipedia.org/wiki/Thermal_conduction#Fourier's_law)) and the heat transfer ([Newton's law of coooling](https://en.wikipedia.org/wiki/Newton%27s_law_of_cooling)) at the boundary is modelled, the can is neglected, and the environment temperature is constant. The physics of the calculations are explained [here](https://studylib.net/doc/11409665/project-no.-2.-cooling-of-beer-1-introduction-may-8--2003) but I modified much of it for my own likings.

![](./Assets/DrinkCoolingVisualizer.png)

## Manual
You can create as many processes as you want, this is useful if you want to compare different configurations.

There are two type of processes: 
- with the first type you can calculate how much time is needed to cool down the drink to a target temperature
- with the second type you can determine the average temperature of the drink after a given amount of time has ellapsed

There are four type of parameters which have to be specified:
* can's geometric properties - height and radius and their division factors,  the radiuses are not divided equally but they are diveded in a way that the generated volumes are equal.
* drink's material properties - self-explanatory (the default values are the water's parameters)
* temperature properties - self explanatory
* time properties: the time-step is the time-discretization value, while the snapshot value is the time difference between two stored states (not all states are stored as they would need much memory and at low time step values technically no differences can be seen between to neighbouring states)

You can manipulate the camera at any time. The camare can move in a cylindrical coordinate system and it always looks at the can/origo.

After setting up the process, press start, and the calculation begins. The length of the calculation depends on the geometry/time discretization and on the temperature conditions, naturally. The default process should only take a moment to finish.

When post-processing, you can set the ellapsed time of the process and the heat distribution at the specified time is visualized. A heat map is also generated, only the viridis color map can be used.

To use it you have to install Visual Studio [redistributables](https://aka.ms/vs/16/release/vc_redist.x64.exe).
## Compilation help
Only a few dependencies are needed to be added:
* [GLFW](https://www.glfw.org/) - utility used to create OpenGL contexts, and windows
* [GLAD](https://glad.dav1d.de/) - an OpenGL Loading Library
* [glm](https://github.com/g-truc/glm) - a maths library for graphics
* [Dear ImGui](https://github.com/ocornut/imgui) - an immediate mode GUI library
* [Tiny Color Map](https://github.com/yuki-koyama/tinycolormap) - a simple header-only library used to create maps 
* [this shader class](https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h) - used to handle shaders easily

A fully configured Visual Studio project is included. You can add the path to your include+library directories in the CustomPaths.props property sheet or add them manually.

## Developer notes
This is a simple hobby project which should not be taken seriously. I do not plan to improve the functionality of this application. The weakest part of it is the essence of the app: the numerical solver. I think some parameters are not okay and there may be problems with the solver too. If I will be in the mood I will try to improve it in the future...