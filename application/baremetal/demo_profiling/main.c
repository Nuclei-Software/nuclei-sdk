// Modified based on https://github.com/dhuertas/AES
#include <stdio.h>

#include "aes.h"
#include "nuclei_sdk_soc.h"

int main(void)
{

	int i, j;

	/* 256 bit key */
	uint8_t key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
			0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
			0x1f };

	uint8_t in[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
			0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	uint8_t out[16]; // 128

	uint8_t *w; // expanded key

	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 16; j++) {
			in[j] = __RV_CSR_READ(CSR_MCYCLE) & 0xff;
		}

		switch (i & 0x3) {
		case 0:
			w = aes_init(16);
			break;
		case 1:
			w = aes_init(24);
			break;
		default:
			w = aes_init(32);
			break;
		}

		aes_key_expansion(key, w);

#if DEBUG
		printf("Plaintext message:\n");
		for (i = 0; i < 4; i++) {
			printf("%02x %02x %02x %02x ", in[4*i+0], in[4*i+1], in[4*i+2], in[4*i+3]);
		}

		printf("\n");
#endif
		aes_cipher(in /* in */, out /* out */, w /* expanded key */);

#if DEBUG
		printf("Ciphered message:\n");
		for (i = 0; i < 4; i++) {
			printf("%02x %02x %02x %02x ", out[4*i+0], out[4*i+1], out[4*i+2], out[4*i+3]);
		}

		printf("\n");
#endif

		aes_inv_cipher(out, in, w);

#if DEBUG
		printf("Original message (after inv cipher):\n");
		for (i = 0; i < 4; i++) {
			printf("%02x %02x %02x %02x ", in[4*i+0], in[4*i+1], in[4*i+2], in[4*i+3]);
		}

		printf("\n");
#endif
		free(w);
	}

	// TODO this is used for collect gprof and gcov data
	extern long gprof_collect(unsigned long interface);
	extern long gcov_collect(unsigned long interface);
	gprof_collect(2);
	gcov_collect(2);
	return 0;
}
