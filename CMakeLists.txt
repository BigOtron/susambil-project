cmake_minimum_required(VERSION 3.16)

project(SystemMonitor)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()
file(GLOB_RECURSE SOURCES
	src/core/*.cpp
	src/gui/*.cpp
	src/utils/*.cpp

)
add_executable(SystemMonitor
    main.cpp
  	${SOURCES}
)

target_include_directories(SystemMonitor PRIVATE src)

target_link_libraries(SystemMonitor PRIVATE Qt6::Widgets)
