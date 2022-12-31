## ByteSmartObjectPool
This repository contains a thread-safe header-only object pool under 'ByteSmartObjectPool/ByteSmartObjectPool.hpp'.<br>For some toy examples look at 'ByteSmartObjectPool/ObjectPoolTests.cpp'.<br>NOTE: The pool's lifetime must exceed that of its objects, otherwise it'll lead to undefined behavior.
#### Some implementation details
The pool items' allocation and deallocation is managed using std::unique_ptr.<br>Under the hood, the pool is implemented using std::array of std::byte, hence<br>there are minimal requirements on template paramter T.<br>The next open slot in the pool is managed using a stack.