template<typename T>
struct l0_insertion_sampler final
{
    l0_insertion_sampler & operator+=(T && item)
    {
        return *this;
    }

    operator T() const
    {
        return m;
    }
private:
    T m;
};
