cmake_minimum_required(VERSION 3.20)

project(compression CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(ROOT REQUIRED)
find_package(HDF5 COMPONENTS CXX C REQUIRED)
find_package(SZ3 REQUIRED)
message("Status of SZ3 ${SZ3_FOUND}")

set(SZ_INCLUDE_DIRS "" CACHE PATH "Include directory of SZ")
set(SZ_LIBRARIES "" CACHE PATH "lib file of SZ")
set(ZSTD_LIBRARIES "" CACHE PATH "lib file for ZSTD")

message("HDF5 LIBRARIES ${HDF5_LIBRARIES}")
#message("HDF5 CXX LIBRARIES ${HDF5_CXX_LIBRARIES}")
message("SZ_INCLUDE_DIRECTORIES ${SZ_INCLUDE_DIRS}")
message("SZ_LIBRARIES ${SZ_LIBRARIES}")
message("ZSTD_LIBRARIES ${ZSTD_LIBRARIES}")
include_directories(SYSTEM
                    ${HDF5_INCLUDE_DIRS}
                    ${ROOT_INCLUDE_DIRS}
                    ${SZ_INCLUDE_DIRS}
                    )

set(libraries 
    ${ROOT_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SZ_LIBRARIES}
    ${ZSTD_LIBRARIES}

    )

add_executable(mytest mytest.cxx)
target_link_libraries(mytest PRIVATE ${libraries} )
