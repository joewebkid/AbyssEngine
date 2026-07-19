#include "engine/render/IParticleSystem.h"

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(void *self, int max);
    }
}

char *MatrixGetPosition(char *out, Matrix const *matrix);

char *MatrixGetRight(char *out, Matrix const *matrix);

char *MatrixGetUp(char *out, Matrix const *matrix);

char *MatrixGetDir(char *out, Matrix const *matrix);

namespace AbyssEngine {
    namespace AEMath {
        Vector operator-(const Vector &);

        Vector operator-(const Vector &, const Vector &);

        Vector operator+(const Vector &, const Vector &);

        Vector operator*(const Vector &, float);

        Vector operator*(float, const Vector &);

        float VectorDot(const Vector &, const Vector &);
    }
}

void *AERandom_seed_ctor(void *self, long long seed);

void AERandom_ctor(void *self);


static char *ParticleSet_definitions = nullptr;

void IParticleSystem::enableUpdate(bool enabled) {
    this->updateEnabled = enabled;
}

int IParticleSystem::getParticleCount() {
    return this->maxParticles;
}

void IParticleSystem::setParticleSet(ParticleSettings::ParticleSet set) {
    if (!this->particleSets->empty() && (*this->particleSets)[0] == set) {
        this->particleSetIndex = 0;
    }
}

void IParticleSystem::enableEmit(bool enabled) {
    if (enabled && this->emitEnabled == 0) {
        this->emitTimer = 0;
    }
    this->emitEnabled = enabled;
}

void IParticleSystem::update(int delta) {
    if (this->updateEnabled != 0) {
        float fdelta = (float) delta;
        for (int i = 0; i < this->maxParticles; ++i) {
            if (this->particleAges[i] != -1) {
                this->updateSingle(i, fdelta);
            }
        }
    }
}

void IParticleSystem::setParticleSetIndex(uint8_t index) {
    this->particleSetIndex = index;
}

void IParticleSystem::setMatrix(Matrix const *matrix) {
    *(Matrix const * volatile *) &this->matrix = matrix;
    this->field_0x4 = 0x100;
}

void IParticleSystem::enableRender(bool enabled) {
    if (!enabled && this->renderEnabled != 0) {
        this->reset();
    }
    this->renderEnabled = enabled;
}

static inline int float_bits(float v) {
    union {
        float f;
        int i;
    } u;
    u.f = v;
    return u.i;
}

static inline float bits_float(int v) {
    union {
        float f;
        int i;
    } u;
    u.i = v;
    return u.f;
}

static inline void zero_vec(char *v) {
    *(uint32_t *) (v + 0) = 0;
    *(uint32_t *) (v + 4) = 0;
    *(uint32_t *) (v + 8) = 0;
}

void IParticleSystem::emit(int delta) {
    if (this->emitEnabled == 0 || this->renderEnabled == 0) {
        return;
    }

    int set = (*this->particleSets)[this->particleSetIndex];
    if (set == -1) {
        return;
    }

    uint32_t flags = this->flags;
    if ((flags & 0x80) == 0) {
        if ((flags & 0x100) != 0) {
            return;
        }
    } else if ((flags & 0x100) != 0 || this->particleAges[0] != -1) {
        return;
    }

    char matrixPos[12];
    char right[12];
    char up[12];
    char dir[12];
    char tmp[12];
    char tmp2[12];
    char travel[12];
    char travelDiv[12];
    char baseDelta[12];
    char uv[16];
    char rotated[16];
    char particlePos[12];
    char velocity[12];
    char emitVelocity[12];

    MatrixGetPosition(matrixPos, this->matrix);
    MatrixGetRight(right, this->matrix);
    if (this->mirror != 0) {
        *(Vector *) tmp = -*(const Vector *) right;
        *(Vector *) (right) = *(Vector *) (tmp);
    }
    MatrixGetUp(up, this->matrix);
    MatrixGetDir(dir, this->matrix);

    char *def = ParticleSet_definitions + (set + set * 4) * 32;
    float speed2 = AbyssEngine::AEMath::VectorDot(this->emitterVelocity, this->emitterVelocity);
    if (speed2 < (float) *(int *) (def + 0x98)) {
        return;
    }

    float fdelta = (float) delta;
    float elapsed = this->emitTimer + fdelta;
    *(Vector *) travel = this->emitterVelocity * elapsed;
    *(Vector *) travelDiv = *(const Vector *) travel;
    *(Vector *) travelDiv /= 1000.0f;
    float travelLen2 = AbyssEngine::AEMath::VectorDot(*(const Vector *) travelDiv, *(const Vector *) travelDiv);
    float invGuess = bits_float(0x5f3759df - (float_bits(travelLen2) >> 1));
    float invLen = (travelLen2 * -0.5f * invGuess * invGuess + 1.5f) * invGuess;
    float distance = 1.0f / invLen;

    int emitCount;
    if ((flags & 0x10) != 0) {
        float countf = distance / *(float *) (def + 0x2c);
        emitCount = (int) countf;
        this->emitTimer = (elapsed * (countf - (float) emitCount)) / countf;
    } else if ((flags & 0x20) != 0) {
        int cycles = (int) (*(float *) (def + 0x2c) * elapsed * 0.001f);
        this->emitTimer = elapsed + ((float) cycles * -1000.0f) / *(float *) (def + 0x2c);
        emitCount = cycles;
    } else {
        emitCount = *(int *) (def + 0x10);
        if ((flags & 0x40) != 0) {
            this->emitEnabled = 0;
        }
    }

    if (emitCount <= 0) {
        return;
    }

    *(Vector *) baseDelta = *(const Vector *) matrixPos - *(const Vector *) travelDiv;
    float spreadScale = 0.0f;
    float pathScale = 0.0f;
    if ((this->flags & 0xc0) == 0) {
        float y = bits_float(0x5f3759df - (float_bits(speed2) >> 1));
        pathScale = (speed2 * -0.5f * y * y + 1.5f) * y;
    }

    ((uint32_t *) uv)[0] = *(uint32_t *) (def + 0x88);
    ((uint32_t *) uv)[1] = *(uint32_t *) (def + 0x90);
    ((uint32_t *) uv)[2] = *(uint32_t *) (def + 0x8c);
    ((uint32_t *) uv)[3] = *(uint32_t *) (def + 0x94);

    uint32_t *uvp = (uint32_t *) uv;
    for (int i = 0; i < emitCount; ++i) {
        int current = this->currentParticle;
        this->particleSetIds[current] = (int8_t) set;
        this->particleAges[current] = 0;
        if (((this->flags >> 24) & 0x80) != 0) {
            uvp = (uint32_t *) rotateUVs((float *) uv, current, (float *) rotated);
        }

        int velSpread = *(int *) (def + 0x50);
        if (velSpread == 0) {
            zero_vec(velocity);
        } else {
            int range = velSpread << 1;
            ((float *) velocity)[0] = *(float *) (def + 0x58) +
                                      (float) (AbyssEngine::AERandom::nextInt(this->random, range) - velSpread);
            ((float *) velocity)[1] = *(float *) (def + 0x5c) +
                                      (float) (AbyssEngine::AERandom::nextInt(this->random, range) - velSpread);
            ((float *) velocity)[2] = *(float *) (def + 0x60) +
                                      (float) (AbyssEngine::AERandom::nextInt(this->random, range) - velSpread);
        }

        Vector &slot = this->particleVelocities[current];
        slot = *(Vector *) (velocity);

        float drag = *(float *) (def + 0x64);
        if (drag != 0.0f) {
            *(Vector *) velocity = this->emitterVelocity * drag;
            slot -= *(Vector *) (velocity);
        }
        if (*(float *) (def + 0x68) != 0.0f) {
            *(Vector *) velocity = *(const Vector *) right * *(float *) (def + 0x68);
            slot += *(Vector *) (velocity);
        }
        if (*(float *) (def + 0x6c) != 0.0f) {
            *(Vector *) velocity = *(const Vector *) up * *(float *) (def + 0x6c);
            slot += *(Vector *) (velocity);
        }
        if (*(float *) (def + 0x70) != 0.0f) {
            *(Vector *) velocity = *(const Vector *) dir * *(float *) (def + 0x70);
            slot += *(Vector *) (velocity);
        }

        float phase;
        if (*(int *) (def + 0x30) == 1) {
            phase = (float) (i + 1);
        } else {
            phase = (float) i + (float) AbyssEngine::AERandom::nextInt(this->random, 10000) * 0.0001f;
        }

        zero_vec(particlePos);
        if ((this->flags & 0xc0) == 0) {
            if (distance >= 1.0f) {
                float step = ((this->flags & 0x10) != 0)
                                 ? *(float *) (def + 0x2c)
                                 : distance / (float) emitCount;
                *(Vector *) tmp = *(const Vector *) travelDiv * (phase * step);
                *(Vector *) tmp2 = *(const Vector *) tmp * pathScale;
                *(Vector *) (particlePos) = *(Vector *) (tmp2);
                *(Vector *) tmp2 = *(const Vector *) baseDelta + *(const Vector *) particlePos;
                *(Vector *) (particlePos) = *(Vector *) (tmp2);
            } else {
                *(Vector *) (particlePos) = *(Vector *) (matrixPos);
                phase = (float) (i + 1);
                emitCount = i + 1;
            }
        } else {
            *(Vector *) (particlePos) = *(Vector *) (matrixPos);
            phase = 0.0f;
        }

        if ((this->flags & 0x80) != 0) {
            int posRange = (int) *(float *) (def + 0x78);
            int range = posRange << 1;
            ((float *) tmp)[0] = (float) (AbyssEngine::AERandom::nextInt(this->random, range) - posRange);
            ((float *) tmp)[1] = (float) (AbyssEngine::AERandom::nextInt(this->random, range) - posRange);
            ((float *) tmp)[2] = (float) (AbyssEngine::AERandom::nextInt(this->random, range) - posRange);
            *(Vector *) (particlePos) += *(Vector *) (tmp);
        } else {
            if (*(float *) (def + 0x78) != 0.0f) {
                *(Vector *) tmp = *(const Vector *) right * *(float *) (def + 0x78);
                *(Vector *) (particlePos) += *(Vector *) (tmp);
            }
            if (*(float *) (def + 0x7c) != 0.0f) {
                *(Vector *) tmp = *(const Vector *) up * *(float *) (def + 0x7c);
                *(Vector *) (particlePos) += *(Vector *) (tmp);
            }
            if (*(float *) (def + 0x80) != 0.0f) {
                *(Vector *) tmp = *(const Vector *) dir * *(float *) (def + 0x80);
                *(Vector *) (particlePos) += *(Vector *) (tmp);
            }
            if (*(float *) (def + 0x84) != 0.0f) {
                *(Vector *) tmp = *(const Vector *) dir * (float) AbyssEngine::AERandom::nextInt(
                                      this->random, (int) *(float *) (def + 0x84));
                *(Vector *) (particlePos) += *(Vector *) (tmp);
            }
            int posSpread = *(int *) (def + 0x48);
            if (posSpread != 0) {
                ((float *) tmp)[0] = (float) (AbyssEngine::AERandom::nextInt(this->random, posSpread << 1) - posSpread);
                ((float *) tmp)[1] = 0.0f;
                ((float *) tmp)[2] = (float) (AbyssEngine::AERandom::nextInt(this->random, posSpread << 1) - posSpread);
                *(Vector *) (particlePos) += *(Vector *) (tmp);
            }
            int ySpread = *(int *) (def + 0x4c);
            if (ySpread != 0) {
                ((float *) particlePos)[1] +=
                        (float) (AbyssEngine::AERandom::nextInt(this->random, ySpread << 1) - ySpread);
            }
        }

        float life = *(float *) (def + 0x14);
        float size0 = *(float *) (def + 0x1c);
        float size1 = *(float *) (def + 0x20);
        int randomLife = *(int *) (def + 0x18);
        if (randomLife != 0) {
            life += (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
            size0 += (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
            size1 += (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
        }

        if (*(float *) (def + 0x24) == 0.0f) {
            zero_vec(emitVelocity);
        } else {
            *(Vector *) emitVelocity = *(float *) (def + 0x24) * slot;
        }

        int colorFlag;
        if (*(int *) (def + 0x3c) > 0) {
            colorFlag = 1;
        } else {
            colorFlag = (*(float *) (def + 0x40) > 0.0f) ? 1 : 0;
        }

        this->setParticle(*(const Vector *) particlePos, life, *(uint32_t *) (def + 0x34),
                          bits_float(uvp[0]), bits_float(uvp[2]), bits_float(uvp[1]),
                          bits_float(uvp[3]), colorFlag != 0, size0, size1,
                          *(const Vector *) emitVelocity);

        if (*(float *) (def + 0x64) != 0.0f) {
            *(Vector *) tmp = this->emitterVelocity * *(float *) (def + 0x64);
            *(Vector *) tmp2 = *(const Vector *) tmp * 2.0f;
            slot += *(Vector *) (tmp2);
        }

        float remaining = pathScale * invLen * ((float) emitCount - phase) * 1000.0f;
        if (remaining > fdelta) {
            remaining = fdelta;
        }
        this->updateSingle(current, remaining);

        current = this->currentParticle + 1;
        if (this->maxParticles <= current) {
            current = 0;
        }
        this->currentParticle = current;
    }
}

void IParticleSystem::interpolateColor(int index, float &alpha, float &red, float &green, float &blue) {
    int age = this->particleAges[index];
    int setIndex = this->particleSetIds[index];
    char *def = ParticleSet_definitions + (setIndex + setIndex * 4) * 32;

    float t = (float) age / (float) *(int *) (def + 0x28);
    if (t > 1.0f) {
        t = 1.0f;
    }
    float inv = 1.0f - t;
    uint32_t c0 = *(uint32_t *) (def + 0x34);
    uint32_t c1 = *(uint32_t *) (def + 0x38);

    float a0 = (float) (c0 >> 24);
    float r0 = (float) ((c0 >> 16) & 0xff);
    float g0 = (float) ((c0 >> 8) & 0xff);
    float b0 = (float) (c0 & 0xff);
    float a1 = (float) (c1 >> 24);
    float r1 = (float) ((c1 >> 16) & 0xff);
    float g1 = (float) ((c1 >> 8) & 0xff);
    float b1 = (float) (c1 & 0xff);

    const float scale = 0.003921568859368563f;
    alpha = (inv * a0 + t * a1) * scale;
    red = (inv * r0 + t * r1) * scale;
    green = (inv * g0 + t * g1) * scale;
    blue = (inv * b0 + t * b1) * scale;

    int fadeFrames = *(int *) (def + 0x3c);
    if (age < fadeFrames) {
        float fade = (float) age / (float) fadeFrames;
        if (this->alphaFade != 0) {
            alpha *= fade;
            red *= fade;
            green *= fade;
        } else {
            blue *= fade;
        }
    }
}

struct LocalRandom {
    char data[12];
};

float *IParticleSystem::rotateUVs(float *src, int seed, float *dst) {
    LocalRandom random;
    AERandom_seed_ctor(&random, (long long) seed);
    unsigned value = (unsigned) AbyssEngine::AERandom::nextInt(&random, 40000);
    unsigned inv = ~value;
    ((uint32_t *) dst)[0] = ((uint32_t *) src)[value & 1];
    ((uint32_t *) dst)[1] = ((uint32_t *) src)[inv & 1];
    ((uint32_t *) dst)[2] = *(uint32_t *) ((char *) src + (((value & 2) << 1) | 8));
    ((uint32_t *) dst)[3] = *(uint32_t *) ((char *) src + (((inv & 2) << 1) | 8));
    AERandom_dtor(&random);
    return dst;
}

IParticleSystem::IParticleSystem(PaintCanvas *canvas, Matrix const *matrix,
                                 Array<ParticleSettings::ParticleSet> const &sets,
                                 bool mirror, bool alphaFade) {
    this->canvas = canvas;
    AERandom_ctor(this->random);

    this->emitterVelocity.x = 0.0f;
    this->emitterVelocity.y = 0.0f;
    this->emitterVelocity.z = 0.0f;
    this->lastEmitterPosition.x = 0.0f;
    this->matrix = matrix;
    this->field_0x2c = 0;
    this->field_0x30 = 0;

    this->particleSets = new Array<ParticleSettings::ParticleSet>();
    this->mirror = mirror;
    this->alphaFade = alphaFade;
    *this->particleSets = sets;

    this->currentParticle = 0;
    this->field_0x54 = -1;
    this->field_0x58 = -1;
    this->emitEnabled = 1;
    this->renderEnabled = 1;
    this->updateEnabled = 1;
    this->maxParticles = 0;
    this->flags = 0;

    int count = sets.size();
    const ParticleSettings::ParticleSet *src = sets.data();
    int firstFlags = 0;
    uint32_t maxParticles = 0;
    while (count != 0) {
        int set = *src;
        if (set != -1) {
            char *def = ParticleSet_definitions + (set + set * 4) * 32;
            uint32_t particles = *(uint32_t *) (def + 0x10);
            if ((int) maxParticles <= (int) particles) {
                maxParticles = particles;
            }
            this->maxParticles = maxParticles;
            if (firstFlags == 0) {
                firstFlags = *(int *) (def + 0xc);
                this->flags = firstFlags;
            }
        }
        ++src;
        --count;
    }

    this->emitTimer = 0;
    this->particleSetIndex = 0;
    this->particleAges = new int[maxParticles];
    this->particleSetIds = new int8_t[maxParticles];

    for (int i = 0; i < (int) maxParticles; ++i) {
        this->particleSetIds[i] = (int8_t) 200;
        maxParticles = this->maxParticles;
    }

    this->field_0x5c = 0;
    this->field_0x4 = 0x101;
}

void IParticleSystem::calcEmitterVelocity(int delta) {
    char position[12];
    char scaled[12];
    char diff[12];
    MatrixGetPosition(position, this->matrix);
    *(Vector *) diff = *(const Vector *) position - this->lastEmitterPosition;
    *(Vector *) scaled = *(const Vector *) diff * (1000.0f / (float) delta);
    this->emitterVelocity = *(Vector *) (scaled);
    this->emitterVelocityDirty = 0;
    this->lastEmitterPosition = *(Vector *) (position);
}

void IParticleSystem::emitManual(Vector position, int particleSet, Vector const *velocity, float lifetime) {
    if (particleSet != -1) {
        int current = this->currentParticle;
        int set = (*this->particleSets)[particleSet];
        this->particleSetIds[current] = (int8_t) set;
        char *def = ParticleSet_definitions + (set + set * 4) * 32;
        this->particleAges[current] = 0;

        uint32_t uv[4];
        uint32_t rotated[4];
        uv[0] = *(uint32_t *) (def + 0x88);
        uv[1] = *(uint32_t *) (def + 0x90);
        uv[2] = *(uint32_t *) (def + 0x8c);
        uv[3] = *(uint32_t *) (def + 0x94);
        uint32_t *uvp = uv;
        if (((this->flags >> 24) & 0x80) != 0) {
            uvp = (uint32_t *) rotateUVs((float *) uv, current, (float *) rotated);
        }

        char randomVelocity[12];
        int spread = *(int *) (def + 0x50);
        if (spread == 0) {
            *(uint32_t *) (randomVelocity + 0) = 0;
            *(uint32_t *) (randomVelocity + 4) = 0;
            *(uint32_t *) (randomVelocity + 8) = 0;
        } else {
            int range = spread << 1;
            ((float *) randomVelocity)[0] = (float) (AbyssEngine::AERandom::nextInt(this->random, range) - spread);
            ((float *) randomVelocity)[1] = *(float *) (def + 0x5c) +
                                            (float) (AbyssEngine::AERandom::nextInt(this->random, range) - spread);
            ((float *) randomVelocity)[2] = (float) (AbyssEngine::AERandom::nextInt(this->random, range) - spread);
        }

        Vector &slot = this->particleVelocities[current];
        slot = *(Vector *) (randomVelocity);

        if (velocity != 0) {
            float drag = *(float *) (def + 0x64);
            if (drag != 0.0f) {
                char tmp[12];
                *(Vector *) tmp = *(const Vector *) velocity * drag;
                slot -= *(Vector *) (tmp);
            }
        }

        int posSpread = *(int *) (def + 0x48);
        if (posSpread != 0) {
            char randomPosition[12];
            ((float *) randomPosition)[0] =
                    (float) (AbyssEngine::AERandom::nextInt(this->random, posSpread << 1) - posSpread);
            ((float *) randomPosition)[1] = 0.0f;
            ((float *) randomPosition)[2] =
                    (float) (AbyssEngine::AERandom::nextInt(this->random, posSpread << 1) - posSpread);
            *(Vector *) (&position) += *(Vector *) (randomPosition);
        }

        int ySpread = *(int *) (def + 0x4c);
        if (ySpread != 0) {
            position.y += (float) (AbyssEngine::AERandom::nextInt(this->random, ySpread << 1) - ySpread);
        }

        if (lifetime < 0.0f) {
            lifetime = *(float *) (def + 0x14);
        }

        char emitVelocity[12];
        int randomLife = *(int *) (def + 0x18);
        if (randomLife == 0) {
            float velocityScale = *(float *) (def + 0x24);
            if (velocityScale == 0.0f) {
                *(uint32_t *) (emitVelocity + 0) = 0;
                *(uint32_t *) (emitVelocity + 4) = 0;
                *(uint32_t *) (emitVelocity + 8) = 0;
            } else {
                *(Vector *) emitVelocity = velocityScale * slot;
            }

            this->setParticle(position, lifetime, *(uint32_t *) (def + 0x34),
                              bits_float(uvp[0]), bits_float(uvp[2]), bits_float(uvp[1]),
                              bits_float(uvp[3]), *(int *) (def + 0x3c) > 0,
                              *(float *) (def + 0x1c), *(float *) (def + 0x20),
                              *(const Vector *) emitVelocity);
        } else {
            float life = lifetime + (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
            float size0 = *(float *) (def + 0x1c) + (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
            float size1 = *(float *) (def + 0x20) + (float) AbyssEngine::AERandom::nextInt(this->random, randomLife);
            float velocityScale = *(float *) (def + 0x24);
            if (velocityScale == 0.0f) {
                *(uint32_t *) (emitVelocity + 0) = 0;
                *(uint32_t *) (emitVelocity + 4) = 0;
                *(uint32_t *) (emitVelocity + 8) = 0;
            } else {
                *(Vector *) emitVelocity = velocityScale * slot;
            }

            this->setParticle(position, life, *(uint32_t *) (def + 0x34),
                              bits_float(uvp[0]), bits_float(uvp[2]), bits_float(uvp[1]),
                              bits_float(uvp[3]), *(int *) (def + 0x3c) > 0,
                              size0, size1, *(const Vector *) emitVelocity);
        }

        float drag = *(float *) (def + 0x64);
        if (drag != 0.0f) {
            char tmp[12];
            char tmp2[12];
            *(Vector *) tmp = this->emitterVelocity * drag;
            *(Vector *) tmp2 = *(const Vector *) tmp * 2.0f;
            slot += *(Vector *) (tmp2);
        }

        current = this->currentParticle + 1;
        if (this->maxParticles <= current) {
            current = 0;
        }
        this->currentParticle = current;
    }
}

void IParticleSystem::resetEmitterVelocity() {
    char value[12] = {};
    this->emitterVelocity = *(Vector *) (value);
    this->emitterVelocityDirty = 1;
    char *matrixValue = value;
    MatrixGetPosition(matrixValue, this->matrix);
    this->lastEmitterPosition = *(Vector *) (matrixValue);
    this->field_0x4 = 0;
}
