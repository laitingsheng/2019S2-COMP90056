#include <ctime>
#include <random>
#include <type_traits>

static std::random_device rd;

template<typename V, typename FD, typename E = std::mt19937_64>
struct insertion_stream final
{
    using VT = V;
    using FT = typename FD::result_type;

    insertion_stream(VT vmin, VT vmax, FT fmin, FT fmax) : fe(rd()), ve(rd()), fd(fmin, fmax), vd(vmin, vmax) {}

    std::pair<VT, FT> operator++(int)
    {
        return { vd(ve), fd(fe) };
    }
private:
    using _check_F = std::enable_if_t<std::is_unsigned_v<FT>>;

    std::uniform_int_distribution<VT> vd;
    FD fd;
    E ve, fe;
};
