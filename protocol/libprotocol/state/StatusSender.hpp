#pragma once

/**
 * @file StatusSender.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

namespace protocol::state
{
    class StatusSender
    {
    public:
        virtual ~StatusSender() = default;

        virtual void sendReceiveStatus() = 0;
        virtual void sendConnect() = 0;
        virtual void sendDisconnect() = 0;
        virtual void sendConnectAck() = 0;
    };
}