add_executable (StackfullObjectPool "StackfullObjectPoolTests.cpp" "StackfullObjectPool.hpp" "catch.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StackfullObjectPool PROPERTY CXX_STANDARD 20)
endif()