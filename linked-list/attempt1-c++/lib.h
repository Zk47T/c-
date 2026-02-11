#ifndef _LIB_H
#define _LIB_H

#include <memory>
#include <iostream>


template <typename T>
class LinkedList {
private:    
    struct Node {
        T data;
        std::unique_ptr<Node> next;

        explicit Node(const T& value) : data(value), next(nullptr) {};
    };

    std::unique_ptr<Node> head;
    Node* tail = nullptr;
public:
    bool add(const T& value);
    bool remove(const T& value);
    void print() const;
};

template<typename T> 
bool LinkedList<T>::add (const T& value)
{
    auto tmp = std::make_unique<Node> (value);
    auto rawPtr = tmp.get();
    if(!head)
    {
        head = std::move(tmp);
        tail = rawPtr;
    }
    else
    {
        tail -> next = std::move(tmp);
        tail = rawPtr;
    }
    return true;
}
template<typename T>
bool LinkedList<T>::remove (const T& value)
{
    if (!head) return false;

    //remove head
    if(head -> data == value)
    {
        head = std::move(head -> next);
        if(!head) tail = nullptr;
        return true;
    }

    //general case
    Node* current = head.get();
    while (current -> next != nullptr)
    {
        if (current -> next -> data == value)
        {
            current -> next = std::move((current -> next) -> next);
            if (current -> next == nullptr) tail = nullptr;
            return true;
        }
        current = (current -> next).get();
    }
    return false;
}

template <typename T> 
void LinkedList<T>::print() const
{
    Node* tmp = head.get();
    std::cout << "The list consist of ";
    while (tmp)
    {
        std::cout << " " << tmp ->data;
        tmp = (tmp->next).get();
    }
    std::cout<<std::endl;
}


#endif