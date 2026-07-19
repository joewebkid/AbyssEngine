#include "engine/math/EaseInOut.h"

namespace AbyssEngine {
    EaseInOut::EaseInOut() {
        m_min = 0.0f;
        m_range = 2.0f * PI;
        m_t = 1.5f * PI;
        UpdateCurrentValue();
    }

    EaseInOut::EaseInOut(float minValue, float maxValue) {
        SetRange(minValue, maxValue);
    }

    void EaseInOut::SetRange(float minValue, float maxValue) {
        m_t = 1.5f * PI;
        m_min = minValue;
        m_range = maxValue - minValue;
        UpdateCurrentValue();
    }

    void EaseInOut::SetToMinValue() {
        m_t = 1.5f * PI;
        UpdateCurrentValue();
    }

    void EaseInOut::SetToMaxValue() {
        m_t = 2.5f * PI;
        UpdateCurrentValue();
    }

    float EaseInOut::GetMaxValue() {
        return m_min + m_range;
    }

    void EaseInOut::Increase(float dt) {
        m_t = (float) ((double) m_t + (double) dt * (1.0 / 65536.0) * (2.0 * PI));
        if ((double) m_t > 2.5 * PI) {
            m_t = 2.5f * PI;
        }
        UpdateCurrentValue();
    }

    void EaseInOut::Decrease(float dt) {
        m_t = (float) ((double) m_t + (double) dt * (-1.0 / 65536.0) * (2.0 * PI));
        if ((double) m_t < 1.5 * PI) {
            m_t = 1.5f * PI;
        }
        UpdateCurrentValue();
    }

    void EaseInOut::RunOut(float dt) {
        float target = 2.0f * PI;
        float t = m_t;
        if (t > target) {
            t = (float) ((double) t + (double) dt * (-1.0 / 65536.0) * (2.0 * PI));
            m_t = t;
            if (t < target) {
                m_t = target;
            }
        } else if (t < target) {
            t = (float) ((double) t + (double) dt * (1.0 / 65536.0) * (2.0 * PI));
            m_t = t;
            if (t > target) {
                m_t = target;
            }
        }
        UpdateCurrentValue();
    }

    void EaseInOut::UpdateCurrentValue() {
        if ((double) m_t == 2.5 * PI) {
            m_current = m_min + m_range;
        } else {
            double s = (double) AEMath::Sinf(m_t) * 0.5 + 0.5;
            m_current = (float) ((double) m_min + s * (double) m_range);
        }
    }

    float EaseInOut::GetValue() {
        return m_current;
    }

    float EaseInOut::GetMinValue() {
        return m_min;
    }
}
