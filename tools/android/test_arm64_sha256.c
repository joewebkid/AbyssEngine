#include <openssl/sha.h>
#include <openssl/crypto.h>

#include <stdio.h>

_Static_assert(sizeof(SHA256_CTX) <= 0x70, "RecordHandler SHA context is too small");

int main(void) {
    unsigned char context[0x70];
    unsigned char digest[SHA256_DIGEST_LENGTH];
    static const unsigned char expected[SHA256_DIGEST_LENGTH] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    if (!SHA256_Init((SHA256_CTX *)context) ||
        !SHA256_Update((SHA256_CTX *)context, "a", 1) ||
        !SHA256_Update((SHA256_CTX *)context, "bc", 2) ||
        !SHA256_Final(digest, (SHA256_CTX *)context)) {
        fprintf(stderr, "SHA256 streaming failed\n");
        return 1;
    }
    if (CRYPTO_memcmp(digest, expected, sizeof expected) != 0) {
        fprintf(stderr, "SHA256('abc') differs from the known vector\n");
        return 1;
    }
    digest[0] ^= 1;
    if (CRYPTO_memcmp(digest, expected, sizeof expected) == 0) {
        fprintf(stderr, "CRYPTO_memcmp accepted a changed digest\n");
        return 1;
    }
    printf("SHA256_CTX=%zu bytes; SHA256('abc') and CRYPTO_memcmp passed\n",
           sizeof(SHA256_CTX));
    return 0;
}
