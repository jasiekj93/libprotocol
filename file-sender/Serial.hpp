#pragma once

#include <string>

#include <libprotocol/Transciever.hpp>

class Serial : public protocol::Transciever
{
public:
    Serial(std::string_view path);
    ~Serial();

    void send(etl::span<const uint8_t> data) override;
    etl::span<const uint8_t> read();

private:
    static constexpr size_t BUFFER_SIZE = 0x20000;

    int handle;
    etl::array<uint8_t, BUFFER_SIZE> buffer;
};