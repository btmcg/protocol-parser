#pragma once

#include "book.hpp"
#include <cstdint>


struct instrument
{
    std::uint16_t locate;
    char name[8];
    book bids;
    book asks;

    instrument() noexcept;
    instrument(std::uint16_t locate, char const (&name)[8]) noexcept;
    void set_name(char const (&name)[8]) noexcept;
};
