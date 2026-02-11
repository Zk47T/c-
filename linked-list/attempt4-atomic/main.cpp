#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "lib.h"

using namespace std::chrono_literals;

int main() {
    LockFreeLinkedList<int> list;

    std::cout << "Lock-free list demo (push-front + remove).\n";

    // Producers add ranges concurrently.
    std::thread t1([&] {
        for (int i = 0; i < 50; ++i) list.add(i);
    });
    std::thread t2([&] {
        for (int i = 50; i < 100; ++i) list.add(i);
    });

    // Remover thread drops even numbers.
    std::thread t3([&] {
        for (int i = 0; i < 100; i += 2) {
            list.remove(i);
            std::this_thread::sleep_for(1ms);
        }
    });

    // Observer prints snapshots.
    std::thread t4([&] {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(25ms);
            list.print();
        }
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "Final list (order is LIFO, evens removed if seen):\n";
    list.print();
    return 0;
}
