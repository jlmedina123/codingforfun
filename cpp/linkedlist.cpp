#include <iostream>
using std::cin;
using std::cout;
using std::endl;
 
class Node {
    /* friend funtion can access class private data */
    /* friend class' function can access the class private data */
    friend class List; /* List member functions can
                          access Node private data */
    public:
        int getData() const;
        Node(const int &data);
  private:
        int data;
        Node *next;
};
 
Node::Node(const int &d) :
    data (d), next(0) {
}
 
int Node::getData() const {
    return data;
}
 
class List {
    public:
        List();
        ~List();
        List& insertOrder(const int &data);
        bool remove(const int &data);
        bool isEmpty() const;
        void print() const;
    private:
        Node *head;
        Node * allocNode(const int &data);
 
};
 
List::List() :
    head (0) {
}
 
List::~List() {
    if (!isEmpty()) {
        Node *currentptr = head->next;
        Node *tmp;
        while (currentptr != 0) {
            tmp = currentptr;
            currentptr = currentptr->next;
            delete tmp;
        }
    }
}
 
Node * List::allocNode(const int &data) {
    return new Node(data);
}
 
List& List::insertOrder(const int &data) {
    Node *newnode = allocNode(data);
    if (isEmpty()) {
        head = newnode;
        return *this;
    }
    Node *current = head->next;
    Node *prev = head;
    while(current != 0) {
        if (current->getData() > data) {
            // insert node
            prev->next = newnode;
            newnode->next = current;
            return *this; // for cascading
        }
        // move forward
        prev = current;
        current = current->next;
    }
    // inserting at end
    prev->next = newnode;
    newnode->next = 0;
    return *this; // for cascading
}
 
bool List::remove(const int &d) {
    Node *current = head->next;
    Node *prev = head;
    while (current != 0) {
        if (current->getData() == d) {
            prev->next = current->next;
            delete current;
            cout << d << " removed from list" << endl;
            return true;
        }
        prev = current;
        current = current->next;
    }
    cout << "Couldnt remove " << d << " from list" << endl;
    return false;
}
 
bool List::isEmpty() const {
     return head == 0 ? true : false;
}
 
 
void List::print() const {
    if (!isEmpty()) {
        Node *current = head;
        while (current != 0) {
            cout << current->getData() << ", ";
            current = current->next;
        }
    }
    cout << endl;
}
 
int main() {
    int numbers[] = {1, 4, 7, 2, 3};
    List list;
    list.print();
    for (int i = 0; i < 5; i++) {
        list.insertOrder(numbers[i]).print();
    }
    list.remove(4);
    list.print();
 
    return 0;
}
 
 
 /*
g++ cpp_linkedlist.cpp -g
 ./a.out
 
1,
1, 4,
1, 4, 7,
1, 2, 4, 7,
1, 2, 3, 4, 7,
4 removed from list
1, 2, 3, 7,
*/
