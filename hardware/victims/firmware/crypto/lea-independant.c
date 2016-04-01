/*
    This file is part of the leaExplorer Example Targets
    Copyright (C) 2016 TeamOscillo <qhdwls202@naver.com>

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
 * \file     lea-independant.c
 * \email    qhdwls202@naver.com
 * \author   TeamOscillo
 * \date     2016-03-14
 * \license  GPLv3 or later
 */

#include "lea-independant.h"

#ifdef AVRCRYPTOLIB
#include "lea128_enc.h"
#include "lea_keyschedule.h"

LEA_KEY ctx; /* lea round key set */

void lea_indep_init(void)
{
	;
}

void lea_indep_key(uint8_t * key)
{
	lea128_init(&ctx,key); /* ctx : round key , key: real_key */
}
/* encrypting the data block */
void lea_indep_enc(uint8_t * pt)
{
	lea128_enc(pt, &ctx);  /* pt : plain text */
}

#elif defined(SIMPLElea)

uint8_t enckey[16];

void lea_indep_init(void)
{
	;
}

void lea_indep_key(uint8_t * key)
{
	for(uint8_t i=0; i < 16; i++){
		enckey[i] = key[i];
	}
}

void lea_indep_enc(uint8_t * pt)
{
	uint8_t * result = lea(pt, enckey);
	for(uint8_t i=0; i < 16; i++){
		pt[i] = result[i];
	}
}

#elif defined(DPAV4)

#include "lea.h"
#include "lea_enc.h"

/*  This is the lea RSM 256 encryption function that call the generic lea RSM encryption core*/
void lea256_enc(uint8_t* j, void* buffer, lea256_ctx_t* ctx,uint8_t rng){
	lea_encrypt_core(j,buffer, (lea_genctx_t*)ctx, 14,(uint8_t)rng);
}

lea256_ctx_t ctx;

void lea_indep_init(void)
{
    ;
}

void lea_indep_key(uint8_t * key)
{
	lea256_init(key, &ctx);
}

void lea_indep_enc(uint8_t * pt)
{
	static uint8_t j[0];

	//Encryption with trigger enabled
	lea256_enc(j, pt, &ctx, 1);
}


#else

#error "No Crypto Lib Defined?"

#endif
