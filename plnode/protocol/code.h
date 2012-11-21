/*
 * code.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef CODE_H_
#define CODE_H_

class ABSCode
{
public:
	ABSCode();
	virtual ~ABSCode();

	virtual void encode(long data) = 0;
	virtual long decode() = 0;
};


#endif /* CODE_H_ */
