#ifndef _LIB_H
#define _LIB_H

#include <memory>
#include <iostream> 
#include <mutex>

template<typename T>
class LinkedList 
{
private:
    struct Node
    {
        T data;
        std::unique_ptr<Node> next;

        explicit Node(const T& value) : data(value), next(nullptr) {} ;
    };

    std::unique_ptr<Node> head;
    Node* tail = nullptr;
    mutable std::mutex mtx;
public:
    bool add(const T& value);
    bool remove(const T& value);
    void print () const;
};

template <typename T>
bool LinkedList<T>::add(const T& value)
{
    std::lock_guard lock(mtx);
    auto tmp = std::make_unique<Node>(value);
    auto rawptr = tmp.get();
    if(!head)
    {
        head = std::move(tmp);
        tail = head.get();
    }
    else
    {
        tail -> next = std::move(tmp);
        tail = rawptr;
    }
    return true;
}

template <typename T>
bool LinkedList<T>::remove(const T& value)
{
    std::lock_guard lock(mtx);
    if(!head) return false;

    bool found = false;

    //remove head
    while(head && head -> data == value)
    {
        head = std::move(head->next);
        found = true;
    }
    if(!head) tail = nullptr;


    //general case 
    Node* current = head.get();
    while(current ->next  && current)
    {
        if(current -> next -> data == value)
        {
            current -> next = std::move(current -> next -> next);
            if (!(current -> next)) tail = current;
            found = true;
        }
        else
        {
            current = (current -> next).get();
        }
        
    }
    return found;
}

template <typename T>
void LinkedList<T>::print() const
{
    std::lock_guard lock(mtx);
    Node *tmp = head.get();
    std::cout << "The list consist of ";
    while (tmp)
    {
        std::cout << " " << tmp -> data;
        tmp = (tmp -> next).get();
    }
    std::cout << std::endl;
}

#endif