#include "engine/math/Vector.h"

namespace AbyssEngine {
    namespace AEMath {
        Vector &Vector::operator*=(const Vector &rhs) {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            return *this;
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Vector::operator float *() { return &x; }
        Vector::operator const float *() const { return &x; }

        float &Vector::operator[](int i) { return (&x)[i]; }
        float Vector::operator[](int i) const { return (&x)[i]; }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Vector &Vector::operator-=(const Vector &rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Vector &Vector::operator/=(const Vector &rhs) {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Vector &Vector::operator=(const Vector &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            return *this;
        }
    }
}
