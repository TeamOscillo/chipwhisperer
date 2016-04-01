/* lea_enc.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2016  TeamOscillo (qhdwls202@naver)
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file     lea_enc.c
 * \email    qhdwls202@naver.com
 * \author   TeamOscillo
 * \date     2016-03-14
 * \license  GPLv3 or later
 */

#include <stdint.h>
#include <string.h>
#include "lea.h"
#include "lea_enc.h"
#include <avr/pgmspace.h>

#define ROR(W,i) (((W) >> (i)) | ((W) << (32 - (i))))
#define ROL(W,i) (((W) << (i)) | ((W) >> (32 - (i))))
//#define loadU32(v)	((uint32_t)((((unsigned char*)(&v))[3]<<24)|(((unsigned char*)(&v))[2]<<16)|(((unsigned char*)(&v))[1]<<8)|(((unsigned char*)(&v))[0])))
#define loadU32(v)	(v)

static
void lea_enc_round(lea_cipher_state_t* state,const LEA_KEY *key){
	/* state : 16byte state ->s[i]*/
	uint32_t X0,X1,X2,X3;
	/* casting uint8_t -> uint32_t*/
	const uint32_t* _mk = (const uint32_t*)state->s;
	
	X0 = loadU32(_mk[0]);
	X1 = loadU32(_mk[1]);
	X2 = loadU32(_mk[2]);
	X3 = loadU32(_mk[3]);
	
	X3 = ROR((X2 ^ key->rk[  4]) + (X3 ^ key->rk[  5]), 3);
	X2 = ROR((X1 ^ key->rk[  2]) + (X2 ^ key->rk[  3]), 5);
	X1 = ROL((X0 ^ key->rk[  0]) + (X1 ^ key->rk[  1]), 9);
	X0 = ROR((X3 ^ key->rk[ 10]) + (X0 ^ key->rk[ 11]), 3);
	X3 = ROR((X2 ^ key->rk[  8]) + (X3 ^ key->rk[  9]), 5);
	X2 = ROL((X1 ^ key->rk[  6]) + (X2 ^ key->rk[  7]), 9);
	X1 = ROR((X0 ^ key->rk[ 16]) + (X1 ^ key->rk[ 17]), 3);
	X0 = ROR((X3 ^ key->rk[ 14]) + (X0 ^ key->rk[ 15]), 5);
	X3 = ROL((X2 ^ key->rk[ 12]) + (X3 ^ key->rk[ 13]), 9);
	X2 = ROR((X1 ^ key->rk[ 22]) + (X2 ^ key->rk[ 23]), 3);
	X1 = ROR((X0 ^ key->rk[ 20]) + (X1 ^ key->rk[ 21]), 5);
	X0 = ROL((X3 ^ key->rk[ 18]) + (X0 ^ key->rk[ 19]), 9);
	
	X3 = ROR((X2 ^ key->rk[ 28]) + (X3 ^ key->rk[ 29]), 3);
	X2 = ROR((X1 ^ key->rk[ 26]) + (X2 ^ key->rk[ 27]), 5);
	X1 = ROL((X0 ^ key->rk[ 24]) + (X1 ^ key->rk[ 25]), 9);
	X0 = ROR((X3 ^ key->rk[ 34]) + (X0 ^ key->rk[ 35]), 3);
	X3 = ROR((X2 ^ key->rk[ 32]) + (X3 ^ key->rk[ 33]), 5);
	X2 = ROL((X1 ^ key->rk[ 30]) + (X2 ^ key->rk[ 31]), 9);
	X1 = ROR((X0 ^ key->rk[ 40]) + (X1 ^ key->rk[ 41]), 3);
	X0 = ROR((X3 ^ key->rk[ 38]) + (X0 ^ key->rk[ 39]), 5);
	X3 = ROL((X2 ^ key->rk[ 36]) + (X3 ^ key->rk[ 37]), 9);
	X2 = ROR((X1 ^ key->rk[ 46]) + (X2 ^ key->rk[ 47]), 3);
	X1 = ROR((X0 ^ key->rk[ 44]) + (X1 ^ key->rk[ 45]), 5);
	X0 = ROL((X3 ^ key->rk[ 42]) + (X0 ^ key->rk[ 43]), 9);

	X3 = ROR((X2 ^ key->rk[ 52]) + (X3 ^ key->rk[ 53]), 3);
	X2 = ROR((X1 ^ key->rk[ 50]) + (X2 ^ key->rk[ 51]), 5);
	X1 = ROL((X0 ^ key->rk[ 48]) + (X1 ^ key->rk[ 49]), 9);
	X0 = ROR((X3 ^ key->rk[ 58]) + (X0 ^ key->rk[ 59]), 3);
	X3 = ROR((X2 ^ key->rk[ 56]) + (X3 ^ key->rk[ 57]), 5);
	X2 = ROL((X1 ^ key->rk[ 54]) + (X2 ^ key->rk[ 55]), 9);
	X1 = ROR((X0 ^ key->rk[ 64]) + (X1 ^ key->rk[ 65]), 3);
	X0 = ROR((X3 ^ key->rk[ 62]) + (X0 ^ key->rk[ 63]), 5);
	X3 = ROL((X2 ^ key->rk[ 60]) + (X3 ^ key->rk[ 61]), 9);
	X2 = ROR((X1 ^ key->rk[ 70]) + (X2 ^ key->rk[ 71]), 3);
	X1 = ROR((X0 ^ key->rk[ 68]) + (X1 ^ key->rk[ 69]), 5);
	X0 = ROL((X3 ^ key->rk[ 66]) + (X0 ^ key->rk[ 67]), 9);

	X3 = ROR((X2 ^ key->rk[ 76]) + (X3 ^ key->rk[ 77]), 3);
	X2 = ROR((X1 ^ key->rk[ 74]) + (X2 ^ key->rk[ 75]), 5);
	X1 = ROL((X0 ^ key->rk[ 72]) + (X1 ^ key->rk[ 73]), 9);
	X0 = ROR((X3 ^ key->rk[ 82]) + (X0 ^ key->rk[ 83]), 3);
	X3 = ROR((X2 ^ key->rk[ 80]) + (X3 ^ key->rk[ 81]), 5);
	X2 = ROL((X1 ^ key->rk[ 78]) + (X2 ^ key->rk[ 79]), 9);
	X1 = ROR((X0 ^ key->rk[ 88]) + (X1 ^ key->rk[ 89]), 3);
	X0 = ROR((X3 ^ key->rk[ 86]) + (X0 ^ key->rk[ 87]), 5);
	X3 = ROL((X2 ^ key->rk[ 84]) + (X3 ^ key->rk[ 85]), 9);
	X2 = ROR((X1 ^ key->rk[ 94]) + (X2 ^ key->rk[ 95]), 3);
	X1 = ROR((X0 ^ key->rk[ 92]) + (X1 ^ key->rk[ 93]), 5);
	X0 = ROL((X3 ^ key->rk[ 90]) + (X0 ^ key->rk[ 91]), 9);

	X3 = ROR((X2 ^ key->rk[100]) + (X3 ^ key->rk[101]), 3);
	X2 = ROR((X1 ^ key->rk[ 98]) + (X2 ^ key->rk[ 99]), 5);
	X1 = ROL((X0 ^ key->rk[ 96]) + (X1 ^ key->rk[ 97]), 9);
	X0 = ROR((X3 ^ key->rk[106]) + (X0 ^ key->rk[107]), 3);
	X3 = ROR((X2 ^ key->rk[104]) + (X3 ^ key->rk[105]), 5);
	X2 = ROL((X1 ^ key->rk[102]) + (X2 ^ key->rk[103]), 9);
	X1 = ROR((X0 ^ key->rk[112]) + (X1 ^ key->rk[113]), 3);
	X0 = ROR((X3 ^ key->rk[110]) + (X0 ^ key->rk[111]), 5);
	X3 = ROL((X2 ^ key->rk[108]) + (X3 ^ key->rk[109]), 9);
	X2 = ROR((X1 ^ key->rk[118]) + (X2 ^ key->rk[119]), 3);
	X1 = ROR((X0 ^ key->rk[116]) + (X1 ^ key->rk[117]), 5);
	X0 = ROL((X3 ^ key->rk[114]) + (X0 ^ key->rk[115]), 9);

	X3 = ROR((X2 ^ key->rk[124]) + (X3 ^ key->rk[125]), 3);
	X2 = ROR((X1 ^ key->rk[122]) + (X2 ^ key->rk[123]), 5);
	X1 = ROL((X0 ^ key->rk[120]) + (X1 ^ key->rk[121]), 9);
	X0 = ROR((X3 ^ key->rk[130]) + (X0 ^ key->rk[131]), 3);
	X3 = ROR((X2 ^ key->rk[128]) + (X3 ^ key->rk[129]), 5);
	X2 = ROL((X1 ^ key->rk[126]) + (X2 ^ key->rk[127]), 9);
	X1 = ROR((X0 ^ key->rk[136]) + (X1 ^ key->rk[137]), 3);
	X0 = ROR((X3 ^ key->rk[134]) + (X0 ^ key->rk[135]), 5);
	X3 = ROL((X2 ^ key->rk[132]) + (X3 ^ key->rk[133]), 9);
	X2 = ROR((X1 ^ key->rk[142]) + (X2 ^ key->rk[143]), 3);
	X1 = ROR((X0 ^ key->rk[140]) + (X1 ^ key->rk[141]), 5);
	X0 = ROL((X3 ^ key->rk[138]) + (X0 ^ key->rk[139]), 9);
		
	/*uint32_t -> uint8_t*/
	uint8_t i;
	for(i=0;i<4;i++){
		state->s[i] = X0;
		state->s[i+4] = X1;
		state->s[i+8] = X2;
		state->s[i+12] = X3;
		X0 >>=8;
		X1 >>=8;
		X2 >>=8;
		X3 >>=8;
	}

}

void lea_encrypt_core(lea_cipher_state_t* state, const LEA_KEY *key){
	
	lea_enc_round(state,key);
		
	/*
	for(i=0; i<16; ++i){
		state->s[i] ^= ks->key[0].ks[i];
	}
	i=1;
	for(;rounds>1;--rounds){
		lea_enc_round(state, &(ks->key[i]));
		++i;
	}
	lea_enc_lastround(state, &(ks->key[i]));
	*/
}
