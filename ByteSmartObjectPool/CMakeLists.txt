# CMakeList.txt : CMake project for ByteSmartObjectPool, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ByteSmartObjectPool "ObjectPoolTests.cpp" "ByteSmartObjectPool.h" "Catch.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ByteSmartObjectPool PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
