#pragma once

#include <cstdint>


template <std::size_t Capacity>
class byte_buffer
{
private:
    std::uint8_t buf_[Capacity];
    std::uint8_t const* rptr_ = nullptr;
    std::uint8_t* wptr_ = nullptr;

public:
    byte_buffer() noexcept;
    std::uint8_t const* read_ptr() const noexcept;
    std::uint8_t* write_ptr() noexcept;
    void bytes_read(std::size_t) noexcept;
    void bytes_written(std::size_t) noexcept;
    void shift() noexcept;

    constexpr std::size_t capacity() const noexcept;
    std::size_t bytes_unread() const noexcept;
};


/**********************************************************************/

template <std::size_t Capacity>
byte_buffer<Capacity>::byte_buffer() noexcept
        : buf_()
        , rptr_(buf_)
        , wptr_(buf_)
{
    // empty
}

template <std::size_t Capacity>
std::uint8_t const*
byte_buffer<Capacity>::read_ptr() const noexcept
{
    return rptr_;
}

template <std::size_t Capacity>
std::uint8_t*
byte_buffer<Capacity>::write_ptr() noexcept
{
    return wptr_;
}

template <std::size_t Capacity>
void
byte_buffer<Capacity>::bytes_read(std::size_t nbytes) noexcept
{
    rptr_ += nbytes;
}

template <std::size_t Capacity>
void
byte_buffer<Capacity>::bytes_written(std::size_t nbytes) noexcept
{
    wptr_ += nbytes;
}

template <std::size_t Capacity>
void
byte_buffer<Capacity>::shift() noexcept
{
}

template <std::size_t Capacity>
constexpr std::size_t
byte_buffer<Capacity>::capacity() const noexcept
{
    return Capacity;
}

template <std::size_t Capacity>
std::size_t
byte_buffer<Capacity>::bytes_unread() const noexcept
{
    return wptr_ - rptr_;;
}
