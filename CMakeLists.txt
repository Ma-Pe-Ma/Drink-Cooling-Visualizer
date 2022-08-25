cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project("DrinkCoolingVisualizer")

# define executable and add sources
add_executable(DrinkCoolingVisualizer 
	"src/glad.c"
	"src/Drawing.cpp"
	"src/Main.cpp"
	"src/Calculating.cpp"
	"src/Snapshot.cpp"
	"src/DrinkCooling.cpp"
	"src/GeometricProperties.cpp"
	"src/ProcessProperties.cpp"
	"src/MaterialProperties.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"
)

# build glfw library
add_subdirectory("src/submodules/glfw")

# Add include search paths for the application
target_include_directories(DrinkCoolingVisualizer PRIVATE
	"${CMAKE_SOURCE_DIR}/src/"
	"${CMAKE_SOURCE_DIR}/src/submodules/"
	"${CMAKE_SOURCE_DIR}/src/submodules/tinycolormap/include"
	"${CMAKE_SOURCE_DIR}/src/submodules/glm"
	"${CMAKE_SOURCE_DIR}/src/submodules/glfw/include"
	"${CMAKE_SOURCE_DIR}/src/submodules/imgui"
)

# copy font to resources folder
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources/")

# link libraries with the app
target_link_libraries(DrinkCoolingVisualizer PRIVATE
	glfw
)

# windows specific settings
if (WIN32)
	# Release mode
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# Hide Console Window method 1 - not working as cmake appends /SUBSYSTEM:CONSOLE to CMAKE_EXE_LINKER_FLAGS after running current CMakeLists.txt
#		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

		# Hide Console Window method 2 - no effect, the cause is probably is the same as in method 1
#		target_link_options(DrinkCoolingVisualizer
#			PUBLIC "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
#		)

		# Hide Console Window method 3 - working
		set_target_properties(DrinkCoolingVisualizer
			PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)

		# runtime libraries, earlier they were necessary
		#target_link_libraries(DrinkCoolingVisualizer PRIVATE
			#msvcmrt	
			#msvcrt			
		#)

	# Debug mode
	else()
		# show console window
		set_target_properties(DrinkCoolingVisualizer
			PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		)
	endif()
endif()