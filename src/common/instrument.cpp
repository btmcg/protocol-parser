#include "instrument.hpp"


instrument::instrument() noexcept
        : locate(0)
        , name{}
        , bids()
        , asks()
{
    // empty
}


instrument::instrument(std::uint16_t l, char const (&nm)[8]) noexcept
        : locate(l)
        , name()
        , bids()
        , asks()
{
    for (std::size_t i = 0; i < sizeof(nm); ++i) {
        if (nm[i] == ' ') {
            name[i] = '\0';
            break;
        }
        name[i] = nm[i];
    }
}
