#ifndef SIMULATION_DS_H
#define SIMULATION_DS_H

#include "../plnode/peer/peer.h"

class SimulationDataStore{
    Peer *peers = NULL;
public:
    SimulationDataStore(int networkSize){
        peers = new Peer[networkSize];
        for(int i=0;i<networkSize;i++){
            peers[i].setProtocol(new )
        }
    }

};
#endif