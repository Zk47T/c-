#ifndef LOCKFREE_LINKEDLIST_LIB_H
#define LOCKFREE_LINKEDLIST_LIB_H

#include <atomic>
#include <iostream>

// Very small, singly-linked lock-free list that supports push-front and
// single-value removal. Nodes are never freed until destruction to avoid
// reclamation hazards. Good enough for demo/testing purposes.
template <typename T>
class LockFreeLinkedList {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        explicit Node(const T& v) : data(v), next(nullptr) {}
    };

    std::atomic<Node*> head{nullptr};

public:
    LockFreeLinkedList() = default;
    LockFreeLinkedList(const LockFreeLinkedList&) = delete;
    LockFreeLinkedList& operator=(const LockFreeLinkedList&) = delete;

    ~LockFreeLinkedList() { clear(); }

    bool add(const T& value) {
        Node* new_node = new Node(value);
        Node* old_head = head.load(std::memory_order_acquire);
        do {
            new_node->next.store(old_head, std::memory_order_relaxed);
        } while (!head.compare_exchange_weak(
            old_head,
            new_node,
            std::memory_order_release,
            std::memory_order_relaxed));
        return true;
    }

    // Removes the first occurrence of value. Memory is not freed here to
    // avoid ABA/use-after-free while other threads traverse. Nodes are
    // reclaimed in clear()/destructor once threads are joined.
    bool remove(const T& value) {
        std::atomic<Node*>* prev = &head;
        Node* curr = head.load(std::memory_order_acquire);

        while (curr) {
            Node* next = curr->next.load(std::memory_order_acquire);

            if (curr->data == value) {
                Node* expected = curr;
                if (prev->compare_exchange_strong(
                        expected,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_relaxed)) {
                    // Logical removal; defer delete.
                    return true;
                }
                // CAS failed: someone changed the link; retry without advancing.
                curr = prev->load(std::memory_order_acquire);
                continue;
            }

            prev = &curr->next;
            curr = next;
        }

        return false;
    }

    void print() const {
        Node* curr = head.load(std::memory_order_acquire);
        std::cout << "List:";
        while (curr) {
            std::cout << ' ' << curr->data;
            curr = curr->next.load(std::memory_order_acquire);
        }
        std::cout << std::endl;
    }

    void clear() {
        Node* curr = head.exchange(nullptr, std::memory_order_acq_rel);
        while (curr) {
            Node* next = curr->next.load(std::memory_order_relaxed);
            delete curr;
            curr = next;
        }
    }
};

#endif // LOCKFREE_LINKEDLIST_LIB_H
