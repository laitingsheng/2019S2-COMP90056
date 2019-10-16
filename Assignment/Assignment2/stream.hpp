#include <ctime>
#include <random>
#include <type_traits>

template<typename V, typename FG, typename E, typename = void>
struct insertion_stream final;

template<typename V, typename FG, typename E = std::mt19937_64>
struct insertion_stream<V, FG, E, std::enable_if_t<std::is_integral_v<V>>> final
{
    using F = std::invoke_result_t<FG>;

    template<typename ... Ts>
    insertion_stream(V vmin, V vmax, Ts ... params) : fe(std::gmtime(nullptr)), ve(std::gmtime(nullptr)), fg(params ...), vg(vmin, vmax) {}

    std::pair<V, F> operator()()
    {
        return { vg(ve), fg(fe) }
    }
private:
    using _check_F = std::enable_if_t<std::is_unsigned_v<F>>

    std::uniform_int_distribution<V> vg;
    FG fg;
    E ve, fe;
};
