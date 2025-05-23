#pragma once

/**
 * @file Type.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

namespace protocol::state
{
    static constexpr auto TYPE_COUNT = 3;

    enum class Type
    {
        IDLE = 0,
        PRIMARY = 1,
        SECONDARY = 2,
    };
}