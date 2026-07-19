#ifndef GOF2_AERANDOM_H
#define GOF2_AERANDOM_H
#include <cstdint>

namespace AbyssEngine {
    class AERandom {
    public:
        uint32_t seedLow;
        uint32_t seedHigh;

        AERandom();

        explicit AERandom(long long seed);

        ~AERandom();

        void setSeed(long long seed);

        void reset();

        uint32_t next(int bits);

        int nextInt();

        int nextInt(int bound);

        static AERandom *gRandom;
    };
}

using ::AbyssEngine::AERandom;

#endif
