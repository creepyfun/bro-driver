cmake_minimum_required(VERSION 2.8)

project(bro-driver)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(${PROJECT_NAME} "main.cpp" "tools.cpp" "EveWindow.cpp" "EveIngameWindow.cpp" "EveLocalChannel.cpp" "ImageLibrary.cpp" "ShipModule.cpp" "Spaceship.cpp" "EveOverview.cpp" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
	
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
target_link_librarieS(${PROJECT_NAME} Qt5::Widgets)
