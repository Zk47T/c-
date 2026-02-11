#ifndef LIB_H
#define LIB_H

class Linked_List {
private:
    
    struct Node {
        Node *ptr;
        int data;

        Node(int _data)
        {
            ptr = nullptr;
            data = _data;
        }
    };
    
    Node* head = nullptr;
    Node* tail = nullptr;
public:
    Node* getHead() { return head; }
    void add(int data);
    bool remove(int data);
    void print(Node* ptr);
};


#endif