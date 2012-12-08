/*
 * GlobalData.h
 *
 *  Created on: 2012-12-07
 *      Author: sr2chowd
 */

#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

#include "../protocol/plexus/rm/ReadMullerCode.h"
//#include "overlay_id.h"

class OverlayID;

class GlobalData
{
public:
	static ReedMuller* rm;
        static OverlayID *oid;
};

ReedMuller* GlobalData::rm = new ReedMuller(2, 4);
OverlayID* GlobalData::oid;
#endif /* GLOBALDATA_H_ */
