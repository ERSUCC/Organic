#pragma once

struct Constants
{
    enum Sequence
    {
        Forward,
        Backward,
        PingPong,
        Random
    };

    enum Random
    {
        Step,
        Linear
    };

    enum Round
    {
        Nearest,
        Up,
        Down
    };
};
