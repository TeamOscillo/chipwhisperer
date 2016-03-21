/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA LEA driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA LEA driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA LEA crypto instruction.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1317 Using the XMEGA built in LEA accelerator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef LEA_DRIVER_H
#define LEA_DRIVER_H

#include "avr_compiler.h"

/* Length of one block. Always 128-bits (16 bytes). */
#define LEA_BLOCK_LENGTH	16

/* \brief LEA structure used by the LEA interrupt driver.*/
typedef struct LEA_interrupt_driver
{
  	/*! \brief  pointer to the LEA input (plaintext or ciphertext)*/
	uint8_t * input_ptr;
 	/*! \brief pointer to the key used by the LEA*/
	uint8_t * key_ptr;
	/*! \brief  pointer to the initialization vector needed in CBC*/
	uint8_t * init_ptr;
	/*! \brief  pointer to the LEA output (plaintext or ciphertext)*/
	uint8_t * output_ptr;
	/*! \brief  variable that stores the number of blocks to encrypt/decrypt.*/
	uint8_t block_count;
	/*! \brief  variable that stores the number of blocks left to encrypt/decrypt.*/
	uint8_t blocks_left;
	/*! \brief  variable that tell if decryption or encryption shall be done*/
	bool decrypt;
} LEA_interrupt_driver_t;


/* Definitions of macros */

/*! \brief  This macro enable LEA module encryption mode. */
#define LEA_encryption_mode_set()   ( LEA.CTRL = LEA.CTRL & (~LEA_DECRYPT_bm) )

/*! \brief  This macro enable LEA module decryption mode. */
#define LEA_decryption_mode_set()   ( LEA.CTRL |= LEA_DECRYPT_bm )

/*! \brief  This macro enable the auto start feature in the LEA module. */
#define LEA_auto_enable()           ( LEA.CTRL |= LEA_AUTO_bm )

/*! \brief  This macro disable the auto start feature in the LEA module. */
#define LEA_auto_disable()          ( LEA.CTRL = LEA.CTRL & (~LEA_AUTO_bm) )

/*! \brief  This macro enable the xor feature in the LEA module. */
#define LEA_xor_enable()            ( LEA.CTRL |= LEA_XOR_bm )

/*! \brief  This macro disable the xor feature in the LEA module. */
#define LEA_xor_disable()           ( LEA.CTRL = LEA.CTRL & (~LEA_XOR_bm) )

/*! \brief  This macro resets all registers in LEA module. */
#define LEA_software_reset()        ( LEA.CTRL = LEA_RESET_bm )

/*! \brief  This macro starts a decryption/encryption. */
#define LEA_start()                 ( LEA.CTRL |= LEA_START_bm )

/*! \brief  This macro checks if LEA state ready interrupt flag is set.
 *
 *   \retval true  if State Ready interrupt flag is set.
 *   \retval false if State Ready interrupt flag is not set.
 */
#define LEA_state_ready_flag_check()	((LEA.STATUS & LEA_SRIF_bm) != 0)

/*! \brief  This macro checks if the error flag is set.
 *
 *  \retval true  if LEA Error flag is set.
 *  \retval false if LEA Error flag not set.
 */
#define LEA_error_flag_check()	    ((LEA.STATUS & LEA_ERROR_bm) != 0)



/* Prototyping of Interrupt driver functions */
void LEA_interrupt_driver_init(LEA_interrupt_driver_t * interrupt_driver,
                               uint8_t * input_ptr, uint8_t * output_ptr,
                               uint8_t * LEA_key, uint8_t * LEA_init,
                               uint8_t block_count, bool decrypt);
bool LEA_interrupt_driver_start(LEA_interrupt_driver_t * interrupt_driver,
                                LEA_INTLVL_t int_lvl);
void LEA_interrupt_handler(LEA_interrupt_driver_t * interrupt_driver);
bool LEA_interrupt_driver_finished(LEA_interrupt_driver_t * interrupt_driver);
void LEA_interruptlevel_set(LEA_INTLVL_t int_lvl);

/* Prototyping of Polled driver functions */
bool LEA_encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * key);
bool LEA_decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * key);
bool LEA_encrypt_backtoback(uint8_t * plaintext, uint8_t * ciphertext);
bool LEA_decrypt_backtoback(uint8_t * ciphertext, uint8_t * plaintext);
bool LEA_lastsubkey_generate(uint8_t * key, uint8_t * decrypt_key);
bool LEA_CBC_encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * keys,
                     uint8_t * init, uint16_t block_count);
bool LEA_CBC_decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * keys,
                     uint8_t * init, uint16_t block_count);

#endif
