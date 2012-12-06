/* 
 * File:   doubleLinkedList.h
 * Author: mfbari
 *
 * Created on November 28, 2012, 9:00 PM
 */

#ifndef DOUBLELINKEDLIST_H
#define	DOUBLELINKEDLIST_H

#include <iostream>
#include <cstring>

#include "overlay_id.h"
#include "host_address.h"

using namespace std;

struct DLLNode {
    OverlayID *key;
    HostAddress *value;
    DLLNode *prev, *next;

    DLLNode() {
    }

    DLLNode(OverlayID *key, HostAddress * value) {
        this->key = key;
        this->value = value;
        prev = next = NULL;
    }
};

class DoublyLinkedList {
    DLLNode *head, *tail;
public:

    DoublyLinkedList() {
        head = tail = NULL;
    }

    ~DoublyLinkedList() {
        DLLNode *temp1 = tail, *temp2;
        while (temp1 != NULL) {
            temp2 = temp1;
            temp1 = temp1->prev;
            delete temp2;
        }
        head = tail = NULL;
    }

    void append2Head(DLLNode *node) {
        if (head == NULL) {
            head = tail = node;
        } else {
            head->prev = node;
            node->next = head;
            head = node;
        }
    }

    void append2Head(OverlayID *key, HostAddress *value) {
        DLLNode *node = new DLLNode(key, value);
        append2Head(node);
    }

    void append2Tail(DLLNode *node) {
        if (tail == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }

    void append2Tail(OverlayID *key, HostAddress *value) {
        DLLNode *node = new DLLNode(key, value);
        append2Tail(node);
    }

    DLLNode* getTail() {
        return tail;
    }

    DLLNode* getHead() {
        return head;
    }
    
    DLLNode* extract(DLLNode *node) {
        if (head == NULL || tail == NULL) {
            return NULL;
        } else if (head == node && tail == node) {
            head = tail = NULL;
        } else if (head == node) {
            head = node->next;
            head->prev = NULL;
        } else if (tail == node) {
            tail = node->prev;
            tail->next = NULL;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        node->next = node->prev = NULL;
        return node;
    }

    void remove(DLLNode *node) {
        delete extract(node);
    }

    void move2Head(DLLNode *node) {
        if (head == NULL || tail == NULL)
            return;
        if (head != node) {
            append2Head(extract(node));
        }
    }

    void printNodesReverse() {
        DLLNode *temp = tail;
        cout << endl << "Nodes in reverse order :" << endl;
        while (temp != NULL) {
            cout << "<" << temp->key->GetOverlay_id() << ", " << temp->value << "> ";
            temp = temp->prev;
        }
        cout << endl;
    }

    void printNodesForward() {
        DLLNode *temp = head;
        cout << endl << "Nodes in forward order:" << endl;
        while (temp != NULL) {
            cout << "<" << temp->key->GetOverlay_id() << ", " << temp->value->GetHostName() << "> ";
            temp = temp->next;
        }
        cout << endl;
    }
};


#endif	/* DOUBLELINKEDLIST_H */

