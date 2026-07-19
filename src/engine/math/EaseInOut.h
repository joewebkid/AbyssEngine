#ifndef GOF2_EASEINOUT_H
#define GOF2_EASEINOUT_H

namespace AbyssEngine {
    namespace AEMath {
        float Sinf(float value);
    }

    static const float PI = 3.1415927f;

    class EaseInOut {
    public:
        float m_min;
        float m_range;
        float m_t;
        float m_current;

        EaseInOut();

        EaseInOut(float minValue, float maxValue);

        void SetRange(float minValue, float maxValue);

        void SetToMinValue();

        void SetToMaxValue();

        float GetMaxValue();

        void Increase(float dt);

        void Decrease(float dt);

        void RunOut(float dt);

        void UpdateCurrentValue();

        float GetValue();

        float GetMinValue();
    };
}

#endif
