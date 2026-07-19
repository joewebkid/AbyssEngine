

#include <cstdio>

// Blowfish substitution boxes: 4 boxes of 256 32-bit entries (4096 bytes, .bss).
// Emitted as the bare C-linkage symbol "S" to match the original binary.
extern "C" unsigned int S[4][256];
unsigned int S[4][256];

// Blowfish P-array: 18 32-bit subkeys (72 bytes, .bss).
// Emitted as the bare C-linkage symbol "P" to match the original binary.
extern "C" unsigned int P[18];
unsigned int P[18];

// The subkey file ("Blowfish.dat") handle; the original exports it as the bare
// symbol "SubkeyFile". opensubkeyfile() opens it; InitializeBlowfish() reads it.
extern "C" std::FILE *SubkeyFile;
std::FILE *SubkeyFile = nullptr;

// Blowfish Feistel function (defined alongside the input code).
extern "C" unsigned int F(unsigned int value);

extern "C" int opensubkeyfile() {
    SubkeyFile = std::fopen("Blowfish.dat", "rb");
    return SubkeyFile != nullptr ? 0 : -1;
}

extern "C" void *decrypt(void *ctx) {
    return ctx;
}

extern "C" void Blowfish_encipher(unsigned int *xl, unsigned int *xr) {
    unsigned int Xl = *xl;
    unsigned int Xr = *xr;
    for (int i = 0; i != 16; ++i) {
        unsigned int t = P[i] ^ Xl;
        Xl = F(t) ^ Xr;
        Xr = t;
    }
    *xl = P[17] ^ Xr;
    *xr = Xl ^ P[16];
}

extern "C" void Blowfish_decipher(unsigned int *xl, unsigned int *xr) {
    unsigned int Xl = *xl;
    unsigned int Xr = *xr;
    for (unsigned int i = 17; i > 1; --i) {
        unsigned int t = P[i] ^ Xl;
        Xl = F(t) ^ Xr;
        Xr = t;
    }
    *xl = P[0] ^ Xr;
    *xr = Xl ^ P[1];
}

extern "C" void InitializeBlowfish(unsigned char *key, int keylen) {
    if (opensubkeyfile() != 0) {
        std::printf("InitializeBlowfish: cannot open subkey file\n");
        return;
    }

    // Load the precomputed P-array and S-boxes from the subkey file.
    for (unsigned int i = 0; i < 18; ++i) {
        if (std::fread(&P[i], 4, 1, SubkeyFile) != 1)
            return;
    }
    for (unsigned int box = 0; box < 4; ++box) {
        for (unsigned int j = 0; j < 256; ++j) {
            if (std::fread(&S[box][j], 4, 1, SubkeyFile) != 1)
                return;
        }
    }
    std::fclose(SubkeyFile);

    // XOR the P-array with the (cyclically repeated) key.
    short k = 0;
    for (int i = 0; i != 18; ++i) {
        unsigned int data = 0;
        for (unsigned short b = 0; b < 4; ++b) {
            int idx = k;
            ++k;
            if (keylen <= k)
                k = 0;
            data = (data << 8) | key[idx];
        }
        P[i] ^= data;
    }

    // Run the key schedule: encrypt the all-zero block repeatedly, filling the
    // P-array and then each S-box two words at a time.
    unsigned int dl = 0;
    unsigned int dr = 0;
    for (int i = 0; i < 18; i += 2) {
        Blowfish_encipher(&dl, &dr);
        P[i] = dl;
        P[i + 1] = dr;
    }
    for (int box = 0; box != 4; ++box) {
        for (int j = 0; j < 256; j += 2) {
            Blowfish_encipher(&dl, &dr);
            S[box][j] = dl;
            S[box][j + 1] = dr;
        }
    }
}
