/* CRYPTO_memcmp isolated from OpenSSL 1.0.2u crypto/cryptlib.c.
 *
 * Compiled as its own translation unit so the static archive pulls ONLY this
 * function when the game references it (the full cryptlib.o would also drag in
 * ~35 unrelated CRYPTO and OPENSSL symbols the original does not export). The
 * body below is the verbatim OpenSSL source. */
#include <stddef.h>

int CRYPTO_memcmp(const volatile void *in_a, const volatile void *in_b, size_t len)
{
    size_t i;
    const volatile unsigned char *a = in_a;
    const volatile unsigned char *b = in_b;
    unsigned char x = 0;

    for (i = 0; i < len; i++)
        x |= a[i] ^ b[i];

    return x;
}
