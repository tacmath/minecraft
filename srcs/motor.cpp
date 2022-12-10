#include "motor.h"

Motor::Motor() {
    previousUpdateTime = 0;
    diff = 0;
}

bool Motor::update(float time) {
    diff = time - previousUpdateTime;
    if (diff >= 0.02f) {
        previousUpdateTime = time - (diff - 0.02f);
        return true;
    }
    return false;
}
