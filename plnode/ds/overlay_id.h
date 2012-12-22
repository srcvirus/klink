/* 
 * File:   OverlayID.h
 * Author: mfbari
 *
 * Created on November 26, 2012, 1:10 AM
 */

#ifndef OVERLAY_ID_H
#define	OVERLAY_ID_H

#include <iostream>

#include "../protocol/plexus/rm/ReadMullerCode.h"
#include "GlobalData.h"

using namespace std;

class GlobalData;

class OverlayID
{
	unsigned int overlay_id;
	int prefix_length;
public:
	int MAX_LENGTH;

	OverlayID()
	{
                ReedMuller *rm = new ReedMuller(2, 4);
                MAX_LENGTH = GlobalData::rm->rm->k;
	}

	OverlayID(unsigned int overlay_id, int prefix_lenght)
	{
                ReedMuller *rm = new ReedMuller(2, 4);
		MAX_LENGTH = GlobalData::rm->rm->k;
		this->overlay_id = overlay_id;
		this->prefix_length = prefix_lenght;
	}

	//    OverlayID(int pattern) {
	//        this->overlay_id = Peer::rm->array2int(Peer::rm->encode(Peer::rm->int2array(pattern, Peer::rm->rm->k)), Peer::rm->rm->n);
	//        this->prefix_length = Peer::rm->rm->n;
	//        this->MAX_LENGTH = Peer::rm->rm->n;
	//    }

	OverlayID(int pattern)
	{
                ReedMuller *rm = new ReedMuller(2, 4);
		MAX_LENGTH = GlobalData::rm->rm->k;
		this->overlay_id = GlobalData::rm->decode(pattern);
		this->prefix_length = GlobalData::rm->rm->k;
	}

	void SetPrefix_length(int prefix_length)
	{
		this->prefix_length = prefix_length;
	}

	int GetPrefix_length() const
	{
		return prefix_length;
	}

	void SetOverlay_id(int overlay_id)
	{
		this->overlay_id = overlay_id;
	}

	int GetOverlay_id() const
	{
		return overlay_id;
	}

	int GetBitAtPosition(int n) const
	{
		return (((this->overlay_id & (1 << n)) >> n) & 0x00000001);
	}

	OverlayID ToggleBitAtPosition(int n) const
	{
		OverlayID id(this->overlay_id, this->prefix_length);
		id.overlay_id ^= (1 << n);
		return id;
	}

	int GetMatchedPrefixLength(const OverlayID &id) const
	{
		int position = MAX_LENGTH - 1;
		for (; position > 0; position--)
		{
			if (this->GetBitAtPosition(position)
					!= id.GetBitAtPosition(position))
			{
				break;
			}
		}
		return MAX_LENGTH - position - 1;
	}

	bool operator==(const OverlayID &id) const
	{
		if (this->prefix_length != id.prefix_length)
			return false;
		if (this->GetMatchedPrefixLength(id) < this->prefix_length)
			return false;
		return true;
	}

	bool operator!=(const OverlayID &id) const
	{
		return !(*this == id);
	}

	//    bool operator<=(const OverlayID &id) {
	//        return this->overlay_id <= id.overlay_id;
	//    }

	bool operator<(const OverlayID &id) const
	{
		return this->overlay_id < id.overlay_id;
	}

	void printBits()
	{
		for (int i = MAX_LENGTH - 1; i >= 0; i--)
		{
			cout << GetBitAtPosition(i);
		}
	}
};
#endif	/* OVERLAY_ID_H */

