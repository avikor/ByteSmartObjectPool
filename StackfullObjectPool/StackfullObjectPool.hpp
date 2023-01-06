#ifndef STACKFULL_OBJECT_POOL
#define STACKFULL_OBJECT_POOL


#include <array>
#include <memory>
#include <functional>


namespace sop
{
    template <typename T>
    concept PoolItemType = std::is_trivially_copyable_v<T>;


    template <PoolItemType T>
    using PoolItem = std::unique_ptr<T, std::function<void(T*)>>;


    class max_capacity_exception : public std::bad_alloc
    {
    public:
        char const* what() const throw() override
        {
            return "object pool reached max capacity.";
        }
    };


    template <PoolItemType T, std::size_t CAPACITY>
    class StackfullObjectPool
    {
    public:
        StackfullObjectPool() noexcept;

        template <typename... Args>
        [[nodiscard]] PoolItem<T> request(Args&&... args) noexcept(false);

        [[nodiscard]] consteval std::size_t capacity() const noexcept;

        [[nodiscard]] std::size_t size() const noexcept;
        
        [[nodiscard]] bool isFull() const noexcept;

    private:
        std::array<std::byte, sizeof(T) * CAPACITY> pool_;
        std::array<std::size_t, CAPACITY> stack_;
        std::size_t stackTop_;
        std::size_t size_;
        std::mutex mutex_;
    };


    template <PoolItemType T, std::size_t CAPACITY>
    StackfullObjectPool<T, CAPACITY>::StackfullObjectPool() noexcept
        : pool_{}
        , stack_{}
        , stackTop_{ 0U }
        , size_{ 0U }
        , mutex_{}
    {
        for (std::size_t i{ 0U }; i != CAPACITY; ++i)
        {
            stack_[i] = i;
        }
    }

    template <PoolItemType T, std::size_t CAPACITY>
    template <typename... Args>
    PoolItem<T> StackfullObjectPool<T, CAPACITY>::request(Args&&... args) noexcept(false)
    {
        std::lock_guard lock{ mutex_ };

        if (stackTop_ == CAPACITY) [[unlikely]]
        {
            throw max_capacity_exception{};
        }

        ++stackTop_;

        ++size_;

        return { new (&pool_[stack_[stackTop_ - 1U] * sizeof(T)]) T{ std::forward<Args>(args)... }, [this](T* obj)
            {
                // NOTE: The pool's lifetime must exceed that of its objects, 
                // otherwise it'll lead to undefined behavior

                std::lock_guard lock{ mutex_ };
                
                const std::size_t freedObjIdx{ static_cast<std::size_t>(obj - reinterpret_cast<T*>(pool_.data())) };

                --stackTop_;
                stack_[stackTop_] = freedObjIdx;

                --size_;
            }
        };
    }

    template <PoolItemType T, std::size_t CAPACITY>
    consteval std::size_t StackfullObjectPool<T, CAPACITY>::capacity() const noexcept
    {
        return CAPACITY;
    }

    template <PoolItemType T, std::size_t CAPACITY>
    std::size_t StackfullObjectPool<T, CAPACITY>::size() const noexcept
    {
        return size_;
    }

    template <PoolItemType T, std::size_t CAPACITY>
    bool StackfullObjectPool<T, CAPACITY>::isFull() const noexcept
    {
        return size_ == CAPACITY;
    }
}


#endif // !STACKFULL_OBJECT_POOL