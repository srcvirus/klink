#include "double_linked_list.h"
#include "ip_address.h"
#include "overlay_id.h"


int main(){

    DoublyLinkedList ddl;
    OverlayID oid(1,1,1);
    IPAddress ip;
    Node *n1 = new Node(&oid, &ip);
    Node *n2 = new Node(new OverlayID(2,2,2), new IPAddress());
    ddl.append2Head(n1);
    ddl.append2Head(n2);
    ddl.printNodesForward();
    return 0;
}