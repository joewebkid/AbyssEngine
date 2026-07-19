#ifndef GOF2_EASEINOUTMATRIX_H
#define GOF2_EASEINOUTMATRIX_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "engine/math/AEMath.h"

#include "engine/math/Vector.h"


namespace AbyssEngine {
    class EaseInOutMatrix {
    public:
        AEMath::Matrix m_min;
        Quaternion m_q0;
        AEMath::Vector m_minPos;
        Quaternion m_q1;
        AEMath::Vector m_posDelta;
        float m_t;
        AEMath::Matrix m_current;
        AEMath::Matrix m_max;
        float m_duration;

        EaseInOutMatrix();

        EaseInOutMatrix(AEMath::Matrix mn, AEMath::Matrix mx, int duration);

        ~EaseInOutMatrix();

        void SetRange(AEMath::Matrix mn, AEMath::Matrix mx);

        void SetDuration(int duration);

        void UpdateCurrentValue();

        void RunOut(float dt);

        void Increase(float dt);

        void Decrease(float dt);

        void SetToMinValue();

        void SetToMaxValue();

        AEMath::Matrix GetValue();

        AEMath::Matrix GetMinValue();

        AEMath::Matrix GetMaxValue();
    };

    Quaternion operator+(const Quaternion &lhs, const Quaternion &rhs);

    Quaternion operator-(const Quaternion &lhs, const Quaternion &rhs);

    Quaternion operator*(const Quaternion &lhs, float rhs);
}

#endif
