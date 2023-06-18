cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project("DrinkCoolingVisualizer")

# to build emscripten version: emcmake cmake -B./out/build/wasm -G "Ninja"

# define executable and add sources
add_executable(DrinkCoolingVisualizer 
	"src/Drawing.cpp"
	"src/Main.cpp"
	"src/Calculating.cpp"
	"src/Snapshot.cpp"
	"src/DrinkCooling.cpp"
	"src/GeometricProperties.cpp"
	"src/ProcessProperties.cpp"
	"src/MaterialProperties.cpp"
	"src/MainLogic.cpp"
	"src/Windowing.cpp"
	"src/Drawer.cpp"
	
	"src/submodules/glad.c"
	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"
	"src/submodules/implot/implot.cpp"
	"src/submodules/implot/implot_items.cpp"
)

# Add include search paths for the application
target_include_directories(DrinkCoolingVisualizer PRIVATE
	"${CMAKE_SOURCE_DIR}/src/"
	"${CMAKE_SOURCE_DIR}/src/submodules/"
	"${CMAKE_SOURCE_DIR}/src/submodules/tinycolormap/include"
	"${CMAKE_SOURCE_DIR}/src/submodules/glm"
	"${CMAKE_SOURCE_DIR}/src/submodules/glfw/include"
	"${CMAKE_SOURCE_DIR}/src/submodules/imgui"
	"${CMAKE_SOURCE_DIR}/src/submodules/implot"
)

if(EMSCRIPTEN)
	# generate html file
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	
	# configure opengl context + pthreads link options
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sLLD_REPORT_UNDEFINED\
		-s USE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 \
		-pthread -s USE_PTHREADS=1 -sPTHREAD_POOL_SIZE=6 -sALLOW_MEMORY_GROWTH"
	)

	# add resource files to build
	set_target_properties(DrinkCoolingVisualizer
		PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/resources@/resources/ \
							   --preload-file ${CMAKE_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf@/resources/Roboto-Medium.ttf \
	")
	
	# add compile options
	target_compile_options(DrinkCoolingVisualizer
		PUBLIC "-pthread;-sUSE_PTHREADS=1;-sPTHREAD_POOL_SIZE=10;-sALLOW_MEMORY_GROWTH=1;"	
	)
else()
	# copy font to resources folder
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources/")

	# build glfw library
	add_subdirectory("src/submodules/glfw")

	# link libraries with the app
	target_link_libraries(DrinkCoolingVisualizer PRIVATE
		glfw
	)
endif()

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