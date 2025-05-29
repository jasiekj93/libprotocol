#pragma once

#include <libprotocol/DebugObserver.hpp>

class Presenter : public protocol::DebugObserver
{
public:
    Presenter() = default;

    void receivedFrameCallback(protocol::state::Type, const protocol::Frame&) override;
    void sentFrameCallback(protocol::state::Type, const protocol::Frame&) override;
    void stateChangedCallback(protocol::state::Type previous, protocol::state::Type actual, bool wasError) override;
    void allDataSentCallback(protocol::state::Type) override;
};
