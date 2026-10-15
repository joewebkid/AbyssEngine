unsigned short *computeFloatString(float value, int precision,
                                   int *exponentOut, int *negativeOut, int mode) {
    unsigned short *buf = new unsigned short[33];
    int count = precision & ~(precision >> 31);
    if (count >= 30)
        count = 30;

    *negativeOut = 0;
    if (value < 0.0f) {
        value = -value;
        *negativeOut = 1;
    }

    int wholeInt = (int) value;
    float whole = (float) wholeInt;
    float fraction = value - whole;
    int exponent = 0;
    unsigned short *write = buf;
    if (wholeInt != 0) {
        unsigned short *reverse = buf + 31;
        do {
            float quotient = whole / 10.0f;
            float truncated = (float) (int) quotient;
            *reverse-- = (unsigned short) ((int) (((double) (quotient - truncated) + 0.03) * 10.0) + '0');
            whole = truncated;
            ++exponent;
        } while (whole != 0.0f);
        for (++reverse; reverse < buf + 32; ++reverse)
            *write++ = *reverse;
    } else if (fraction > 0.0f) {
        float previous;
        exponent = 1;
        do {
            previous = fraction;
            --exponent;
            fraction *= 10.0f;
        } while (fraction < 1.0f);
        fraction = previous;
    }

    *exponentOut = exponent;
    int endIndex = count + (mode == 0 ? exponent : 0);
    if (endIndex < 0) {
        buf[0] = 0;
        return buf;
    }

    while (write - buf <= endIndex && write < buf + 32) {
        float scaled = fraction * 10.0f;
        int digit = (int) scaled;
        fraction = scaled - (float) digit;
        *write++ = (unsigned short) ('0' + digit);
    }
    if (endIndex >= 32) {
        buf[31] = 0;
        return buf;
    }

    unsigned short *cursor = buf + endIndex;
    unsigned short rounded = (unsigned short) (*cursor + 5);
    *cursor = rounded;
    while (rounded >= '9' + 1) {
        *cursor = '0';
        if (cursor == buf) {
            buf[0] = '1';
            *exponentOut = ++exponent;
            if (mode == 0) {
                if (endIndex > 0)
                    buf[endIndex] = '0';
                ++endIndex;
            }
            break;
        }
        rounded = (unsigned short) (*--cursor + 1);
        *cursor = rounded;
    }
    buf[endIndex] = 0;
    return buf;
}
