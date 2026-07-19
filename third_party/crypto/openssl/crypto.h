/* Vendored subset of OpenSSL 1.0.2 <openssl/crypto.h>.
 * Declares the libcrypto utility routines the game uses; the implementation is
 * built from the OpenSSL source by third_party/openssl/build.sh into
 * libcrypto_gof2.a. */
#ifndef GOF2_VENDORED_OPENSSL_CRYPTO_H
#define GOF2_VENDORED_OPENSSL_CRYPTO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constant-time memory comparison; returns 0 iff the two buffers are equal. */
int CRYPTO_memcmp(const volatile void *a, const volatile void *b, size_t len);

#ifdef __cplusplus
}
#endif

#endif
