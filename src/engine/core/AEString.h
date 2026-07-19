#ifndef GOF2_AESTRING_H
#define GOF2_AESTRING_H

#include <cstdint>

#define AESTRING_SHIM inline

namespace AbyssEngine {
    class String {
    public:
        unsigned short *data;
        int length;

        String();

        String(const char *cstr, bool reverse = false);

        AESTRING_SHIM String(const String &other) : data(nullptr), length(0) { Set(other.data); }

        String(const uint16_t *wstr, bool reverse);

        String(const String &other, bool reverse);

        explicit String(char c);

        explicit String(int v);

        explicit String(float v);

        explicit String(long long v);

        virtual ~String();

        String &operator=(const String &other);

        String &operator=(const char *cstr) {
            Set(cstr);
            return *this;
        }

        String &operator=(const char16_t *wstr) {
            Set(reinterpret_cast<const unsigned short *>(wstr));
            return *this;
        }

        String &operator+=(const String &other);

        String &operator+=(const char &c);

        String &operator+=(const int &v);

        String &operator+=(const float &v);

        String &operator+=(const long long &v);

        void copy(const String *src, bool reverse) {
            Set(src->data);
            if (reverse) Reverse();
        }
        const char16_t *text() const { return reinterpret_cast<const char16_t *>(data); }
        uint32_t size() const { return (uint32_t) length; }

        void Set(const char *s);

        void Set(const unsigned short *s);

        void Set(float v);

        void Set(long long v);

        static int StrLen(const char *s);

        static int StrLen(const unsigned short *s);

        static int GetStringLength(const char *s);

        int Compare(const String &other);

        unsigned int Compare(const char *s);

        const unsigned short *GetAEWChar() const;

        operator unsigned short *();

        operator const unsigned short *() const;

        unsigned short *operator[](int i);

        const unsigned short *operator[](int i) const;

        AESTRING_SHIM unsigned int Compare_char(const char *s) { return Compare(s); }
        AESTRING_SHIM int Compare_str(String *other) { return Compare(*other); }

        void ConvertFromUTF8();

        char *GetAEChar() const;

        unsigned int IndexOf(const String &needle);

        unsigned int IndexOf(unsigned int start, const String &needle);

        void PrintOut();

        void ReplaceChar(char from, char to);

        void ReplaceString(String find, String repl);

        void Reverse();

        AESTRING_SHIM void Set_char(const char *s) { Set(s); }
        AESTRING_SHIM void Set_float(float v) { Set(v); }

        AESTRING_SHIM void Set_longlong(long long v) { Set((long long) v); }
        AESTRING_SHIM void Set_wchar(const uint16_t *s) { Set((const unsigned short *) s); }

        void *Split(String sep);

        void SplitTags(String tag);

        AESTRING_SHIM int StrLen_char(const char *s) { return StrLen(s); }
        AESTRING_SHIM int StrLen_wchar(const uint16_t *s) { return StrLen((const unsigned short *) s); }

        String SubString(unsigned int start, unsigned int end);

        void ToLowerCase();

        void ToUpperCase();

        void Trim();

        int ValueOf();

        AESTRING_SHIM String *ctor_char(const char *s, bool reverse) {
            Set_char(s);
            if (reverse)
                Reverse();
            return this;
        }

        AESTRING_SHIM String *ctor_float(float v) {
            Set_float(v);
            return this;
        }

        AESTRING_SHIM String *ctor_longlong(long long v) {
            Set_longlong(v);
            return this;
        }

        AESTRING_SHIM uint16_t *index(int i) {
            return reinterpret_cast<uint16_t *>((*this)[i]);
        }

        AESTRING_SHIM uint16_t *index_const(int i) {
            return const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(
                (*const_cast<const String *>(this))[i]));
        }

        static uint16_t *getWCharFromUtf8(char *utf8, int len);

        // Static data member present in the original binary (defined for symbol parity).
        static char16_t termChar;
    };

    String operator+(const String &a, const String &b);

    String operator+(const int &a, const String &b);
}

using AbyssEngine::String;

int GetStringLength(AbyssEngine::String str);

#endif
