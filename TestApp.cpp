#include <iostream>
#include <thread>

using namespace std::chrono_literals;

class Monitor final
{
private:
    std::thread m_thread;
    int m_value;

    void Exec()
    {
        for (;;)
        {
            ++m_value;
            std::this_thread::sleep_for(100ms);
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

    int GetValue() const
    {
        return m_value;
    }
};


int main()
{
    Monitor monitor;

    for (int i = 0; i < 10; ++i)
    {
        std::cout << monitor.GetValue() << std::endl;
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
