#include "lib.h"
#include <iostream>
#include <thread>
#include <vector>

void producer(ThreadSafeStack<int>& stack, int start, int count) {
    for (int i = 0; i < count; ++i) {
        stack.push(start + i);
    }
}

void consumer(ThreadSafeStack<int>& stack, int count) {
    for (int i = 0; i < count; ++i) {
        auto val = stack.pop();
        if (val) {
            std::cout << "Popped: " << *val << std::endl;
        } else {
            std::cout << "Stack empty!" << std::endl;
        }
    }
}

int main() {
    ThreadSafeStack<int> stack;
    const int items = 10;

    std::thread prod(producer, std::ref(stack), 100, items);
    std::thread cons(consumer, std::ref(stack), items);

    prod.join();
    cons.join();

    return 0;
}
