#ifndef _STACK_H
#define _STACK_H

#include <memory>
#include <shared_mutex>
#include <optional>

template<typename T>
class ThreadSafeStack {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    mutable std::shared_mutex mtx;
public:
    bool push(const T& value);
    std::optional<T> pop();
    bool empty() const;
};

// Implementation

template <typename T>
bool ThreadSafeStack<T>::push(const T& value) {
    std::unique_lock lock(mtx);
    auto tmp = std::make_unique<Node>(value);
    tmp->next = std::move(head);
    head = std::move(tmp);
    return true;
}

template <typename T>
std::optional<T> ThreadSafeStack<T>::pop() {
    std::unique_lock lock(mtx);
    if (!head) return std::nullopt;
    T value = head->data;
    head = std::move(head->next);
    return value;
}

template <typename T>
bool ThreadSafeStack<T>::empty() const {
    std::shared_lock lock(mtx);
    return !head;
}

#endif
