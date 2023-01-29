#ifndef STACKFULL_OBJECT_POOL
#define STACKFULL_OBJECT_POOL


#include <array>
#include <memory>
#include <mutex>


namespace sop
{
    template <typename T>
    concept PoolItemConcept = std::is_trivially_copyable_v<T>;


    template <PoolItemConcept T, std::size_t CAPACITY>
    class StackfullObjectPool;

    template <PoolItemConcept T, std::size_t CAPACITY>
    class PoolItemDeleter
    {
    public:
        constexpr PoolItemDeleter(StackfullObjectPool<T, CAPACITY>& objectPool)
            : objectPool_{ objectPool }
        { }

        constexpr void operator()(T* obj) const
        {
            objectPool_.release(obj);
        }

    private:
        StackfullObjectPool<T, CAPACITY>& objectPool_;
    };

    template <PoolItemConcept T, std::size_t CAPACITY>
    using PoolItem = std::unique_ptr<T, const PoolItemDeleter<T, CAPACITY>&>;


    class max_capacity_exception : public std::bad_alloc
    {
    public:
        char const* what() const throw() override
        {
            return "object pool reached max capacity.";
        }
    };


    template <PoolItemConcept T, std::size_t CAPACITY>
    class StackfullObjectPool
    {
    public:
        constexpr StackfullObjectPool() noexcept;

        template <typename... Args>
        [[nodiscard]] constexpr PoolItem<T, CAPACITY> request(Args&&... args) noexcept(false);

        [[nodiscard]] consteval std::size_t capacity() const noexcept;

        [[nodiscard]] constexpr std::size_t size() const noexcept;
        
        [[nodiscard]] constexpr bool isFull() const noexcept;

    private:
        friend class PoolItemDeleter<T, CAPACITY>;

        std::array<std::byte, sizeof(T) * CAPACITY> pool_;
        T* const poolStart_;
        std::array<std::size_t, CAPACITY> stack_;
        std::size_t stackTop_;
        std::size_t size_;
        std::mutex mutex_;
        const PoolItemDeleter<T, CAPACITY> poolItemDeleter_;

        constexpr void release(T* obj) noexcept;
    };


    template <PoolItemConcept T, std::size_t CAPACITY>
    constexpr StackfullObjectPool<T, CAPACITY>::StackfullObjectPool() noexcept
        : pool_{}
        , poolStart_{ reinterpret_cast<T* const>(pool_.data()) }
        , stack_{}
        , stackTop_{ 0U }
        , size_{ 0U }
        , mutex_{}
        , poolItemDeleter_{ *this }
    {
        for (std::size_t i{ 0U }; i != CAPACITY; ++i)
        {
            stack_[i] = i;
        }
    }

    template <PoolItemConcept T, std::size_t CAPACITY>
    template <typename... Args>
    constexpr PoolItem<T, CAPACITY> StackfullObjectPool<T, CAPACITY>::request(Args&&... args) noexcept(false)
    {
        std::lock_guard lock{ mutex_ };

        if (stackTop_ == CAPACITY) [[unlikely]]
        {
            throw max_capacity_exception{};
        }

        ++stackTop_;

        ++size_;

        return { new (&pool_[stack_[stackTop_ - 1U] * sizeof(T)]) T{ std::forward<Args>(args)... }, poolItemDeleter_ };
    }

    template <PoolItemConcept T, std::size_t CAPACITY>
    constexpr void StackfullObjectPool<T, CAPACITY>::release(T* obj) noexcept
    {
        // NOTE: The pool's lifetime must exceed that of its objects, 
        // otherwise it'll lead to undefined behavior

        std::lock_guard lock{ mutex_ };

        const std::size_t freedObjIdx{ static_cast<std::size_t>(obj - poolStart_) };

        --stackTop_;
        stack_[stackTop_] = freedObjIdx;

        --size_;
    }

    template <PoolItemConcept T, std::size_t CAPACITY>
    consteval std::size_t StackfullObjectPool<T, CAPACITY>::capacity() const noexcept
    {
        return CAPACITY;
    }

    template <PoolItemConcept T, std::size_t CAPACITY>
    constexpr std::size_t StackfullObjectPool<T, CAPACITY>::size() const noexcept
    {
        return size_;
    }

    template <PoolItemConcept T, std::size_t CAPACITY>
    constexpr bool StackfullObjectPool<T, CAPACITY>::isFull() const noexcept
    {
        return size_ == CAPACITY;
    }
}


#endif // !STACKFULL_OBJECT_POOL