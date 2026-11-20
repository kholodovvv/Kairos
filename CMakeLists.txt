cmake_minimum_required(VERSION 3.16)

project(Kairos LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    add_compile_definitions(UNICODE)
    add_compile_definitions(_UNICODE)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

find_package(Qt5 COMPONENTS Core Network Gui Widgets REQUIRED)
find_package(Boost COMPONENTS asio REQUIRED)

set(RESOURCES
    resources/resources.qrc
    )

 qt5_add_resources(RESOURCES ${RESOURCES})

add_executable(${PROJECT_NAME}
  sources/main.cpp
  sources/mainwindow.cpp
  sources/network.cpp
  sources/logger.cpp
  sources/core.cpp
  include/mainwindow.hpp
  include/network.hpp
  include/logger.hpp
  include/core.hpp
  ${RESOURCES}
  )

target_link_libraries(${PROJECT_NAME}
 Qt5::Core
 Qt5::Network
 Qt5::Gui
 Qt5::Widgets
 Boost::asio
)