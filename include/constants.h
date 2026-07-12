#pragma once

struct Constants
{
    enum Sequence
    {
        Forward,
        Backward,
        PingPong,
        Shuffle
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
