cmake_minimum_required(VERSION 2.8)
project(spiffing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

file(GLOB ASN_SOURCE_FILES
        "gen-ber/*.c"
        "gen-ber/*.h"
)
include_directories(gen-ber)
add_library(spiffing-asn ${ASN_SOURCE_FILES})

set(SOURCE_FILES
    src/category.cc
    src/categorydata.cc
    src/categorygroup.cc
    src/catutils.cc
    src/classification.cc
    src/clearance.cc
    src/label.cc
    src/lacv.cc
    src/marking.cc
    src/spif.cc
    src/tag.cc
    src/tagset.cc include/spiffing/spiffing.h src/spiffing.cc src/equivclass.cc include/spiffing/equivclass.h src/equivcat.cc include/spiffing/equivcat.h src/constants.cc include/spiffing//constants.h include/spiffing/markings.h src/markings.cc)
include_directories(include)
include_directories(deps/rapidxml)
add_library(spiffing ${SOURCE_FILES})
add_dependencies(spiffing spiffing-asn)
target_link_libraries(spiffing spiffing-asn)

add_executable(spifflicator spifflicator.cc)
add_dependencies(spifflicator spiffing)
target_link_libraries(spifflicator spiffing spiffing-asn)

add_executable(test-spiffing test.cc)
add_dependencies(test-spiffing spiffing)
target_link_libraries(test-spiffing spiffing spiffing-asn)

add_executable(transpifferizer transpifferizer.cc)
target_link_libraries(transpifferizer spiffing spiffing-asn)
