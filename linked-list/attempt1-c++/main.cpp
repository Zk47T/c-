#include <iostream>
#include <string>
#include "lib.h"

using namespace std;

int main()
{
    LinkedList<int> list;
    string command;
    int value;
    
    cout << "=== Linked List Manager ===" << endl;
    cout << "Commands: add <value>, remove <value>, print, exit" << endl;
    cout << endl;
    
    while(true)
    {
        cout << "> ";
        cin >> command;
        
        if(command == "add")
        {
            cin >> value;
            list.add(value);
            cout << "Added " << value << " to the list" << endl;
        }
        else if(command == "remove")
        {
            cin >> value;
            bool removed = list.remove(value);
            if(removed)
                cout << "Removed " << value << " from the list" << endl;
            else
                cout << "Value " << value << " not found" << endl;
        }
        else if(command == "print")
        {
            list.print();
        }
        else if(command == "exit")
        {
            cout << "Exiting..." << endl;
            break;
        }
        else
        {
            cout << "Invalid command. Use: add <value>, remove <value>, print, or exit" << endl;
        }
        cout << endl;
    }
    
    return 0;
}
