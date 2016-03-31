/* lea128_enc.c */
/*
 This file is part of the AVR-Crypto-Lib.
 Copyright TeamOscillo (qhdwls202@naver).
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

*/
/**
 * \file     lea128_enc.h
 * \email    qhdwls202@naver.com
 * \author   TeamOscillo
 * \date     2016-03-14
 **/

#include "lea.h"
#include "lea_enc.h"

void lea128_enc(void* buffer, LEA_KEY* ctx){
	lea_encrypt_core(buffer,ctx);
}


