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
	static string host_file_name;
	static string config_file_name;
	static int network_size;
};

ReedMuller* GlobalData::rm = new ReedMuller(2, 4);
string GlobalData::host_file_name = "ihostlist";
string GlobalData::config_file_name = "config_temp";
int GlobalData::network_size;
#endif /* GLOBALDATA_H_ */
