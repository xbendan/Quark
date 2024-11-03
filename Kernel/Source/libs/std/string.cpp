#include <mixins/std/string.h>

extern "C"
{
    void* memset(void* p, u8 value, usize num)
    {

#if defined(__x86_64__)
        asm volatile("rep stosb" : "+D"(p), "+c"(num) : "a"(value) : "memory");
#elif
        for (u64 i = 0; i < len; i++) {
            ((u8*)_min)[i] = val;
        }
#endif
        return p;
    }

    void* memcpy(void* dest, const void* src, usize num)
    {
        u8*       d = (u8*)dest;
        const u8* s = (const u8*)src;
        for (usize i = 0; i < num; i++) {
            d[i] = s[i];
        }
        return dest;
    }

    int memcmp(const void* s1, const void* s2, usize n)
    {
        const u8* p1 = (const u8*)s1;
        const u8* p2 = (const u8*)s2;
        for (usize i = 0; i < n; i++) {
            if (p1[i] < p2[i]) {
                return -1;
            } else if (p1[i] > p2[i]) {
                return 1;
            }
        }
        return 0;
    }

    usize strlen(const char* str)
    {
        usize len = 0;
        while (str[len] != '\0') {
            len++;
        }
        return len;
    }

    usize strnlen(const char* str, usize max)
    {
        usize len = 0;
        while (str[len] != '\0' && len < max) {
            len++;
        }
        return len;
    }
}