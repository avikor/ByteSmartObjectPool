#ifndef BYTE_SMART_OBJECT_POOL
#define BYTE_SMART_OBJECT_POOL


#include <array>
#include <memory>
#include <functional>


namespace BSPool
{
    template <typename T>
    using PoolItem = std::unique_ptr<T, std::function<void(T*)>>;


    class max_capacity_exception : public std::bad_alloc
    {
    public:
        char const* what() const override
        {
            return "object pool reached max capacity.";
        }
    };


    template <typename T, std::size_t CAPACITY>
    class ByteSmartObjectPool
    {
    public:
        ByteSmartObjectPool() noexcept;

        ~ByteSmartObjectPool() noexcept;

        template <typename... Args>
        [[nodiscard]] PoolItem<T> request(Args&&... args) noexcept(false);

        [[nodiscard]] consteval std::size_t capacity() const noexcept;

        [[nodiscard]] std::size_t size() const noexcept;
        
        [[nodiscard]] bool isFull() const noexcept;

    private:
        static constexpr std::size_t s_infinity{ std::numeric_limits<std::size_t>::max() }; // signifies a wrap around

        std::array<std::byte, sizeof(T) * CAPACITY> m_pool;
        std::array<std::size_t, CAPACITY> m_stack;
        std::size_t m_stackTop;
        std::size_t m_size;
        std::size_t m_maxObjsUsed;
        std::mutex m_mutex;
    };


    template <typename T, std::size_t CAPACITY>
    ByteSmartObjectPool<T, CAPACITY>::ByteSmartObjectPool() noexcept
        : m_pool{}
        , m_stack{}
        , m_stackTop{ 0U }
        , m_size{ 0U }
        , m_maxObjsUsed{ 0U }
        , m_mutex{}
    {
        for (std::size_t i{ 0U }; i < CAPACITY; ++i)
        {
            m_stack[i] = i;
        }
    }

    template <typename T, std::size_t CAPACITY>
    ByteSmartObjectPool<T, CAPACITY>::~ByteSmartObjectPool() noexcept
    {
        // if the pool's destructor was called then all of its objects have been released
        // and no new objects would be requested

        for (; m_maxObjsUsed != s_infinity && m_maxObjsUsed >= 0U; --m_maxObjsUsed)
        {
            T& obj = reinterpret_cast<T&>(m_pool[m_maxObjsUsed * sizeof(T)]);
            obj.~T();
        }
    }

    template <typename T, std::size_t CAPACITY>
    template <typename... Args>
    PoolItem<T> ByteSmartObjectPool<T, CAPACITY>::request(Args&&... args) noexcept(false)
    {
        std::lock_guard lock{ m_mutex };

        if (m_stackTop == CAPACITY)
        {
            throw max_capacity_exception{};
        }

        m_maxObjsUsed = std::max(m_maxObjsUsed, m_stackTop);

        ++m_stackTop;

        ++m_size;

        return { new (&m_pool[m_stack[m_stackTop - 1] * sizeof(T)]) T{ std::forward<Args>(args)... }, [this](T* obj)
            {
                // NOTE: The pool's lifetime must exceed that of its objects, 
                // otherwise it'll lead to undefined behavior

                std::lock_guard lock{ m_mutex };

                std::size_t freedObjIdx{ static_cast<std::size_t>(obj - reinterpret_cast<T*>(m_pool.data())) };

                --m_stackTop;
                m_stack[m_stackTop] = freedObjIdx;

                --m_size;
            }
        };
    }

    template <typename T, std::size_t CAPACITY>
    consteval std::size_t ByteSmartObjectPool<T, CAPACITY>::capacity() const noexcept
    {
        return CAPACITY;
    }

    template <typename T, std::size_t CAPACITY>
    std::size_t ByteSmartObjectPool<T, CAPACITY>::size() const noexcept
    {
        return m_size;
    }

    template <typename T, std::size_t CAPACITY>
    bool ByteSmartObjectPool<T, CAPACITY>::isFull() const noexcept
    {
        return m_size == CAPACITY;
    }
}


#endif // !BYTE_SMART_OBJECT_POOL