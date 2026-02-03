#include <iostream>
#include <thread>
#include <array>
#include <span>

using namespace std::chrono_literals;

template <size_t Size, class BaseType = unsigned char>
class LockFreeMemory final
{
public:
    using Buffer = std::array<BaseType, Size>;

private:
    static constexpr size_t BufferCount = 3;
    std::array<Buffer, BufferCount> m_buffers;
    size_t m_readIndex;
    size_t m_writeIndex;

public:
    LockFreeMemory() :
        m_buffers(),
        m_readIndex(BufferCount - 1),
        m_writeIndex(0)
    {
    }

    ~LockFreeMemory() = default;
    LockFreeMemory(const LockFreeMemory&) = delete;
    LockFreeMemory(LockFreeMemory&&) = default;
    LockFreeMemory& operator=(const LockFreeMemory&) = delete;
    LockFreeMemory& operator=(LockFreeMemory&&) = default;

    const Buffer& Read() const
    {
        return m_buffers[m_readIndex];
    }

    void Write(const std::span<BaseType, Size>& values)
    {
        memcpy(m_buffers[m_writeIndex].data(), values.data(), Size);
        m_readIndex = m_writeIndex;
        m_writeIndex = (m_writeIndex + 1) % BufferCount;
    }
};

class Monitor final
{
private:
    std::thread m_thread;
    int m_value;
    LockFreeMemory<4> m_buffer;

    void Exec()
    {
        for (;;)
        {
            m_value = rand();
            m_buffer.Write(std::span<unsigned char, 4>(reinterpret_cast<unsigned char*>(&m_value), reinterpret_cast<unsigned char*>(&m_value + 1)));
            std::this_thread::sleep_for(1s);
        }
    }

public:
    Monitor() :
        m_thread(&Monitor::Exec, this),
        m_value(0)
    {
    }

    ~Monitor()
    {
        m_thread.detach();
    }

    Monitor(const Monitor&) = delete;
    Monitor(Monitor&&) = default;
    Monitor& operator=(const Monitor&) = delete;
    Monitor& operator=(Monitor&&) = default;

    auto GetValue() const -> decltype(m_buffer.Read())
    {
        return m_buffer.Read();
    }
};


int main()
{
    Monitor monitor;

    for (int i = 0; i < 10; ++i)
    {
        std::cout << i << std::endl;
        for (auto&& v : monitor.GetValue())
        {
            std::cout << (int)v << ", ";
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
