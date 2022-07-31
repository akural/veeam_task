# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

cmake_minimum_required (VERSION 3.8)
project ("Veeam" VERSION 0.1 LANGUAGES CXX)
#set(CMAKE_CXX_STANDARD 20)
#set(Boost_INCLUDE_DIR "C:/boost/boost_1_78_0/") 
#set(Boost_LIBRARY_DIR "C:/boost/boost_1_78_0/lib64-msvc-14.2")
#set(Boost_USE_STATIC_LIBS ON)
#set( BOOST_ROOT  "C:/boost/boost_1_78_0")
#PRINT_ALL_VARIABLES()
#find_package(Boost COMPONENTS thread REQUIRED)

#target_link_libraries(Veeam ${Boost_LIBRARIES})
# Включите подпроекты.
add_subdirectory ("Veeam")

