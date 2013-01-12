/*
 * code.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef CODE_H_
#define CODE_H_

class ABSCode {
public:

        ABSCode() {
        };

        virtual ~ABSCode() {
        };

        virtual long encode(long message) {
                return message;
        }

        virtual long decode(long codeword) {
                return codeword;
        }

        virtual int K() {
                return 20;
        };

        virtual int N() {
                K();
        };
};

#endif /* CODE_H_ */
