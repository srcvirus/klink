/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_SIMULATION_PROTOCOL_H
#define	PLEXUS_SIMULATION_PROTOCOL_H

#include "../protocol.h"

using namespace std;

class ABSProtocol;

class PlexusSimulationProtocol: public ABSProtocol
{
public:

	PlexusSimulationProtocol() :
			ABSProtocol()
	{
	}

	virtual void initiate_join()
	{
	}

	virtual void process_join()
	{
	}

	virtual bool setNextHop(ABSMessage*)
	{
		return false;
	}

	virtual void get(string name)
	{
	}

	virtual void put(string name, HostAddress my_address)
	{
	}

	virtual void rejoin()
	{
	}

};

#endif	/* PLEXUS_PROTOCOL_H */

