#ifndef _LIB_H
#define _LIB_H

#include <memory>
#include <shared_mutex>
#include <optional>

// ...existing code...

template<typename T>
class ThreadSafeQueue {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    Node* tail = nullptr;
    mutable std::shared_mutex mtx;
public:
    bool enqueue(const T& value);
    std::optional<T> dequeue();
    bool empty() const;
};

// Implementation

template <typename T>
bool ThreadSafeQueue<T>::enqueue(const T& value) {
    std::unique_lock lock(mtx);
    auto tmp = std::make_unique<Node>(value);
    auto rawptr = tmp.get();
    if (!head) {
        head = std::move(tmp);
        tail = head.get();
    } else {
        tail->next = std::move(tmp);
        tail = rawptr;
    }
    return true;
}

template <typename T>
std::optional<T> ThreadSafeQueue<T>::dequeue() {
    std::unique_lock lock(mtx);
    if (!head) return std::nullopt;
    T value = head->data;
    head = std::move(head->next);
    if (!head) tail = nullptr;
    return value;
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::shared_lock lock(mtx);
    return !head;
}

#endif
