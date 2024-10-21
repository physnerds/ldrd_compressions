cmake_minimum_required(VERSION 3.20)

project(compression CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(ROOT REQUIRED)
find_package(HDF5 COMPONENTS CXX C REQUIRED)
find_package(SZ3 REQUIRED)
find_package(mgard REQUIRED)
message("Status of SZ3 ${SZ3_FOUND}")
message("Status of MGARD ${mgard_FOUND}")
set(SZ_INCLUDE_DIRS "" CACHE PATH "Include directory of SZ")
set(SZ_LIBRARIES "" CACHE PATH "lib file of SZ")
set(ZSTD_LIBRARIES "" CACHE PATH "lib file for ZSTD")
set(IDEALEM_INCLUDE_DIRS "" CACHE PATH "Include directory of IDEALEM")
set(IDEALEM_LIBRARIES "" CACHE PATH "lib file for IDEALEM")

message("HDF5 LIBRARIES ${HDF5_LIBRARIES}")
#message("HDF5 CXX LIBRARIES ${HDF5_CXX_LIBRARIES}")
message("SZ_INCLUDE_DIRECTORIES ${SZ_INCLUDE_DIRS}")
message("SZ_LIBRARIES ${SZ_LIBRARIES}")
message("ZSTD_LIBRARIES ${ZSTD_LIBRARIES}")
message("IDEALEM_INCLUDE_DIRECTORIES ${IDEALEM_INCLUDE_DIRS}")
message("IDEALEM_LIBRARIES ${IDEALEM_LIBRARIES}")
include_directories(SYSTEM
                    ${HDF5_INCLUDE_DIRS}
                    ${ROOT_INCLUDE_DIRS}
                    ${SZ_INCLUDE_DIRS}
                    ${MGARD_INCLUDE_DIRS}
                    ${IDEALEM_INCLUDE_DIRS}
                    )

set(libraries 
    ${ROOT_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SZ_LIBRARIES}
    #${MGARD_LIBRARIES}
    ${ZSTD_LIBRARIES}
    ${IDEALEM_LIBRARIES}
    mgard::mgard

    )

add_executable(mytest mytest.cxx)
target_link_libraries(mytest PRIVATE ${libraries} )

add_executable(mytest_sz3 mytest_SZ3.cxx)
target_link_libraries(mytest_sz3 PRIVATE ${libraries} ROOT::ROOTNTuple)

add_executable(decompress_sz3_test decompress_sz3_test.cxx)
target_link_libraries(decompress_sz3_test PRIVATE ${libraries} ROOT::ROOTNTuple)

add_executable(compress_channels_SZ3 compress_channels_SZ3.cxx)
target_link_libraries(compress_channels_SZ3 PRIVATE ${libraries} ROOT::ROOTNTuple)

add_executable(compress_channels_SZ3_2D compress_channels_SZ3_2D.cxx)
target_link_libraries(compress_channels_SZ3_2D PRIVATE ${libraries} ROOT::ROOTNTuple)


