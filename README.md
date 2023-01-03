
## StackfullObjectPool
This repository contains a thread-safe header-only stackfull object pool under 'StackfullObjectPool/StackfullObjectPool.hpp'.<br>For some toy examples look at 'StackfullObjectPool/StackfullObjectPoolTests.cpp'.<br>NOTE #1: The pool is meant to store only [Trivially Copyable types](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable), and it is enforced with a concept.<br>NOTE #2: The pool's lifetime must exceed that of its objects, otherwise it'll lead to undefined behavior.
#### Some implementation details
The pool items' allocation and deallocation is managed using std::unique_ptr.<br>Under the hood, the pool is implemented using std::array of std::byte.<br>The next open slot in the pool is managed using a stack.