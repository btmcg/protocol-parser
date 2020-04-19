#pragma once

#include <cstdint>

#define PACKED __attribute__((packed))


namespace soup_bin_tcp {
    inline namespace v3_0_0 {

        struct header
        {
            std::uint16_t length;
            std::uint8_t type;
        } PACKED;
        static_assert(sizeof(header) == 3);

        struct debug : public header
        {
            // char text[];
        } PACKED;

        struct login_accepted : public header
        {
            char session[10];
            char sequence_number[20];
        } PACKED;
        static_assert(sizeof(login_accepted) == 33);

        struct login_rejected : public header
        {
            char reject_reason_code;
        } PACKED;
        static_assert(sizeof(login_rejected) == 4);

        struct sequenced_data : public header
        {
            // std::uint8_t message[];
        } PACKED;

        struct server_heartbeat : public header
        {
        } PACKED;
        static_assert(sizeof(server_heartbeat) == 3);

        struct end_of_session : public header
        {
        } PACKED;
        static_assert(sizeof(end_of_session) == 3);

        struct login_request : public header
        {
            char username[6];
            char password[10];
            char requested_session[10];
            char requested_sequence_number[20];
        } PACKED;
        static_assert(sizeof(login_request) == 49);

        struct unsequenced_data : public header
        {
            // std::uint8_t message[];
        } PACKED;

        struct client_heartbeat : public header
        {
        } PACKED;
        static_assert(sizeof(client_heartbeat) == 3);

        struct logout_request : public header
        {
        } PACKED;
        static_assert(sizeof(logout_request) == 3);

    } // namespace v3_0_0
} // namespace soup_bin_tcp

#undef PACKED
