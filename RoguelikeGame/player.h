#ifndef PLAYER_H
#define PLAYER_H

struct Turn
{
    float timer = 0.0f;
    float duration = 0.0f;
};

enum MoveState
{
    MoveState_Moving,
    MoveState_Idle,
    MoveState_Rotating
};

enum Direction
{
    Direction_Front = 0,
    Direction_Right,
    Direction_Back,
    Direction_Left,
};

#endif//PLAYER_H