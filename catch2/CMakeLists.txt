add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_features(catch2 INTERFACE cxx_std_17)