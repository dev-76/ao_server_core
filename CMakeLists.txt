cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_COMPILER /opt/gcc/7.3.0/bin/gcc)
SET(CMAKE_CXX_COMPILER /opt/gcc/7.3.0/bin/g++)

SET(CMAKE_C_FLAGS "-ldl -lpthread -std=c++11")
SET(CMAKE_CXX_FLAGS "-ldl -lpthread -std=c++11")

project(ao_server_core)	

FILE(GLOB_RECURSE AO_CORE_HEADERS src/*.h)

FILE(GLOB_RECURSE AO_CORE_SOURCES src/*.cpp)

add_executable(ao_server_core ${AO_CORE_HEADERS} ${AO_CORE_SOURCES})
include_directories(/opt/boost_1_66_0 /opt/mysql-connector-c/6.1.11-gcc-7.3.0/include)
target_link_libraries(ao_server_core /opt/mysql-connector-c/6.1.11-gcc-7.3.0/lib/libmysqlclient.a)
target_link_libraries(ao_server_core /opt/boost_1_66_0/lib/libboost_system.a)
target_link_libraries(ao_server_core /opt/boost_1_66_0/lib/libboost_date_time.a)

