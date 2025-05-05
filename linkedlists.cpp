// Singly
#include <iostream>
using namespace std;

struct Node{
    int data;
    struct Node* next;
};

//insertion
void insertNode(Node** head, int data){
    Node* newNode= new Node();
    newNode->data= data;
    newNode->next= *head;
    *head= newNode;
    // cout<< "Inserted node with data: " << newNode->data << endl;

};

void printList(Node* head){
    Node* cur= head;
    cout<< "List: ";
    while(cur!=NULL){
        cout<< cur->data<<" ";
        cur= cur->next;
    }
    cout<< endl;
}
//doubly

struct NodeD{
    int data;
    struct NodeD* next;
    struct NodeD* prev;
};

int main(){
    struct Node* head= NULL;
    insertNode(&head, 1);
    insertNode(&head, 2);
    insertNode(&head, 3);
    printList(head);
    return 0;
}