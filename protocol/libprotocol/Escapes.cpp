#include "Escapes.hpp"

using namespace protocol;


static int countEscapes(etl::span<Byte> buffer, Byte escape);

static bool isEscapedChar(Byte byte, const EscapeConfig& config)
{
    if(byte == config.escapeChar)
        return true;
    else
        return etl::find(config.escapedCharacters.begin(), 
            config.escapedCharacters.end(), 
            byte) != config.escapedCharacters.end();
}

bool protocol::addEscapes(etl::span<const Byte> input, etl::ivector<Byte>& output, const EscapeConfig &config)
{
    for(auto& byte : input)
    {
        if(output.available() < 2)
            return false;

        if (isEscapedChar(byte, config))
            output.push_back(config.escapeChar);

        output.push_back(byte);
    }

    return true;
}

bool protocol::removeEscapes(etl::span<const Byte> input, etl::ivector<Byte> &output, const EscapeConfig &config)
{
    bool escaped = false;

    for(auto& byte : input)
    {
        if (byte == config.escapeChar and not escaped)
        {
            escaped = true;
            continue;
        }

        if(output.full())
            return false;
        
        output.push_back(byte);
        escaped = false;
    }

    return true;
}

etl::span<Byte>::iterator protocol::findFlag(etl::span<Byte> buffer, Byte flag, Byte escape)
{
    auto iterator = buffer.begin();

    if(*iterator == flag)
        return iterator;

    while(true)
    {
        iterator = std::find(etl::next(iterator), buffer.end(), flag);

        if(iterator == buffer.end())
            return buffer.end();
        
        if(auto escapeCount = countEscapes({ buffer.begin(), iterator }, escape); 
            (escapeCount != 0 and (escapeCount % 2 == 1)))
            continue;
        else
            return iterator;
    }
}

static int countEscapes(etl::span<Byte> buffer, Byte escape)
{
    int count = 0;

    for(auto it = buffer.rbegin(); it != buffer.rend(); ++it)
    {
        if(*it == escape)
            count++;
        else
            return count;
    }

    return count;
}