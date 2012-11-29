/* 
 * File:   doubleLinkedList.h
 * Author: mfbari
 *
 * Created on November 28, 2012, 9:00 PM
 */

#ifndef DOUBLELINKEDLIST_H
#define	DOUBLELINKEDLIST_H

#include <iostream>

#include "ip_address.h"
#include "overlay_id.h"

using namespace std;

struct Node {
    OverlayID *key;
    IPAddress *value;
    Node *prev, *next;

//    Node(OverlayID key, IPAddress value) {
//        this->key = &key;
//        this->value = &value;
//        prev = next = NULL;
//    }

    Node(OverlayID *key, IPAddress *value) {
        this->key = key;
        this->value = value;
        prev = next = NULL;
    }
};

class DoublyLinkedList {
    Node *head, *tail;
public:

    DoublyLinkedList() {
        head = tail = NULL;
    }

    ~DoublyLinkedList() {
        Node *temp1 = tail, *temp2;
        while (temp1 != NULL) {
            temp2 = temp1;
            temp1 = temp1->prev;
            delete temp2;
        }
        head = tail = NULL;
    }

    void append2Head(Node *node) {
        if (head == NULL) {
            head = tail = node;
        } else {
            head->prev = node;
            node->next = head;
            head = node;
        }
    }

    void append2Head(OverlayID *key, IPAddress *value) {
        Node *node = new Node(key, value);
        append2Head(node);
    }

    void append2Tail(Node *node) {
        if (tail == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }

    void append2Tail(OverlayID *key, IPAddress *value) {
        Node *node = new Node(key, value);
        append2Tail(node);
    }

    void printNodesReverse() {
        Node *temp = tail;
        cout << endl << "Nodes in reverse order :" << endl;
        while (temp != NULL) {
            cout << "<" << temp->key->GetOverlay_id() << ", " << temp->value->getStrIpAddress() << "> ";
            temp = temp->prev;
        }
        cout << endl;
    }

    void printNodesForward() {
        Node *temp = head;
        cout << endl << "Nodes in forward order:" << endl;
        while (temp != NULL) {
            cout << "<" << temp->key->GetOverlay_id() << ", " << temp->value->getStrIpAddress() << "> ";
            temp = temp->next;
        }
        cout << endl;
    }
};


#endif	/* DOUBLELINKEDLIST_H */

