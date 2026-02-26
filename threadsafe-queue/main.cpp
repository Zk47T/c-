#include "lib.h"
#include <iostream>
#include <thread>
#include <vector>

void producer(ThreadSafeQueue<int>& queue, int start, int count) {
    for (int i = 0; i < count; ++i) {
        queue.enqueue(start + i);
    }
}

void consumer(ThreadSafeQueue<int>& queue, int count) {
    for (int i = 0; i < count; ++i) {
        auto val = queue.dequeue();
        if (val) {
            std::cout << "Dequeued: " << *val << std::endl;
        } else {
            std::cout << "Queue empty!" << std::endl;
        }
    }
}

int main() {
    ThreadSafeQueue<int> queue;
    const int items = 10;

    std::thread prod(producer, std::ref(queue), 100, items);
    std::thread cons(consumer, std::ref(queue), items);

    prod.join();
    cons.join();

    return 0;
}
