/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA LEA driver source file.
 *
 *      This file contains the function implementations the XMEGA LEA driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA LEA module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
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

#if HW_CRYPTO

#include "LEA_driver.h"

/*! \brief  Function that initialize the interrupt driver
 *
 *  \param  interrupt_driver  Pointer to interrupt driver struct .
 *  \param  input_ptr         Pointer to the input blocks (plaintext/ciphertext).
 *  \param  output_ptr        Pointer to where to store the output.
 *  \param  LEA_key           Pointer to the key used by the LEA algorithm.
 *  \param  LEA_CBC_init      Pointer to initialization vector needed in CBC.
 *  \param  block_count       The number of block that is being encrypted/decrypted.
 *  \param  decrypt           Bool that determine if encryption or decryption is done.
 *
 */
void LEA_interrupt_driver_init(LEA_interrupt_driver_t * interrupt_driver,
                               uint8_t * input_ptr,
                               uint8_t * output_ptr,
                               uint8_t * LEA_key,
                               uint8_t * LEA_CBC_init,
                               uint8_t block_count,
                               bool decrypt)
{
	/* Initialize interrupt driver struct. */
	interrupt_driver->block_count = block_count;
	interrupt_driver->blocks_left = block_count;
	interrupt_driver->output_ptr = output_ptr;
	interrupt_driver->input_ptr = input_ptr;
	interrupt_driver->key_ptr = LEA_key;
	interrupt_driver->init_ptr = LEA_CBC_init;
	interrupt_driver->decrypt = decrypt;
}



/*! \brief  Function that starts the LEA interrupt driver
 *
 *  CBC is used if the number of blocks is more than one.
 *
 *  \param  interrupt_driver  Pointer to interrupt driver struct.
 *  \param  int_lvl           Interrupt level for the LEA module.
 *
 *  \retval true   Starting the LEA interrupt driver was successful.
 *  \retval false  Starting the LEA interrupt driver was not successful.
 *
 */
bool LEA_interrupt_driver_start(LEA_interrupt_driver_t * interrupt_driver,
                                LEA_INTLVL_t int_lvl)
{
  	bool start_ok;

  	/* Remove pending LEA interrupts. */
  	LEA.STATUS = (LEA_ERROR_bm | LEA_SRIF_bm);

	/* Set LEA to the desired interrupt level.
	 * NOTE: If interrupt level is set to off, interrupts will never execute. */
	LEA.INTCTRL = int_lvl;

	/* Put LEA module in right mode. */
	if(interrupt_driver->decrypt){
		LEA.CTRL |= LEA_DECRYPT_bm;
	}else{
		LEA.CTRL = LEA.CTRL & (~LEA_DECRYPT_bm);
	}

	/* If encryption and there are more than one block CBC is used. In CBC
	 * encryption the first plaintext block is xored with the initialization
	 * vector. */
	if((interrupt_driver->block_count > 1) && !(interrupt_driver->decrypt)){

		/* Load key to LEA Key memory. */
		uint8_t * temp_key_ptr = interrupt_driver->key_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		  	LEA.KEY =  *(temp_key_ptr++);
		}

		/* Load the first plaintext block to LEA State memory. */
		uint8_t * temp_input_ptr = interrupt_driver->input_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.STATE =  *(temp_input_ptr++);
		}
		interrupt_driver->input_ptr = temp_input_ptr;

		/* Enable Auto mode and the XOR feature. */
		LEA.CTRL = LEA.CTRL | LEA_XOR_bm | LEA_AUTO_bm;

		/* Load the initialization vector to the LEA State memory.
		 * The initialization vector is xored with the plaintext block already
		 * loaded into the memory and the LEA module is auto started. */
		uint8_t * temp_init_ptr = interrupt_driver->init_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.STATE =  *(temp_init_ptr++);
		}

		/* Check if error flag is set. */
		start_ok = !(LEA_error_flag_check());
	}
	/* If decryption or encryption of a single block the xor feature is not
	 * used. */
	else{

		/* Load key to LEA Key memory. */
	  	volatile uint8_t * temp_key_ptr = interrupt_driver->key_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.KEY =  *(temp_key_ptr++);
		}

		/* Enable Auto mode. */
		LEA.CTRL |= LEA_AUTO_bm;

		/* Load the first input block to LEA State memory. */
		uint8_t * temp_input_ptr = interrupt_driver->input_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.STATE =  *(temp_input_ptr++);
		}

		/* Update input pointer. */
		interrupt_driver->input_ptr = temp_input_ptr;

		/* Check if error flag is set. */
		start_ok = !(LEA_error_flag_check());
	}

	return start_ok;
}



/*! \brief  Function that control the LEA when State Ready Interrupts occurs.
 *
 *  CBC is used if the number of blocks is more than one.
 *
 *  \param  interrupt_driver  Pointer to interrupt driver struct.
 */
void LEA_interrupt_handler(LEA_interrupt_driver_t * interrupt_driver)
{
	/* If encryption is done, the answer can be read out directly from the
	 * state memory. Then the output pointer is updated. */
  	if(!(interrupt_driver->decrypt)){

		/* Store result to memory. */
		uint8_t * temp_output_ptr = interrupt_driver->output_ptr;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_output_ptr++) = LEA.STATE;
		}

		/* Update output pointer and the number of blocks left to
		 * encrypt/decrypt. */
		interrupt_driver->output_ptr = temp_output_ptr;
		interrupt_driver->blocks_left -= 1;

		/* If there are more blocks to encrypt a new encryption is started. */
	 	if(interrupt_driver->blocks_left > 0){

			/* Load key to LEA Key memory. */
		  	uint8_t * temp_key_ptr = interrupt_driver->key_ptr;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				LEA.KEY =  *(temp_key_ptr++);
			}

			/* Load the next plaintext to the LEA State memory. The block is xored
			 * with the previous encrypted block and the LEA module is auto
			 * started. */
			uint8_t * temp_input_ptr = interrupt_driver->input_ptr;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				LEA.STATE =  *(temp_input_ptr++);
			}

			/* Update input pointer. */
			interrupt_driver->input_ptr = temp_input_ptr;
		}
	}
	/* When decryption is done, the answer can only be read out directly if
	 * there only is one block to decrypt. If there are more than one block and
	 * CBC is used the answer must be xored with the previous cipher text or
	 * the initialization vector to reconstruct the plaintext. */
	else{

		/* If only one block should be decrypted the plaintext can be read out
		 * directly from the LEA State memory. */
		if(interrupt_driver->block_count == 1){

			/* Store result to memory. */
			uint8_t * temp_output_pointer = interrupt_driver->output_ptr;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				*(temp_output_pointer + i) = LEA.STATE;
			}
		}
		/* If there are more than one block to decrypt. */
		else{

			/* Disable of Auto mode and enable on the xor feature. */
			LEA.CTRL = (LEA.CTRL & (~LEA_AUTO_bm)) | LEA_XOR_bm;

			/* If it is the first block that is decrypted the answer must be
			 * xored with initialization vector to reconstruct the first
			 * plaintext. */
			uint8_t * temp_ptr;
			uint8_t temp_blocks_left = interrupt_driver->blocks_left;
			if(interrupt_driver->block_count == temp_blocks_left){
			  	temp_ptr = interrupt_driver->init_ptr;
			}
			/* Else the answer must be xored with previous ciphertext value to
			 * reconstruct the plaintext. */
			else{
				temp_ptr = interrupt_driver->input_ptr -(LEA_BLOCK_LENGTH*2);
			}

			/* Xor the initialization vector or the previous ciphertext with
			 * the answer from the decryption. */
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				LEA.STATE =  *(temp_ptr++);
			}

			/* Store the result. */
			uint8_t * temp_output_ptr = interrupt_driver->output_ptr;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				*(temp_output_ptr++) = LEA.STATE;
			}

			/* Update output pointer and the number of blocks left to
			 * encrypt/decrypt. */
			interrupt_driver->output_ptr = temp_output_ptr;
			interrupt_driver->blocks_left -= 1;

			/* If there are more block to decrypt a new decryption is started. */
			if(interrupt_driver->blocks_left > 0){

				/* Enable the Auto mode and disable the xor feature. */
				LEA.CTRL = (LEA.CTRL & (~LEA_XOR_bm)) | LEA_AUTO_bm;

				/* Load key to LEA Key memory. */
				uint8_t * temp_key_ptr = interrupt_driver->key_ptr;
				for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
					LEA.KEY =  *(temp_key_ptr++);
				}

				/* Load the next ciphertext block to the LEA State memory. The
				* LEA module is auto started. */
				uint8_t * temp_input_ptr = interrupt_driver->input_ptr;
				for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
					LEA.STATE =  *(temp_input_ptr++);
				}

				/* Update the input pointer. */
				interrupt_driver->input_ptr = temp_input_ptr;
			}
		}
	}
}



/*! \brief  Function that check if the interrupt driver is finished.
 *
 *  \param  interrupt_driver  Pointer to interrupt driver struct.
 *
 *  \retval true   The LEA interrupt driver is finished.
 *  \retval false  The LEA interrupt driver is not finished.
 */
bool LEA_interrupt_driver_finished(LEA_interrupt_driver_t * interrupt_driver)
{
 	bool finished = (interrupt_driver->blocks_left == 0);
	return finished;
}



/*! \brief  Polled function that does an LEA encryption on one 128-bit data block.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  plaintext  Pointer to the plaintext that shall be encrypted
 *  \param  ciphertext Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  key        Pointer to the LEA key
 *
 *  \retval true   If the LEA encryption was successful.
 *  \retval false  If the LEA encryption was not successful.
 */
bool LEA_encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * key)
{
  	bool encrypt_ok;

	/* Load key into LEA key memory. */
	uint8_t * temp_key = key;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.KEY =  *(temp_key++);
	}

	/* Load data into LEA state memory. */
	uint8_t * temp_plaintext = plaintext;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  *(temp_plaintext++);
	}

//    PORTA.OUT = PIN0_bm;

	/* Set LEA in encryption mode and start LEA. */
	LEA.CTRL = (LEA.CTRL & (~LEA_DECRYPT_bm)) | LEA_START_bm;
    
    //PORTA.OUT = 0;

	do{
		/* Wait until LEA is finished or an error occurs. */
	}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

	/* If not error. */
	if((LEA.STATUS & LEA_ERROR_bm) == 0){
		/* Store the result. */
		uint8_t * temp_ciphertext = ciphertext;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_ciphertext++) = LEA.STATE;
		}
		encrypt_ok = true;
	}else{
		encrypt_ok = false;

	}
	return encrypt_ok;
}



/*! \brief  Polled function that does an LEA decryption on one 128-bit data block.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param  key         Pointer to the DES key
 *
 *  \retval true   If the LEA decryption was successful.
 *  \retval false  If the LEA decryption was not successful.
 */
bool LEA_decrypt(uint8_t * ciphertext, uint8_t * plaintext,
                 uint8_t * key)
{
  	bool decrypt_ok;

	/* Load key into LEA key memory. */
	uint8_t * temp_key = key;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.KEY =  *(temp_key++);
	}

	/* Load data into LEA state memory. */
	uint8_t * temp_ciphertext = ciphertext;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  *(temp_ciphertext++);
	}

	/* Set LEA in decryption mode and start the LEA.*/
	LEA.CTRL |= (LEA_START_bm | LEA_DECRYPT_bm);

	do{
		/* Wait until LEA is finished or an error occurs. */
	}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

	/* If not error. */
	if((LEA.STATUS & LEA_ERROR_bm) == 0){
		/* Store the result. */
		uint8_t * temp_plaintext = plaintext;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_plaintext++) = LEA.STATE;
		}
		decrypt_ok = true;
	}else{
		decrypt_ok = false;

	}
	return decrypt_ok;
}




/*! \brief  Polled function that generates the last subkey of the Expanded Key
 *          needed during decryption.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  key           Pointer to LEA key.
 *  \param  last_sub_key  Pointer to where the last subkey of the Expanded Key
 *                        shall be stored.
 *
 *  \retval true   If generating the last subkey was successful.
 *  \retval false  If generating the last subkey was not successful.
 */
bool LEA_lastsubkey_generate(uint8_t * key, uint8_t * last_sub_key)
{
	bool keygen_ok;
	LEA_software_reset();

	/* Load key into LEA key memory. */
	uint8_t * temp_key = key;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.KEY =  *(temp_key++);
	}

	/* Load dummy data into LEA state memory. */
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  0x00;
	}

	/* Set LEA in encryption mode and start LEA. */
	LEA.CTRL = (LEA.CTRL & (~LEA_DECRYPT_bm)) | LEA_START_bm;


	do{
		/* Wait until LEA is finished or an error occurs. */
	}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

	/* If not error. */
	if((LEA.STATUS & LEA_ERROR_bm) == 0){
		/* Store the last subkey. */
		uint8_t * temp_last_sub_key = last_sub_key;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_last_sub_key++) = LEA.KEY;
		}
		LEA.STATUS = LEA_SRIF_bm;
		keygen_ok = true;
	}else{
		LEA.STATUS = LEA_ERROR_bm;
		keygen_ok = false;

	}
	return keygen_ok;
}



/*! \brief  Polled function that does LEA CBC encryption on a given number of
 *           128-bit data block.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  plaintext    Pointer to the plaintext that shall be encrypted.
 *  \param  ciphertext   Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  key          Pointer to the key.
 *  \param  init	        Pointer to the initialization vector used in the CBC.
 *  \param  block_count  The number of blocks to encrypt.
 *
 *	\retval true:	The LEA CBC encryption was successful.
 *  \retval false:  The LEA CBC encryption was not successful.
 */
bool LEA_CBC_encrypt(uint8_t * plaintext, uint8_t * ciphertext,
                     uint8_t * key, uint8_t * init, uint16_t block_count)
{
  	bool CBC_ok = true;

  	/* The first encryption uses the initialization vector. */
	uint8_t * temp_init = init;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  *(temp_init++);
	}

	/* Set LEA in encryption mode and enables the XOR feature and the AUTO start
	 * mode. */
	LEA.CTRL = (LEA.CTRL & (~LEA_DECRYPT_bm))| LEA_XOR_bm |LEA_AUTO_bm;

	/* Temporary values used to reduce memory access. */
	uint8_t * temp_plaintext = plaintext;
	uint8_t * temp_ciphertext = ciphertext;

	for(uint8_t blocks_left = block_count; blocks_left > 0; blocks_left--){

		/* Load key into LEA key memory. */
		uint8_t * temp_key = key;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.KEY =  *(temp_key++);
		}

		/* Load plaintext into LEA state memory. Auto starts. */
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.STATE =  *(temp_plaintext++);
		}


		do{
			/* Wait until LEA is finished or an error occurs. */
		}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

		/* If not error. */
		if((LEA.STATUS & LEA_ERROR_bm) == 0){

			/* Store result. */
			uint8_t * temp = temp_ciphertext;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				*(temp++) = LEA.STATE;
			}
			temp_ciphertext = temp;
		}else{
			CBC_ok = false;
		}
	}

	/* Turn off auto mode and xor feature. */
	LEA.CTRL = (LEA.CTRL & ~( LEA_XOR_bm |LEA_AUTO_bm));

	return CBC_ok;
}



/*! \brief  Polled function that does LEA CBC decryption on a given number of
 *           128-bit data block.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  ciphertext   Pointer to the ciphertext that shall be decrypted.
 *  \param  plaintext    Pointer to where the plaintext (answer) shall be stored.
 *  \param  key          Pointer to the last subkey of the Expanded Key.
 *  \param  init	        Pointer to the initialization vector used in the CBC.
 *  \param  block_count  The number of blocks to decrypt.
 *
 *  \retval true   If the LEA CBC decryption was successful.
 *  \retval false  If the LEA CBC decryption was not successful.
 */
bool LEA_CBC_decrypt(uint8_t * ciphertext, uint8_t * plaintext,
                     uint8_t * key, uint8_t * init, uint16_t block_count)
{
	bool CBC_ok = true;

	/* Temporary values used to reduce memory access. */
  	uint8_t * temp_plaintext = plaintext;
	uint8_t * temp_ciphertext = ciphertext;

	for(uint8_t blocks_left = block_count; blocks_left > 0; blocks_left--){

	  	/* Load key into LEA key memory. */
		uint8_t * temp_key = key;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.KEY =  *(temp_key++);
		}

		/* Load ciphertext into LEA state memory. */
		uint8_t * temp = temp_ciphertext;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			LEA.STATE =  *(temp++);
		}

		temp_ciphertext = temp;

		/* Set LEA in decryption mode and enable xor feature and start the LEA. */
		LEA.CTRL |= (LEA_DECRYPT_bm | LEA_XOR_bm | LEA_START_bm);

		do{
			/* Wait until LEA is finished or an error occurs. */
		}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

				/* If not error. */
		if((LEA.STATUS & LEA_ERROR_bm) == 0){

			/* The first block is xored with the initialization vector. */
			if(blocks_left == block_count){
				/* Load into LEA state memory. */
				uint8_t * temp_init = init;
				for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
					LEA.STATE =  *(temp_init++);
				}
			}
			/* The other blocks is xored with the previous ciphertext block. */
			else{
				/* Load into LEA state memory. */
			  	uint8_t * last_ciphertext = temp_ciphertext - (LEA_BLOCK_LENGTH*2);
				for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
					LEA.STATE =  *(last_ciphertext++);
				}
			}

			/* Disable XOR feature before next round. */
			LEA.CTRL = LEA.CTRL & (~LEA_XOR_bm);

			/* Store the result. */
			uint8_t * temp = temp_plaintext;
			for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
				*(temp++) = LEA.STATE;
			}
			temp_plaintext = temp;
		}else{
			CBC_ok = false;
		}

	}
	return CBC_ok;
}



/*! \brief  Function that sets LEA interrupt level
 *
 *  \param  int_lvl  The LEA interrupt level
 */
void LEA_interruptlevel_set(LEA_INTLVL_t int_lvl)
{
	LEA.INTCTRL = int_lvl;
}




/*! \brief  Polled function that does an LEA encryption on one 128-bit data block.
 *
 *  Function equal to the LEA_encrypt function but the key is not loaded
 *  into the key memory. The function require that the key already is in the
 *  key memory. Used when encryption and decryption with the same key is
 *  done every other time.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  plaintext  Pointer to the plaintext that shall be encrypted
 *  \param  ciphertext Pointer to where in memory the ciphertext (answer) shall be stored.
 *
 *  \retval true   If the LEA encryption was successful.
 *  \retval false  If the LEA encryption was not successful.
 */
bool LEA_encrypt_backtoback(uint8_t * plaintext, uint8_t * ciphertext)
{
  	bool encrypt_ok;

	/* Load data into LEA state memory. */
	uint8_t * temp_plaintext = plaintext;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  *(temp_plaintext++);
	}

	/* Set LEA in encryption mode and start LEA. */
	LEA.CTRL = (LEA.CTRL & (~LEA_DECRYPT_bm)) | LEA_START_bm;


	do{
		/* Wait until LEA is finished or an error occurs. */
	}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

	/* If not error. */
	if((LEA.STATUS & LEA_ERROR_bm) == 0){
		/* Store the result. */
		uint8_t * temp_ciphertext = ciphertext;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_ciphertext++) = LEA.STATE;
		}
		encrypt_ok = true;
	}else{
		encrypt_ok = false;

	}
	return encrypt_ok;
}



/*! \brief  Polled function that does an LEA decryption on one 128-bit data block.
 *
 *  Function equal to the LEA_decrypt function but the key is not loaded
 *  into the key memory. The function require that the key already is in the
 *  key memory. Used when encryption and decryption with the same key is
 *  done every other time.
 *
 *  \note This code is blocking and will dead lock if no interrupt flags are set.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *
 *  \retval true   If the LEA decryption was successful.
 *  \retval false  If the LEA decryption was not successful.
 */
bool LEA_decrypt_backtoback(uint8_t * ciphertext, uint8_t * plaintext)
{
  	bool decrypt_ok;

	/* Load data into LEA state memory. */
	uint8_t * temp_ciphertext = ciphertext;
	for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
		LEA.STATE =  *(temp_ciphertext++);
	}

	/* Set LEA in decryption mode and start the LEA.*/
	LEA.CTRL |= (LEA_START_bm | LEA_DECRYPT_bm);


	do{
		/* Wait until LEA is finished or an error occurs. */
	}while((LEA.STATUS & (LEA_SRIF_bm|LEA_ERROR_bm) ) == 0);

	/* If not error. */
	if((LEA.STATUS & LEA_ERROR_bm) == 0){
		/* Store the result. */
		uint8_t * temp_plaintext = plaintext;
		for(uint8_t i = 0; i < LEA_BLOCK_LENGTH; i++){
			*(temp_plaintext++) = LEA.STATE;
		}
		decrypt_ok = true;
	}else{
		decrypt_ok = false;

	}
	return decrypt_ok;
}

#endif

