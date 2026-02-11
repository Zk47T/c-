#include <utility>
#include <iostream>



#include "lib.h"
using namespace std;
void Linked_List::add(int _data)
{
    if (head == nullptr)
    {
        head  = new Node(_data);
        tail = head  ;
    }
    else
    {
        Node* tmp = new Node(_data);
        tail->ptr = tmp ;
        tail = std::move(tmp);
    }
}

bool Linked_List::remove(int _data)
{
    if(!head) return false;

    //special case : remove head
    if(head->data == _data)
    {
        Node* tmp = head;
        head = head-> ptr;
        if(head == nullptr) tail = nullptr;
        delete tmp;
        return true;
    }



    Node* tmp = head;
    Node* prev = tmp;

    while(tmp != nullptr)
    {
        if(tmp->data == _data)
        {
            prev->ptr = tmp->ptr;
            if(tmp == tail) tail = prev;
            delete tmp;
            return true;
        }
        prev = tmp;
        tmp = tmp->ptr;
    }
    return false;
}

void Linked_List::print(Node* ptr)
{
    cout << "Print all node data in Linked List ";
    while (ptr != nullptr)
    {
        cout<< " " << ptr->data;
        ptr = ptr ->ptr;
    }
    cout << endl;
}