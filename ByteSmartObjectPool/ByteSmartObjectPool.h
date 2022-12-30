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

        template <typename... Args>
        [[nodiscard]] PoolItem<T> request(Args&&... args) noexcept(false);

        ~ByteSmartObjectPool() noexcept;

    private:
        static constexpr std::size_t s_infinity{ std::numeric_limits<std::size_t>::max() }; // signifies a wrap around

        std::array<std::byte, sizeof(T) * CAPACITY> m_pool;
        std::array<std::size_t, CAPACITY> m_stack;
        std::size_t m_stackTop;
        std::size_t m_maxObjsUsed;
        std::mutex m_mutex;
    };


    template <typename T, std::size_t CAPACITY>
    ByteSmartObjectPool<T, CAPACITY>::ByteSmartObjectPool() noexcept
        : m_pool{}
        , m_stack{}
        , m_stackTop{ 0U }
        , m_maxObjsUsed{ 0U }
        , m_mutex{}
    {
        for (std::size_t i = 0; i < CAPACITY; ++i)
        {
            m_stack[i] = i;
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

        return { new (&m_pool[m_stack[m_stackTop - 1] * sizeof(T)]) T{ std::forward<Args>(args)... }, [this](T* obj)
            {
                std::lock_guard lock{ m_mutex };

                std::size_t idx = obj - reinterpret_cast<T*>(m_pool.data());

                --m_stackTop;
                m_stack[m_stackTop] = idx;
            }
        };
    }

    template <typename T, std::size_t CAPACITY>
    ByteSmartObjectPool<T, CAPACITY>::~ByteSmartObjectPool() noexcept
    {
        for (; m_maxObjsUsed != s_infinity && m_maxObjsUsed >= 0U; --m_maxObjsUsed)
        {
            T& obj = reinterpret_cast<T&>(m_pool[m_maxObjsUsed * sizeof(T)]);
            obj.~T();
        }
    }
}


#endif // !BYTE_SMART_OBJECT_POOL