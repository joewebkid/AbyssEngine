#ifndef GOF2_PARTICLESETTINGSREF_H
#define GOF2_PARTICLESETTINGSREF_H

class ParticleSettingsRef {
public:
    static void initialize();

    // Static data members present in the original binary (defined for symbol parity).
    static int assertInit;
    static unsigned char cur[7680];
    static unsigned char init[7680];
};

#endif
