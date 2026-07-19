#ifndef GOF2_THIRD_PARTY_OPENSSL_SHA_H
#define GOF2_THIRD_PARTY_OPENSSL_SHA_H

/*
 * Vendored subset of the OpenSSL libcrypto SHA-256 API.
 *
 * The original Galaxy on Fire 2 binary links against OpenSSL's libcrypto for
 * the record/save-game integrity hashing in RecordHandler. Only the three
 * streaming SHA-256 entry points are referenced (Init/Update/Final), so this
 * header declares just that minimal surface rather than vendoring the whole
 * <openssl/sha.h>. The context pointer is opaque here (the caller stack-
 * allocates a buffer large enough for the real SHA256_CTX); the signatures
 * match what libcrypto exports.
 */

#ifdef __cplusplus
extern "C" {
#endif

void SHA256_Init(void *c);
void SHA256_Update(void *c, const void *data, int n);
void SHA256_Final(unsigned char *md, void *c);

#ifdef __cplusplus
}
#endif

#endif /* GOF2_THIRD_PARTY_OPENSSL_SHA_H */
