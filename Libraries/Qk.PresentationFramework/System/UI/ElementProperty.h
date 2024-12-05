namespace System::UI {
    template <typename T>
    class ElementProperty
    {
    public:
        ElementProperty();
        ElementProperty(const ElementProperty& other);
        ElementProperty(ElementProperty&& other) noexcept;
        ElementProperty& operator=(const ElementProperty& other);
        ElementProperty& operator=(ElementProperty&& other) noexcept;
        ~ElementProperty();

        T&   operator()();
        void operator()(T const& value);

    private:
        // Name
        T m_value; // Value

        T& (*m_getter)(T const&); // Getter
        void (*m_setter)(T const& it,
                         T&       value); // Setter
    };

    template <typename T>
    class ElementPropertyDefinition
    {};
}