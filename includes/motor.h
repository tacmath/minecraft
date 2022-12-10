#ifndef MOTOR_CLASS_H
#define MOTOR_CLASS_H

class Motor {
    float previousUpdateTime;
    float diff;

public:
    Motor ();

    bool update(float time);
};

#endif