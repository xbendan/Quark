namespace System::UI::Controls::Layout {
    enum class GridUnitType
    {
        Auto,
        Star,
        Pixel
    };

    struct RowDefinition
    {
        GridUnitType Height;
        float        Value;
    };

    struct ColumnDefinition
    {
        GridUnitType Width;
        float        Value;
    };

    class GridView
    {};
}