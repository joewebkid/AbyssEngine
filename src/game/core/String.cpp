#include "game/core/String.h"
#include "engine/core/GameText.h"

#include <cstdlib>
#include <cstring>

void String_printImpl(const char *s);

uint16_t *String_computeFloatString(float v, int base, int *outExp, int *outNeg);

void String_concat(String * out, String * a, String * b);

void String_assign_op(String * self, String * src);

String::~String() {
    if (this->data)
        delete[] this->data;
    this->data = nullptr;
}

char *String::GetAEChar() const {
    unsigned int len = (unsigned int) this->length;
    unsigned int n = len + 1;
    char *out = new char[n];
    for (unsigned int i = 0; i < len; i++)
        out[i] = (char) (this->data[i] & 0xff);
    out[len] = 0;
    return out;
}

void String::ReplaceString(String find, String repl) {
    if (this->data == nullptr || this->length == 0 || find.length == 0)
        return;

    String acc;
    unsigned int pos = 0;
    int idx;
    while ((idx = (int) ((String *) (this))->IndexOf(pos, find)) != -1) {
        acc += this->SubString(pos, (unsigned int) idx) + repl;
        pos = (unsigned int) find.length + idx;
    }

    if (pos != 0 && pos < (unsigned int) this->length)
        acc += this->SubString(pos, (unsigned int) this->length);

    if (acc.length != 0)
        this->Set(acc.data);
}

void String::ReplaceChar(char from, char to) {
    for (int i = 0; i < this->length; i++)
        if ((unsigned int) this->data[i] == (unsigned int) (int) from)
            this->data[i] = (unsigned short) (short) to;
}

void String::Reverse() {
    if (this->length > 0 && GameText::getLanguage() == 9) {
        for (int i = 0, j = this->length - 1; i < j; i++, j--) {
            unsigned short t = this->data[i];
            this->data[i] = this->data[j];
            this->data[j] = t;
        }
    }
}

void String::ToUpperCase() {
    for (int i = 0; i < this->length; i++) {
        short c = (short) this->data[i];
        unsigned short u = (unsigned short) (c - 0x61);
        bool above = u > 0x19;
        bool eq1a = u == 0x1a;
        if (above) {
            u = (unsigned short) (c - 0xe0);
            eq1a = u == 0x1e;
        }
        if (!above || u < 0x1e || eq1a) {
            this->data[i] = (unsigned short) (c - 0x20);
        } else {
            switch (c) {
                case 0x81: this->data[i] = 0x9a;
                    break;
                case 0x82: this->data[i] = 0x90;
                    break;
                case 0x84: this->data[i] = 0x8e;
                    break;
                case 0x86: this->data[i] = 0x8f;
                    break;
                case 0x83:
                case 0x85:
                    break;
                default:
                    if (c == 0x91) this->data[i] = 0x92;
                    else if (c == 0x94) this->data[i] = 0x99;
                    else if (c == 0xa4) this->data[i] = 0xa5;
                    break;
            }
        }
    }
}

int String::ValueOf() {
    char *bytes = ((String *) (this))->GetAEChar();
    int v = atoi(bytes);
    delete[] bytes;
    return v;
}

void *String::Split(String sep) {
    if (this->length != 0 && sep.length != 0) {
        Array<String *> *arr = new Array<String *>();

        unsigned int pos = 0;
        unsigned int idx;
        while ((idx = ((String *) (this))->IndexOf(pos, sep)) != 0xffffffff) {
            if (pos < idx) {
                String *piece = new String();
                *piece = this->SubString(pos, idx);
                ArrayAdd(piece, *arr);
            }
            pos = (unsigned int) sep.length + idx;
        }
        if (pos != 0 && pos < (unsigned int) this->length) {
            String *piece = new String();
            *piece = this->SubString(pos, (unsigned int) this->length);
            ArrayAdd(piece, *arr);
        }

        if (arr->size() != 0)
            return arr;

        ArrayRemoveAll(*arr);
        delete arr;
    }
    return 0;
}

static unsigned short g_String_nullChar = 0;

unsigned short *String::operator[](int i) {
    if (i < 0 || i >= this->length)
        return &g_String_nullChar;
    return this->data + i;
}

void String::Set(long long v) {
    if (this->data) delete[] this->data;
    this->data = nullptr;
    this->length = 0;

    if (v == 0) {
        this->Set("");
        return;
    }

    bool neg = v < 0;
    unsigned long long mag = neg ? (unsigned long long) (-v) : (unsigned long long) v;

    char16_t tmp[24];
    int n = 0;
    do {
        unsigned int rem = (unsigned int) (mag % 10);
        char16_t ch = (char16_t) (rem | 0x30);
        if (rem > 9)
            ch = (char16_t) (rem + 0x57);
        tmp[n++] = ch;
        mag /= 10;
    } while (mag != 0);

    int total = n + (neg ? 1 : 0);
    unsigned short *out = (unsigned short *) ::operator new[]((total + 1) * 2);
    int w = 0;
    if (neg)
        out[w++] = (unsigned short) u'-';
    for (int i = n - 1; i >= 0; i--)
        out[w++] = (unsigned short) tmp[i];
    out[w] = 0;
    this->Set(out);
    ::operator delete[](out);
}

int String::StrLen(const char *s) {
    const char *p = s;
    while (*p != '\0')
        p++;
    return (int) (p - s);
}

unsigned int String::Compare(const char *s) {
    if (this->length == 0)
        return 1;

    bool reachedEnd = false;
    uint16_t cur = 0;
    unsigned int other = 0;
    int i = 0;
    for (; i < this->length && (cur = (uint16_t) this->data[i]) != 0; i++) {
        other = (unsigned int) (unsigned char) *s;
        if (other == 0 || other != cur)
            goto done;
        s++;
    }
    other = (unsigned int) (unsigned char) *s;
    reachedEnd = true;
    cur = 0;
done:
    if (other != 0) {
        char d = (char) cur - (char) other;
        if (reachedEnd)
            d = -1;
        return (unsigned int) (int) d;
    }
    return (unsigned int) (cur != 0);
}

void String::Set(const char *s) {
    if (this->data) delete[] this->data;
    this->data = nullptr;
    this->length = 0;
    if (s == 0)
        return;
    int n = 0;
    for (const unsigned char *p = (const unsigned char *) s; *p != 0; p++)
        n++;
    if (n == 0)
        return;
    unsigned short *nd = new unsigned short[n + 1];
    const unsigned char *p = (const unsigned char *) s;
    for (int i = 0; i < n; i++)
        nd[i] = (unsigned short) p[i];
    nd[n] = 0;
    this->data = nd;
    this->length = n;
}

int String::Compare(const String &otherRef) {
    const String *other = &otherRef;
    short result;
    if (other->length == this->length) {
        int i = 0;
        short sc = 0, oc = 0;
        bool reachedEnd = false;
        while (true) {
            sc = (i < this->length) ? (short) this->data[i] : 0;
            oc = (i < other->length) ? (short) other->data[i] : 0;
            if (sc == 0) {
                reachedEnd = true;
                sc = 0;
                break;
            }
            if (oc != sc) {
                reachedEnd = false;
                break;
            }
            i++;
        }
        if (oc == 0) {
            result = sc;
            if (sc != 0)
                result = 1;
        } else {
            result = sc - oc;
            if (reachedEnd)
                result = -1;
        }
    } else {
        result = 0xff;
    }
    return (int) (char) result;
}

void String::Trim() {
    int len = this->length;
    if (len == 0)
        return;

    int start = 0;
    while (start < len) {
        unsigned short c = this->data[start];
        if (c != 0x20 && c != 9)
            break;
        start++;
    }
    int end = len;
    while (end > 0) {
        unsigned short c = this->data[end - 1];
        if (c != 0x20 && c != 9)
            break;
        end--;
    }
    if (start < end) {
        int sublen = end - start;
        unsigned short *sub = (unsigned short *) ::operator new[]((sublen + 1) * 2);
        memcpy(sub, this->data + start, (size_t) sublen * 2);
        sub[sublen] = 0;
        this->Set(sub);
        ::operator delete[](sub);
    } else {
        if (this->data) delete[] this->data;
        this->data = nullptr;
        this->length = 0;
    }
}

int String::GetStringLength(const char *s) {
    const char *p = s;
    while (*p != '\0')
        p++;
    return (int) (p - s);
}

void String::ToLowerCase() {
    for (int i = 0; i < this->length; i++) {
        short c = (short) this->data[i];
        unsigned short u = (unsigned short) (c - 0x41);
        bool above = u > 0x19;
        bool eq1a = u == 0x1a;
        if (above) {
            u = (unsigned short) (c - 0xc0);
            eq1a = u == 0x1e;
        }
        if (!above || u < 0x1e || eq1a) {
            this->data[i] = (unsigned short) (c + 0x20);
        } else {
            switch (c) {
                case 0x8e: this->data[i] = 0x84;
                    break;
                case 0x8f: this->data[i] = 0x86;
                    break;
                case 0x90: this->data[i] = 0x82;
                    break;
                case 0x92: this->data[i] = 0x91;
                    break;
                case 0x99: this->data[i] = 0x94;
                    break;
                case 0x9a: this->data[i] = 0x81;
                    break;
                case 0x91:
                case 0x93:
                case 0x94:
                case 0x95:
                case 0x96:
                case 0x97:
                case 0x98:
                    break;
                default:
                    if (c == 0xa5)
                        this->data[i] = 0xa4;
                    break;
            }
        }
    }
}

String &String::operator=(const String &other) {
    if (other.data != nullptr)
        this->Set(other.data);
    return *this;
}

void String::Set(const unsigned short *s) {
    if (this->data) delete[] this->data;
    this->data = nullptr;
    this->length = 0;
    if (s == 0)
        return;
    int n = 0;
    while (s[n] != 0)
        n++;
    if (n == 0)
        return;
    unsigned short *nd = new unsigned short[n + 1];
    for (int i = 0; i < n; i++)
        nd[i] = s[i];
    nd[n] = 0;
    this->data = nd;
    this->length = n;
}

void String::ConvertFromUTF8() {
    if (this->length == 0)
        return;

    char *bytes = ((String *) (this))->GetAEChar();
    uint16_t *wide = String::getWCharFromUtf8(bytes, this->length);
    ((String *) (this))->Set_wchar(wide);
    delete[] bytes;
    delete[] wide;
}

static const char kOpen[] = "<";
static const char kClose[] = ">";
static const char kSlash[] = "</";

void String::SplitTags(String tag) {
    if (this->length == 0 || tag.length == 0)
        return;

    {
        String open(kOpen, false);
        String close(kClose, false);
        tag = open + tag + close;
    }

    Array<String *> *arr = new Array<String *>();

    int endPos = 0;
    unsigned int pos = 0;
    unsigned int idx;
    while ((idx = ((String *) (this))->IndexOf(pos, tag)) != 0xffffffff) {
        if (pos <= idx) {
            String *piece = new String();
            *piece = this->SubString(pos, idx);
            ArrayAdd(piece, *arr);

            unsigned int afterTag = (unsigned int) tag.length + idx;
            String closer;
            ((String *) (&closer))->ctor_char(kSlash, false);
            endPos = (int) ((String *) (this))->IndexOf(afterTag, closer);
            if (endPos == -1)
                goto done;

            String *piece2 = new String();
            *piece2 = this->SubString(afterTag, (unsigned int) endPos);
            ArrayAdd(piece2, *arr);
        }
        pos = endPos + 1;
    }

    if (pos != 0 && pos < (unsigned int) this->length) {
        String *piece = new String();
        *piece = this->SubString(pos, (unsigned int) this->length);
        ArrayAdd(piece, *arr);
    }

    if (arr->size() == 0) {
        ArrayRemoveAll(*arr);
        delete arr;
    }
done:
    ;
}

static const char kZeroDot[] = "0.";
static const char kZero[] = "0";
static const char kDot[] = ".";
static const char kExp[] = "E";

void String::Set(float v) {
    int exp = 0;
    int neg = 0;
    uint16_t *digitsW = String_computeFloatString(v, 10, &exp, &neg);

    int digitCount = String::StrLen((const unsigned short *) digitsW);

    /* Upper bound on the result length: leading "0." plus up to -exp zeros,
       the digits, a possible '.' separator, and a possible leading 'E' sign. */
    int leadingZeros = (exp < 1 && exp < 0) ? -exp : 0;
    int cap = 2 + leadingZeros + digitCount + 1 + 1;
    unsigned short *acc = (unsigned short *) ::operator new[]((cap + 1) * 2);
    int len = 0;

    if (exp < 1) {
        for (const char *p = kZeroDot; *p; p++)
            acc[len++] = (unsigned short) (unsigned char) *p;
        for (int i = 0; exp < i; i--)
            for (const char *p = kZero; *p; p++)
                acc[len++] = (unsigned short) (unsigned char) *p;
    }

    for (const uint16_t *p = digitsW; *p != 0; p++)
        acc[len++] = (unsigned short) *p;

    if (exp > 0 && exp <= len) {
        /* Insert '.' after the first `exp` code units. */
        for (int i = len; i > exp; i--)
            acc[i] = acc[i - 1];
        acc[exp] = (unsigned short) (unsigned char) kDot[0];
        len++;
    }

    if (neg != 0) {
        /* Prepend the 'E' (exponent/sign marker). */
        for (int i = len; i > 0; i--)
            acc[i] = acc[i - 1];
        acc[0] = (unsigned short) (unsigned char) kExp[0];
        len++;
    }

    acc[len] = 0;

    delete[] digitsW;
    this->Set(acc);
    ::operator delete[](acc);
}

int String::StrLen(const unsigned short *s) {
    if (s == 0)
        return 0;
    const unsigned short *p = s;
    while (*p != 0)
        p++;
    return (int) (p - s);
}

String String::SubString(unsigned int start, unsigned int end) {
    String out;
    unsigned int slen = (unsigned int) this->length;
    if (start < end && start < slen) {
        unsigned int hi = end > slen ? slen : end;
        {
            const unsigned short *_src = this->data + start;
            int _n = (int) (hi - start);
            if (out.data) delete[] out.data;
            out.data = nullptr;
            out.length = 0;
            if (_src && _n >= 0) {
                unsigned short *_nd = new unsigned short[_n + 1];
                for (int _i = 0; _i < _n; _i++) _nd[_i] = _src[_i];
                _nd[_n] = 0;
                out.data = _nd;
                out.length = _n;
            }
        }
    }
    return out;
}

uint16_t *String::getWCharFromUtf8(char *s, int len) {
    int outCount = 0;
    for (int i = 0; i < len; i = i + 1) {
        if (((unsigned char) s[i] & 0xe0) == 0xc0)
            i = i + 1;
        else if (((unsigned char) s[i] & 0xf0) == 0xe0)
            i = i + 2;
        outCount = outCount + 1;
    }

    uint16_t *out = new uint16_t[outCount + 1];
    uint16_t *w = out;

    for (int i = 0; i < len; i = i + 1) {
        unsigned short cp = (unsigned short) (unsigned char) s[i];
        unsigned short val = cp;
        if ((cp & 0xe0) == 0xc0) {
            unsigned short hi = cp & 0x1f;
            int j = i + 1;
            val = (unsigned short) ((hi << 6) | ((unsigned char) s[j] & 0x3f));
            i = j;
        } else if ((cp & 0xf0) == 0xe0) {
            int j = i + 2;
            unsigned short mid = (unsigned short) (((unsigned char) s[i + 1] & 0x3f) | ((cp & 0xf) << 6));
            val = (unsigned short) ((mid << 6) | ((unsigned char) s[j] & 0x3f));
            i = j;
        }
        *w = val;
        w++;
    }
    out[outCount] = 0;

    for (int i = 0; i != outCount; i = i + 1) {
        unsigned short c = out[i];
        unsigned short r = c;
        switch (c) {
            case 0x410: r = 0x41;
                break;
            case 0x412: r = 0x42;
                break;
            case 0x415: r = 0x45;
                break;
            case 0x41a: r = 0x4b;
                break;
            case 0x41c: r = 0x4d;
                break;
            case 0x41d: r = 0x48;
                break;
            case 0x41e: r = 0x4f;
                break;
            case 0x420: r = 0x50;
                break;
            case 0x421: r = 0x43;
                break;
            case 0x422: r = 0x54;
                break;
            case 0x425: r = 0x58;
                break;
            case 0x411:
            case 0x413:
            case 0x414:
            case 0x416:
            case 0x417:
            case 0x418:
            case 0x419:
            case 0x41b:
            case 0x41f:
            case 0x423:
            case 0x424:
                break;
            case 0x43e:
            case 0xba: r = 0x6f;
                break;
            case 0x440: r = 0x70;
                break;
            case 0x441: r = 0x63;
                break;
            case 0x445: r = 0x78;
                break;
            case 0x43f:
            case 0x442:
            case 0x443:
            case 0x444:
                break;
            case 0x435: r = 0x65;
                break;
            case 0xaa:
            case 0x430: r = 0x61;
                break;
            case 0x60: r = 0x27;
                break;
            default:
                break;
        }
        out[i] = r;
    }
    return out;
}

unsigned int String::IndexOf(const String &needle) {
    return this->IndexOf(0, needle);
}

unsigned int String::IndexOf(unsigned int start, const String &needle) {
    unsigned int slen = (unsigned int) this->length;
    unsigned int nlen = (unsigned int) needle.length;
    while (start < slen && nlen <= slen - start) {
        if (needle.data[0] == this->data[start]) {
            unsigned int k = 0;
            while (start + k < slen && this->data[start + k] == needle.data[k]) {
                if (nlen <= k + 1)
                    return start;
                k++;
            }
            start += k;
        } else {
            start++;
        }
    }
    return 0xffffffff;
}

const unsigned short *String::operator[](int i) const {
    if (i < 0 || i >= this->length)
        return &g_String_nullChar;
    return this->data + i;
}

const unsigned short *String::GetAEWChar() const {
    return this->data;
}

String::operator unsigned short *() {
    return this->data;
}

String::operator const unsigned short *() const {
    return this->data;
}

void String::PrintOut() {
    char *bytes = ((String *) (this))->GetAEChar();
    String_printImpl(bytes);
}

String::String() : data(nullptr), length(0) {
}

String::String(const char *cstr, bool reverse) : data(nullptr), length(0) {
    Set(cstr);
    if (reverse) Reverse();
}

String::String(const uint16_t *wstr, bool reverse) : data(nullptr), length(0) {
    Set((const unsigned short *) wstr);
    if (reverse) Reverse();
}

String::String(const String &other, bool reverse) : data(nullptr), length(0) {
    Set(other.data);
    if (reverse) Reverse();
}

String::String(char c) : data(nullptr), length(0) { Set((long long) c); }

String::String(int v) : data(nullptr), length(0) { Set((long long) v); }

String::String(float v) : data(nullptr), length(0) { ctor_float(v); }

String::String(long long v) : data(nullptr), length(0) { ctor_longlong(v); }

String &String::operator+=(const String &other) {
    const unsigned short *src = other.data;
    int n = other.length;
    if (src && n > 0) {
        int newLen = this->length + n;
        unsigned short *nd = new unsigned short[newLen + 1];
        for (int i = 0; i < this->length; i++) nd[i] = this->data[i];
        for (int i = 0; i < n; i++) nd[this->length + i] = src[i];
        nd[newLen] = 0;
        if (this->data) delete[] this->data;
        this->data = nd;
        this->length = newLen;
    }
    return *this;
}

String &String::operator+=(const char &c) {
    unsigned short ch = (unsigned short) *(const unsigned char *) &c;
    {
        int newLen = this->length + 1;
        unsigned short *nd = new unsigned short[newLen + 1];
        for (int i = 0; i < this->length; i++) nd[i] = this->data[i];
        nd[this->length] = ch;
        nd[newLen] = 0;
        if (this->data) delete[] this->data;
        this->data = nd;
        this->length = newLen;
    }
    return *this;
}

String &String::operator+=(const int &v) {
    String tmp((long long) v);
    *this += tmp;
    return *this;
}

String &String::operator+=(const float &v) {
    String tmp(v);
    *this += tmp;
    return *this;
}

String &String::operator+=(const long long &v) {
    String tmp(v);
    *this += tmp;
    return *this;
}

// Static data member present in the original binary (defined for symbol parity).
char16_t AbyssEngine::String::termChar;
