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
#include <stdlib.h>
/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <TEEencrypt_ta.h>

void printUsage(char* filename) {
    printf("Usage: \n");
    printf("\tEncryption: %s -e [PlaintextFile]\n", filename);
    printf("\tDecryption: %s -d [CipherTextFile] [KeyFile]\n", filename);
}
int main(int argc, char* argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_TEEencrypt_UUID;
	uint32_t err_origin;
<<<<<<< HEAD
	FILE *f, *key, *dest_cipher, *dest_key;
	char val_key = 0;
	char buffer[BUF_SIZE] = {0, };  // buffer.
	char key_buffer[BUF_SIZE] = {0, };  // key buffer.

	if (argc < 3) goto PRINTUSAGE;
	if ((strcmp(argv[1], "-e") == 0 && argc == 3) || (strcmp(argv[1], "-d") == 0 && argc == 4)) {
		goto MAIN;
	}
PRINTUSAGE:
	printUsage(argv[0]);
	return 1;
=======
    FILE *f, *key, *dest_cipher, *dest_key;
    char val_key = 0;
    int i_key = 0;
    char buffer[BUF_SIZE] = {0, };  // buffer.

    if ((strcmp(argv[1], "-e") == 0 && argc == 3) || (strcmp(argv[1], "-d") == 0 && argc == 4)) {
        goto MAIN;
    }
    printUsage(argv[0]);
    return 1;
>>>>>>> c088fbf54dca329281b0a307a4f096b25f70d6cb

MAIN:
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	memset(&op, 0, sizeof(op));

<<<<<<< HEAD
	op.paramTypes = TEEC_PARAM_TYPES(
 		TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

	if (strcmp(argv[1], "-e") == 0)     // Encrypt
	{
		puts("Encrypt start");
		f = fopen(argv[2], "r");
		puts("File reading");
		fread(buffer, sizeof(char), BUF_SIZE, f);   // read file.
		if (buffer[strlen(buffer)-1] == '\n') {
			buffer[strlen(buffer)-1] = '\0';
		}
		printf("Readed: %s, Length: %d\n", buffer, strlen(buffer));
		op.params[0].tmpref.buffer = buffer;
//		memcpy(op.params[0].tmpref.buffer, buffer, BUF_SIZE);
		op.params[0].tmpref.size = BUF_SIZE;

		op.params[1].tmpref.buffer = &val_key;
		op.params[1].tmpref.size = sizeof(char);

		printf("Invoking TA to encrypt %s\n", op.params[0].tmpref.buffer);
		res = TEEC_InvokeCommand(
			&sess, TA_TEEencrypt_CMD_CAESAR_ENC_VALUE, &op, &err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed");
=======
    op.paramTypes = TEEC_PARAM_TYPES(
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

    if (strcmp(argv[1], "-e") == 0)     // Encrypt
    {
        puts("Encrypt start");
        f = fopen(argv[2], "r");
        puts("File reading");
        fread(buffer, sizeof(char), BUF_SIZE, f);   // read file.
        if (buffer[strlen(buffer)-1] == '\n') {
            buffer[strlen(buffer)-1] = '\0';
        }
        printf("Readed: %s, Length: %d\n", buffer, strlen(buffer));
        op.params[0].tmpref.buffer = buffer;
//        memcpy(op.params[0].tmpref.buffer, buffer, BUF_SIZE);
        op.params[0].tmpref.size = BUF_SIZE;

        op.params[1].tmpref.buffer = &val_key;
        op.params[1].tmpref.size = sizeof(char);

        printf("Invoking TA to encrypt %s\n", op.params[0].tmpref.buffer);
        res = TEEC_InvokeCommand(
            &sess, TA_TEEencrypt_CMD_CAESAR_ENC_VALUE, &op, &err_origin);
        if (res != TEEC_SUCCESS) 
            errx(1, "TEEC_InvokeCommand failed");

        dest_cipher = fopen("ciphertext.txt", "wt");
        fprintf(dest_cipher, "%s\n", buffer);
//        fwrite(op.params[0].tmpref.buffer, sizeof(char), op.params[0].tmpref.size, dest_cipher);
        dest_key = fopen("encryptedkey.txt", "wt");
        fprintf(dest_key, "%d\n", val_key);
//        fwrite(op.params[1].tmpref.buffer, sizeof(char), op.params[1].tmpref.size, dest_key);
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
        op.params[0].tmpref.size = BUF_SIZE;
        fread(buffer, sizeof(char), BUF_SIZE, key);
        memcpy(op.params[1].tmpref.buffer, buffer, BUF_SIZE);
        op.params[1].tmpref.size = sizeof(char);
        printf("Invoking TA to decrypt %s\n", op.params[0].tmpref.buffer);
>>>>>>> c088fbf54dca329281b0a307a4f096b25f70d6cb

		dest_cipher = fopen("ciphertext.txt", "wt");
		fprintf(dest_cipher, "%s\n", buffer);
//		fwrite(op.params[0].tmpref.buffer, sizeof(char), op.params[0].tmpref.size, dest_cipher);
		dest_key = fopen("encryptedkey.txt", "wt");
		fprintf(dest_key, "%d\n", val_key);
//		fwrite(op.params[1].tmpref.buffer, sizeof(char), op.params[1].tmpref.size, dest_key);
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
		op.params[0].tmpref.buffer = buffer;
//		memcpy(op.params[0].tmpref.buffer, buffer, BUF_SIZE);
		op.params[0].tmpref.size = BUF_SIZE;

		if (buffer[strlen(buffer)-1] == '\n') buffer[strlen(buffer)-1] = '\0';

		fread(key_buffer, sizeof(char), BUF_SIZE, key);
		val_key = (char)atoi(key_buffer);
		op.params[1].tmpref.buffer = &val_key;
//		memcpy(op.params[1].tmpref.buffer, buffer, BUF_SIZE);
		op.params[1].tmpref.size = sizeof(char);
		printf("Invoking TA to decrypt %s, key: %s\n", op.params[0].tmpref.buffer, key_buffer);

		res = TEEC_InvokeCommand(
			&sess, TA_TEEencrypt_CMD_CAESAR_DEC_VALUE, &op, &err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed");

		dest_cipher = fopen("decryptedtext.txt", "w");
		fprintf(dest_cipher, "%s\n", buffer);
//		fwrite(op.params[0].tmpref.buffer, sizeof(char), op.params[0].tmpref.size, dest_cipher);
		fclose(f);
		fclose(key);
		fclose(dest_cipher);
		printf("Decryption Success. Check \"decryptedtext.txt\" file.\n");
	}

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
