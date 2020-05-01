#include "instrument.hpp"


instrument::instrument() noexcept
        : locate(0)
        , name{}
        , book()
{
    // empty
}


instrument::instrument(std::uint16_t l, char const (&nm)[8]) noexcept
        : locate(l)
        , name()
        , book()
{
    set_name(nm);
}


void
instrument::set_name(char const (&nm)[8]) noexcept
{
    for (std::size_t i = 0; i < sizeof(nm); ++i) {
        if (nm[i] == ' ') {
            name[i] = '\0';
            break;
        }
        name[i] = nm[i];
    }
}
