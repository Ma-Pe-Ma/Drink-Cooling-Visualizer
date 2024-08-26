cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("DrinkCoolingVisualizer")

# to build emscripten version: emcmake cmake -B./out/build/wasm-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# define executable and add sources
add_executable(DrinkCoolingVisualizer 
	"src/dcv/Drawing.cpp"
	"src/dcv/Main.cpp"
	"src/dcv/Calculating.cpp"
	"src/dcv/Snapshot.cpp"
	"src/dcv/DrinkCooling.cpp"
	"src/dcv/GeometricProperties.cpp"
	"src/dcv/ProcessProperties.cpp"
	"src/dcv/MaterialProperties.cpp"
	"src/dcv/MainLogic.cpp"
	"src/dcv/Windowing.cpp"
	"src/dcv/Drawer.cpp"
	
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
	"src/dcv/"
	"src/submodules/"
	"src/submodules/tinycolormap/include"
	"src/submodules/glm"
	"src/submodules/glfw/include"
	"src/submodules/imgui"
	"src/submodules/implot"
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
		PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets/ \
							   --preload-file ${CMAKE_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf@/assets/Roboto-Medium.ttf \
	")
	
	# add compile options
	target_compile_options(DrinkCoolingVisualizer
		PUBLIC "-pthread;-sUSE_PTHREADS=1;-sPTHREAD_POOL_SIZE=10;-sALLOW_MEMORY_GROWTH=1;"	
	)
else()
	# copy font to resources folder
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/assets/")

	# build glfw library
	add_subdirectory("src/submodules/glfw")

	# link libraries with the app
	target_link_libraries(DrinkCoolingVisualizer PRIVATE
		glfw
	)
endif()

# windows specific settings
if (WIN32)
	file(WRITE "assets/icon.rc" "IDI_ICON1 ICON \"./can.ico\"")

	target_sources(DrinkCoolingVisualizer PRIVATE
		"assets/icon.rc"
	)

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

# cmake --install /path/to/build --prefix /path/to/install --component dcv
if(EMSCRIPTEN)
	set(em_target "DrinkCoolingVisualizer")
	install(TARGETS DrinkCoolingVisualizer COMPONENT dcv RUNTIME DESTINATION ".")
	install(FILES
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.js"
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.wasm"
		"$<TARGET_FILE_DIR:${em_target}>/${em_target}.data"
		COMPONENT dcv
        DESTINATION ".")
elseif(WIN32 OR UNIX)
	install(TARGETS DrinkCoolingVisualizer COMPONENT dcv RUNTIME DESTINATION ".")
	install(DIRECTORY "assets" COMPONENT dcv DESTINATION "." PATTERN "*.rc" EXCLUDE PATTERN "*.ico" EXCLUDE)
	install(FILES "ReadMe.md" "LICENSE.md" COMPONENT dcv DESTINATION ".")
endif()
