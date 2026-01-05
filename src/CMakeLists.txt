cmake_minimum_required(VERSION 3.10)

project(eternityii)

add_subdirectory(eii-check)
add_subdirectory(eii-print)

install(TARGETS eii-check DESTINATION .)
install(TARGETS eii-print DESTINATION .)
