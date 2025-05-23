#pragma once

#include <libprotocol/Frame.hpp>
#include <libprotocol/state/Base.hpp>

namespace protocol
{
    class DebugObserver
    {
    public:
        virtual ~DebugObserver() = default;

        virtual void receivedFrameCallback(state::Type, const Frame&) = 0;
        virtual void sentFrameCallback(state::Type, const Frame&) = 0;
        virtual void stateChangedCallback(state::Type previous, state::Type actual, bool wasError) = 0;
        virtual void allDataSentCallback(state::Type) = 0;
    };
}