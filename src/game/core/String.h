#ifndef GOF2_STRING_H
#define GOF2_STRING_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
void String_ctor(String * self);

String *String_ctor_char(String *self, const char *s, bool reverse);

String *String_ctor_wchar(String *self, const uint16_t *s, bool reverse);

String *String_ctor_copy(String *self, String *other, bool reverse);

String *String_ctor_int(String *self, int v);

String *String_ctor_longlong(String *self, long long v);

String *String_ctor_float(String *self, float v);

String *String_ctor_charval(String *self, char c);

String *String_dtor(String * self);
void String_dtor_del(String * self);

void String_Set_char(String *self, const char *s);

void String_Set_wchar(String *self, const uint16_t *s);

void String_Set_longlong(String *self, long long v);

void String_Set_float(String *self, float v);

String *String_assign(String * self, String * other);
String *String_addAssign_str(String * self, String * other);

String *String_addAssign_char(String *self, const char *c);

String *String_addAssign_int(String *self, const int *v);

String *String_addAssign_longlong(String *self, const long long *v);

String *String_addAssign_float(String *self, const float *v);

void String_SubString(String *out, String *self, unsigned int start, unsigned int end);

unsigned int String_IndexOf_from(String *self, unsigned int start, String *needle);

unsigned int String_IndexOf(String * self, String * needle);

uint16_t *String_index(String *self, int i);

uint16_t *String_index_const(String *self, int i);

void String_ReplaceString(String * self, String * find, String * repl);

void String_ReplaceChar(String *self, char from, char to);

void String_Reverse(String * self);
void String_Trim(String * self);
void String_ToUpperCase(String * self);
void String_ToLowerCase(String * self);
void String_ConvertFromUTF8(String * self);

void *String_Split(String * self, String * sep);
void String_SplitTags(String * self, String * tag);

char *String_GetAEChar(String * self);
int String_ValueOf(String * self);

int String_StrLen_char(String *self, const char *s);

int String_StrLen_wchar(String *self, const uint16_t *s);

void String_PrintOut(String * self);

#endif
