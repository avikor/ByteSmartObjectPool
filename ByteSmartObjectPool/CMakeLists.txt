add_executable (ByteSmartObjectPool "ObjectPoolTests.cpp" "ByteSmartObjectPool.h" "catch.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ByteSmartObjectPool PROPERTY CXX_STANDARD 20)
endif()