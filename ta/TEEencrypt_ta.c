/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <TEEencrypt_ta.h>
#include <string.h>
#include <stdlib.h>
#define KEY_SIZE    26

const int root_key = 7;
/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result Caesar_encrypt(uint32_t param_types, TEE_Param params[4]) {
	char* pl = (char*)params[0].memref.buffer;
	int pl_len = strlen(pl);
	char encrypted[BUF_SIZE];
	char random_key;
	int k = 0;

	IMSG("PlainText: %s\n", pl);
	do {
		TEE_GenerateRandom((void*)(&random_key), sizeof(char));

		IMSG("Key before Convert: 0x%02x", random_key);
		random_key %= 26;
		IMSG("Converted: 0x%02x", random_key);
	} while(random_key == 0);
	strcpy(encrypted, pl);
	for (int i=0;i<pl_len;i++) {
		if (encrypted[i] >= 'A' && encrypted[i] <= 'Z') {
			encrypted[i] += random_key;
			if (encrypted[i] > 'Z') encrypted[i] -= 26;
		}
		else if(encrypted[i] >='a' && encrypted[i] <= 'z') {
			encrypted[i] += random_key;
			if (encrypted[i] > 'z') encrypted[i] -= 26;
		}
	}
	encrypted[pl_len] = '\0';
	IMSG("Cipher : %s", encrypted);
	strcpy(pl, encrypted);

	random_key += root_key;	// random_key encrypt by root_key.
	(*(char*)params[1].memref.buffer) = random_key;

	return TEE_SUCCESS;
}
static TEE_Result Caesar_decrypt(uint32_t param_types, TEE_Param params[4]) {
	char *pl = (char*)params[0].memref.buffer;
	int pl_len = strlen(pl);
	char k = *(char*)params[1].memref.buffer;
	char *decrypt_buf = NULL;

	k -= root_key;		// key decrypt.

	decrypt_buf = (char*)malloc(sizeof(char)*pl_len);
	strcpy(decrypt_buf, pl);
	for (int i=0;i<pl_len;i++) {
		if (decrypt_buf[i] >= 'A' && decrypt_buf[i] <= 'Z') {
			decrypt_buf[i] -= k;
			if (decrypt_buf[i] < 'A') decrypt_buf[i] += 26;
		}
		else if (decrypt_buf[i] >= 'a' && decrypt_buf[i] <= 'z') {
			decrypt_buf[i] -= k;
			if (decrypt_buf[i] < 'a') decrypt_buf[i] += 26;
		}
	}
	IMSG("CipherText: %s", pl);
	IMSG("DecryptedText: %s", decrypt_buf);
	strcpy(pl, decrypt_buf);
	free(decrypt_buf);

	return TEE_SUCCESS;
}
static TEE_Result inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a++;
	IMSG("Increase value to: %u", params[0].value.a);

	return TEE_SUCCESS;
}

static TEE_Result dec_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a--;
	IMSG("Decrease value to: %u", params[0].value.a);

	return TEE_SUCCESS;
}
/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_HELLO_WORLD_CMD_INC_VALUE:
		return inc_value(param_types, params);
	case TA_HELLO_WORLD_CMD_DEC_VALUE:
		return dec_value(param_types, params);
	case TA_TEEencrypt_CMD_CAESAR_ENC_VALUE:
		return Caesar_encrypt(param_types, params);
	case TA_TEEencrypt_CMD_CAESAR_DEC_VALUE:
		return Caesar_decrypt(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
