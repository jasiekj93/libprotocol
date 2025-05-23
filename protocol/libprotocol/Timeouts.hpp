#pragma once

/**
 * @file Timeouts.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

namespace protocol
{
    struct Timeouts
    {
        using Ms = int;

        static constexpr Ms DEFAULT_IDLE_TIMEOUT = 10;
        static constexpr Ms DEFAULT_PRIMARY_TIMEOUT = 100;
        static constexpr Ms DEFAULT_SECONDARY_TIMEOUT = 2000;

        Ms idle = DEFAULT_IDLE_TIMEOUT; 
        Ms primary = DEFAULT_PRIMARY_TIMEOUT; 
        Ms secondary = DEFAULT_SECONDARY_TIMEOUT; 
    };
}