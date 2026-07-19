#include "engine/math/EaseInOutMatrix.h"
#include <algorithm>
#include <new>

namespace AbyssEngine {
    void EaseInOutMatrix::RunOut(float dt) {
        float target = 1.0f;
        float t = m_t;
        if (t > target) {
            t = t + (dt * -0.5f) / m_duration;
            m_t = t;
            if (t < target) {
                m_t = 1.0f;
            }
        } else if (t < target) {
            t = t + (dt * 0.5f) / m_duration;
            m_t = t;
            if (t > target) {
                m_t = 1.0f;
            }
        }
        UpdateCurrentValue();
    }

    AEMath::Matrix EaseInOutMatrix::GetValue() {
        return this->m_current;
    }

    EaseInOutMatrix::~EaseInOutMatrix() {
    }

    void EaseInOutMatrix::SetDuration(int duration) {
        m_duration = (float) duration;
    }

    AEMath::Matrix EaseInOutMatrix::GetMaxValue() {
        return this->m_max;
    }

    AEMath::Matrix EaseInOutMatrix::GetMinValue() {
        return this->m_min;
    }

    void EaseInOutMatrix::SetToMaxValue() {
        m_t = 1.25f;
        UpdateCurrentValue();
    }

    void EaseInOutMatrix::SetRange(AEMath::Matrix mn, AEMath::Matrix mx) {
        m_min = mn;
        m_max = mx;

        m_q0.Set(mn);

        AEMath::Vector minPos = AEMath::MatrixGetPosition(mn);
        this->m_minPos = minPos;

        Quaternion qMax(mx);
        m_q1 = qMax - m_q0;

        AEMath::Vector maxPos = AEMath::MatrixGetPosition(mx);
        this->m_posDelta = maxPos - minPos;

        this->m_t = 0.75f;
        UpdateCurrentValue();
    }

    void EaseInOutMatrix::UpdateCurrentValue() {
        AEMath::Matrix &current = this->m_current;

        if (this->m_t == 1.25f) {
            current = this->m_max;
            return;
        }

        static const float kSweep = 3.14159265f;
        float s = AEMath::Sinf(this->m_t * kSweep);
        float w = s * 0.5f + 0.5f;

        Quaternion blended = m_q1 * w + m_q0;
        blended.Convert(current);

        AEMath::Vector t = this->m_minPos + this->m_posDelta * w;
        AEMath::MatrixSetTranslation(current, t);
    }

    void EaseInOutMatrix::Increase(float dt) {
        float t = this->m_t + (dt * 0.5f) / this->m_duration;
        this->m_t = std::min(t, 1.25f);
        UpdateCurrentValue();
    }

    void EaseInOutMatrix::SetToMinValue() {
        this->m_t = 0.75f;
        UpdateCurrentValue();
    }

    void EaseInOutMatrix::Decrease(float dt) {
        float t = this->m_t + (dt * -0.5f) / this->m_duration;
        this->m_t = std::max(t, 0.75f);
        UpdateCurrentValue();
    }

    EaseInOutMatrix::EaseInOutMatrix(AEMath::Matrix mn, AEMath::Matrix mx, int duration) {
        this->m_minPos = AEMath::Vector{0, 0, 0};
        this->m_posDelta = AEMath::Vector{0, 0, 0};

        SetRange(mn, mx);
        this->m_duration = (float) duration;
    }

    EaseInOutMatrix::EaseInOutMatrix() {
        this->m_minPos = AEMath::Vector{0, 0, 0};
        this->m_posDelta = AEMath::Vector{0, 0, 0};

        AEMath::Matrix ident;
        AEMath::MatrixIdentity(ident);
        SetRange(ident, ident);
        this->m_duration = 0.0f;
    }
}

