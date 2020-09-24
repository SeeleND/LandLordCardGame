#include "Robot.h"
#include "CallLordThread.h"
#include "PlayHandThread.h"

Robot::Robot(QObject *parent)
{
    m_type = Player::Robot;
}

void Robot::StartCallLord()
{
    CallLordThread* thread = new CallLordThread(this);
    thread->start();
}

void Robot::StartPlayHand()
{
    PlayHandThread* thread = new PlayHandThread(this);
    thread->start();
}

void Robot::ThinkForCallLord()
{
    CallLord(0);
}

void Robot::ThinkForPlayHand()
{
    PlayHand(Cards());
}

