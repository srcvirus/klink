/*
 * message_test.cc
 *
 *  Created on: 2012-11-24
 *      Author: sr2chowd
 */

#include "../plnode/message/control/peer_config_msg.h"
#include "../plnode/message/message.h"

#include <stdio.h>

int main()
{
	ABSMessage* ptrConfigMsg = new PeerConfigMessage();
	ptrConfigMsg->setDestHost("localhost");
	ptrConfigMsg->setDestPort(50);
	ptrConfigMsg->setOverlayHops(1);
	ptrConfigMsg->setOverlayTtl(20);
	ptrConfigMsg->setSourceHost("localhost2");
	ptrConfigMsg->setSourcePort(1000);
	((PeerConfigMessage*)ptrConfigMsg)->setParameterAlpha(0.5);
	((PeerConfigMessage*)ptrConfigMsg)->setParameterK(1000);

	puts("Before Serialization");
	ptrConfigMsg->message_print_dump();
	int n = 0;
	char* buffer = ptrConfigMsg->serialize(&n);

	ABSMessage* dsMsg = new PeerConfigMessage();
	dsMsg->deserialize(buffer, n);

	puts("\nAfter Serialization");
	dsMsg->message_print_dump();

	return 0;
}



