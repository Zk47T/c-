#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class CircularQueue
{
private:
    std::vector<T> buffer;
    size_t head;
    size_t tail;
    size_t count;

    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    explicit CircularQueue(size_t capacity)
        : buffer(capacity), head(0), tail(0), count(0)
    {
    }

    bool produce(const T& value)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (buffer.empty())
        {
            return false;
        }

        not_full.wait(lock, [this]() { return count < buffer.size(); });
        buffer[tail] = value;

        ++count;
        tail = (tail + 1) % buffer.size();
        not_empty.notify_one();
        return true;
    }

    T consume()
    {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this]() { return count > 0; });

        T result = buffer[head];
        head = (head + 1) % buffer.size();
        --count;
        not_full.notify_one();
        return result;
    }
};

int main()
{
    CircularQueue<int> queue(5);

    std::thread t1([&queue]() {
        for (int i = 0; i < 10; ++i)
        {
            std::cout << "Producing ... " << i << std::endl;
            queue.produce(i);
            std::cout << "Produced !!" << std::endl;
        }
    });

    std::thread t2([&queue]() {
        for (int i = 0; i < 10; ++i)
        {
            std::cout << "Consuming ... " << std::endl;
            int value = queue.consume();
            std::cout << "Consume !! " << value << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    t1.join();
    t2.join();

    return 0;
}
