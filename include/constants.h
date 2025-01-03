#pragma once

struct BytecodeConstants
{
    static constexpr char OBC_ID[] = "BACH";
    static constexpr unsigned int OBC_ID_LENGTH = 4;
    static constexpr unsigned int HEADER_LENGTH = 6;

    static constexpr unsigned char RETURN = 0x0;
    static constexpr unsigned char STACK_PUSH_DEFAULT = 0x1;
    static constexpr unsigned char STACK_PUSH_BYTE = 0x2;
    static constexpr unsigned char STACK_PUSH_INT = 0x3;
    static constexpr unsigned char STACK_PUSH_DOUBLE = 0x4;
    static constexpr unsigned char STACK_PUSH_ADDRESS = 0x5;
    static constexpr unsigned char STACK_PUSH_RESOURCE = 0x6;
    static constexpr unsigned char SET_VARIABLE = 0x7;
    static constexpr unsigned char GET_VARIABLE = 0x8;
    static constexpr unsigned char CALL_NATIVE = 0x9;
    static constexpr unsigned char CALL_USER = 0xa;

    static constexpr unsigned char LIST = 0x0;
    static constexpr unsigned char TIME = 0x1;

    static constexpr unsigned char ADD = 0x10;
    static constexpr unsigned char SUBTRACT = 0x11;
    static constexpr unsigned char MULTIPLY = 0x12;
    static constexpr unsigned char DIVIDE = 0x13;
    static constexpr unsigned char POWER = 0x14;
    static constexpr unsigned char EQUAL = 0x15;
    static constexpr unsigned char LESS = 0x16;
    static constexpr unsigned char GREATER = 0x17;
    static constexpr unsigned char LESSEQUAL = 0x18;
    static constexpr unsigned char GREATEREQUAL = 0x19;

    static constexpr unsigned char SINE = 0x30;
    static constexpr unsigned char SQUARE = 0x31;
    static constexpr unsigned char TRIANGLE = 0x32;
    static constexpr unsigned char SAW = 0x33;
    static constexpr unsigned char NOISE = 0x34;
    static constexpr unsigned char SAMPLE = 0x35;

    static constexpr unsigned char HOLD = 0x50;
    static constexpr unsigned char LFO = 0x51;
    static constexpr unsigned char SWEEP = 0x52;
    static constexpr unsigned char SEQUENCE = 0x53;
    static constexpr unsigned char REPEAT = 0x54;
    static constexpr unsigned char RANDOM = 0x55;
    static constexpr unsigned char LIMIT = 0x56;
    static constexpr unsigned char TRIGGER = 0x57;
    static constexpr unsigned char IF = 0x58;

    static constexpr unsigned char DELAY = 0x70;

    static constexpr unsigned char PLAY = 0x90;
    static constexpr unsigned char PERFORM = 0x91;
};
