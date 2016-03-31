/* lea_types.h */
/*
 This file is part of the AVR-Crypto-Lib.
 Copyright TeamOscillo (qhdwls202@naver).
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

*/
/**
 * \file     lea_types.h
 * \email    qhdwls202@naver.com
 * \author   TeamOscillo
 * \date     2016-03-14
 **/
 
#ifndef LEA_TYPES_H_
#define LEA_TYPES_H_

#include <stdint.h>

typedef struct lea_key_st
{
	uint32_t rk[192];
	uint8_t round;
} LEA_KEY;
/*
typedef struct{
	uint8_t ks[24];
} lea_roundkey_t;

typedef struct{
	lea_roundkey_t key[24+1];
} lea128_ctx_t;

typedef struct{
	lea_roundkey_t key[28+1];
} lea192_ctx_t;

typedef struct{
	lea_roundkey_t key[32+1];
} lea256_ctx_t;

typedef struct{
	lea_roundkey_t key[1];
} lea_genctx_t;
*/
typedef struct{
	uint8_t s[16];
} lea_cipher_state_t;

#endif

