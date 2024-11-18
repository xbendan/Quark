namespace Quark::System::Diagnostic {
    static inline void MakePageFault()
    {
        int* p = reinterpret_cast<int*>(0x100'0000'0000ULL);
        *p     = 0;
    }
}