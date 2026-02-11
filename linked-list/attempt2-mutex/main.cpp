#include "lib.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono_literals;
int main()
{
    LinkedList<int> list;

    std::thread thread1([&] {
        for (int i = 0; i < 50; ++i) list.add(i);
    });

    std::thread thread2([&] {
        for (int i = 50; i < 100; ++i) list.add(i);
    });

    std::thread thread3([&] {
        for (int i = 0; i < 100; i += 2) {
            list.remove(i);
            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread thread4([&] {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(20ms);
            list.print();
        }
    });

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    list.print();
    return 0;
}
