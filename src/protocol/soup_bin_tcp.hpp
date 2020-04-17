#pragma once

#include <cstdint>

namespace soup_bin_tcp {
    inline namespace v3_0_0 {

        struct header
        {
            std::uint16_t length;
            std::uint8_t type;
        } __attribute__((packed));
        static_assert(sizeof(header) == 3);

        struct debug : public header
        {
            // char text[];
        } __attribute__((packed));

        struct login_accepted : public header
        {
            char session[10];
            char sequence_number[20];
        } __attribute__((packed));
        static_assert(sizeof(login_accepted) == 33);

        struct login_rejected : public header
        {
            char reject_reason_code;
        } __attribute__((packed));
        static_assert(sizeof(login_rejected) == 4);

        struct sequenced_data : public header
        {
            // std::uint8_t message[];
        } __attribute__((packed));

        struct server_heartbeat : public header
        {
        } __attribute__((packed));
        static_assert(sizeof(server_heartbeat) == 3);

        struct end_of_session : public header
        {
        } __attribute__((packed));
        static_assert(sizeof(end_of_session) == 3);

        struct login_request : public header
        {
            char username[6];
            char password[10];
            char requested_session[10];
            char requested_sequence_number[20];
        } __attribute__((packed));
        static_assert(sizeof(login_request) == 49);

        struct unsequenced_data : public header
        {
            // std::uint8_t message[];
        } __attribute__((packed));

        struct client_heartbeat : public header
        {
        } __attribute__((packed));
        static_assert(sizeof(client_heartbeat) == 3);

        struct logout_request : public header
        {
        } __attribute__((packed));
        static_assert(sizeof(logout_request) == 3);

    } // namespace v3_0_0
} // namespace soup_bin_tcp
