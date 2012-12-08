/*
 * GlobalData.h
 *
 *  Created on: 2012-12-07
 *      Author: sr2chowd
 */

#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

#include "../protocol/plexus/rm/ReadMullerCode.h"

class GlobalData
{
public:
	static ReedMuller* rm;
};

ReedMuller* GlobalData::rm = new ReedMuller(2, 4);

#endif /* GLOBALDATA_H_ */
