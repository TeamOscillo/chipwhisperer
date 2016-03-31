/* lea_enc.h */
/*
 This file is part of the AVR-Crypto-Lib.
 Copyright TeamOscillo (qhdwls202@naver).
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

*/
/**
 * \file     lea_enc.h
 * \email    qhdwls202@naver.com
 * \author   TeamOscillo
 * \date     2016-03-14
 **/
#ifndef LEA_ENC_H_
#define LEA_ENC_H_
#include "lea_types.h"
#include <stdint.h>


void lea_encrypt_core(lea_cipher_state_t* state,const LEA_KEY *key);


#endif

