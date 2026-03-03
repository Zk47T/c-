#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <chrono>

template<typename T>
class Queue
{
private:
    struct Node
    {
        T data;
        std::unique_ptr<Node> next;
        explicit Node(T value) : data(value), next(nullptr) {};
    };

    std::unique_ptr<Node> head;
    Node* tail;

    size_t capacity;
    size_t count;

    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    
public:
    Queue(size_t cap) : capacity(cap), count(0), tail(nullptr) {};
    ~Queue() {
        while(head)
        {
            head = std::move(head->next);
        }

        tail = nullptr;
    };

    bool publish(T value)
    {
        std::unique_lock<std::mutex> lock(mtx);
        auto tmp = std::make_unique<Node>(value);
        Node* rawptr = tmp.get();


        not_full.wait(lock, [this](){
            return count < capacity;
        });

        if(!head)
        {
            head = std::move(tmp);
            tail = rawptr;
        }
        else
        {
            tail -> next = std::move(tmp);
            tail = rawptr;
        }
        count ++;
        not_empty.notify_one();
        return true;
    }
    T consume()
    {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this](){
            return count > 0;
        });
        T g_data = head->data;
        
        //remove head
        head = std::move(head->next);
        count --;


        if(!head) tail = nullptr;

        not_full.notify_one();
        return g_data;
    }

    void print()
    {
        std::cout << "Queue: ";
        Node* tmp = head.get();
        while(tmp != nullptr)
        {
            std::cout << " " << tmp->data;
            tmp = (tmp -> next ).get();

        }
        std::cout << std::endl;
    }
};

int main()
{
    Queue<int> queue(5);
    std::thread t1([&queue](){
        for(int i =0 ; i < 10; i++)
        {
            std::cout << "Producing ... " << i << std::endl;
            queue.publish(i);
            std::cout << "Produced !!" << std::endl;
            
        }
    });

    std::thread t2 ([&queue](){
        for(int i = 0; i <10;i++)
        {
            std::cout << "Consuming ... " << std::endl;
            int tmp = queue.consume();
            std::cout << "Consume !! "<< tmp << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    t1.join();
    t2.join();

    return 0;
}

