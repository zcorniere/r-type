#include "System.hpp"

System::System(std::function<void(Game &)> system) : m_call_wrapper(system)
{
}

void System::call(Game &game) const
{
    m_call_wrapper(game);
}

template <>
IDisplayModule &getModule(Game &game)
{
    return game.displayModule.value().get();
}
template <>
IInputModule &getModule(Game &game)
{
    return game.inputModule.value().get();
}
template <>
IAudioModule &getModule(Game &game)
{
    return game.audioModule.value().get();
}

template <>
Game &getModule(Game &game)
{
    return game;
}
