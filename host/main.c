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

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <hello_world_ta.h>

void printUsage() {
    printf("Usage: \n");
    printf("\tEncryption: %s -e [PlaintextFile]\n");
    printf("\tDecryption: %s -d [CipherTextFile] [KeyFile]\n");
}
int main(int argc, char* argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
	uint32_t err_origin;
    FILE* f, key, dest_cipher, dest_key;
    char buffer[BUF_SIZE];  // buffer.
    if ((strcmp(argv[1], "-e") == 0 && argc == 3) || (strcmp(argv[1], "-d") == 0 && argc == 4)) {
        goto MAIN;
    }
    printUsage();
    return 1;

MAIN:
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	// op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
	// 				 TEEC_NONE, TEEC_NONE);
	// op.params[0].value.a = 42;

	/*
	 * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
	 * called.
	 */
	// printf("Invoking TA to increment %d\n", op.params[0].value.a);
	// res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_INC_VALUE, &op,
	// 			 &err_origin);
	// if (res != TEEC_SUCCESS)
	// 	errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
	// 		res, err_origin);
	// printf("TA incremented value to %d\n", op.params[0].value.a);
    op.paramTypes = TEEC_PARAM_TYPES(
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

    if (strcmp(argv[1], "-e") == 0)     // Encrypt
    {
        f = fopen(argv[2], "r");
        fread(buffer, sizeof(char), BUF_SIZE, f);   // read file.

        memcpy(op.params[0].tmpref.buffer, buffer, BUF_SIZE);
        op.params[0].tmpref.len = BUF_SIZE;
        printf("Invoking TA to encrypt %s\n", buffer);
        res = TEEC_InvokeCommand(
            &sess, TA_HW_CMD_CAESAR_ENC_VALUE, &op, &err_origin);
        if (res != TEEC_SUCCESS) 
            errx(1, "TEEC_InvokeCommand failed");
        
        dest_cipher = fopen("ciphertext.txt", "wt");
        fwrite(op.params[0].tmpref.buffer, sizeof(char), op.params[0].tmpref.len, dest_cipher);
        dest_key = fopen("encryptedkey.txt", "wt");
        fwrite(op.params[1].tmpref.buffer, sizeof(char), op.params[1].tmpref.len, dest_key);
        fclose(f);
        fclose(dest_cipher);
        fclose(dest_key);
        printf("Encryption Success. Check \"ciphertext.txt\" and \"encryptedkey.txt\".\n");
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        f = fopen(argv[2], "r");
        key = fopen(argv[3], "r");
        fread(buffer, sizeof(char), BUF_SIZE, f);   // read file.
        memcpy(op.params[0].tmpref.buffer, buffer, BUF_SIZE);
        op.params[0].tmpref.len = BUF_SIZE;
        fread(buffer, sizeof(char), BUF_SIZE, key);
        memcpy(op.params[1].tmpref.buffer, buffer, BUF_SIZE);
        op.params[1].tmpref.len = BUF_SIZE;
        printf("Invoking TA to decrypt %s\n", op.params[0].tmpref.buffer);

        res = TEEC_InvokeCommand(
            &sess, TA_HW_CMD_CAESAR_DEC_VALUE, &op, &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed");
        
        dest_cipher = fopen("decryptedtext.txt", "w");
        fwrite(op.params[0].tmpref.buffer, sizeof(char), op.params[0].tmpref.len, dest_cipher);
        fclose(f);
        fclose(key);
        fclose(dest_cipher);
        printf("Decryption Success. Check \"decryptedtext.txt\" file.");
    }
	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
