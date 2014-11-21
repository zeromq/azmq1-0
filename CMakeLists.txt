cmake_minimum_required(VERSION 2.8)
project(aziomq)

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
endif()

if (${CMAKE_CXX_PLATFORM_ID} STREQUAL "Windows")
    # Build for Windows Vista / Windows Server 2008 and later
    add_definitions(-D_WIN32_WINNT=0x0600)

    add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN -D_WINSOCK_DEPRECATED_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    if (NOT DEFINED Boost_USE_STATIC_LIBS)
        set(Boost_USE_STATIC_LIBS ON) # On Windows, Boost is compiled as static libraries by default
    endif()
endif()

if(USE_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc++abi")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc++abi")
    set(ADDITIONAL_LIBS "pthread")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/config")

find_package(Boost 1.53 COMPONENTS system log unit_test_framework date_time thread chrono REQUIRED)
find_package(ZeroMQ 4.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS}
                    ${ZeroMQ_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR})

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
#add_subdirectory(doc/examples)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/aziomq
        DESTINATION include)
