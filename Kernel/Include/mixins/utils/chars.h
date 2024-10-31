namespace Chars {
    bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool IsAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool IsAlnum(char c)
    {
        return IsAlpha(c) || IsDigit(c);
    }

    bool IsLowerCase(char c)
    {
        return c >= 'a' && c <= 'z';
    }

    bool IsUpperCase(char c)
    {
        return c >= 'A' && c <= 'Z';
    }

    char ToLowerCase(char c)
    {
        if (IsUpperCase(c)) {
            return c + 32;
        }
        return c;
    }

    char ToUpperCase(char c)
    {
        if (IsLowerCase(c)) {
            return c - 32;
        }
        return c;
    }
}