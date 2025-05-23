#pragma once

/**
 * @file Timer.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

#include <libprotocol/Timeouts.hpp>

namespace protocol::state
{
    class Timer
    {
    public:
        virtual ~Timer() = default;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;

        virtual Timeouts getTimeouts() const = 0;
    };
}