cmake_minimum_required (VERSION 3.18)
set(BOOST_PATH "C:/boost/boost_1_79_0/")
set(CMAKE_CXX_STANDARD 17)
set(Boost_INCLUDE_DIR "${BOOST_PATH}") 
set(Boost_LIBRARY_DIR "${BOOST_PATH}stage/lib")
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_ROOT ${BOOST_PATH})
find_package(Boost COMPONENTS thread REQUIRED)
# Добавьте источник в исполняемый файл этого проекта.
include_directories(${BOOST_PATH})
add_executable (Veeam "Veeam.cpp" "Veeam.h" "IOManager.h" "IOManager.cpp" "MD5/MD5.cpp" "Consts.h" "MeasureTime.cpp" "MeasureTime.h" )
#set(VS_DEBUGGER_COMMAND_ARGUMENTS "C:/boost/in.txt" "C:/boost/out.txt")
target_link_libraries(Veeam ${Boost_LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Veeam PROPERTY CXX_STANDARD 20)
endif()
