#pragma once

#include <Arduino.h>
#include "TwoDigitDisplay.h"

enum AnimationType {
    NONE,
    TRANSITION,
    CLOCKWISE_INIT,
    COUNTERCLOCKWISE_INIT
};

class DisplayAnimator {
private:
    TwoDigitDisplay* display;
    int startNumber;
    int endNumber;
    unsigned long startTime;
    unsigned long durationPer10Units;
    bool animating;
    AnimationType currentAnimation;
    int currentSegment;
    unsigned long segmentDuration;
    unsigned long lastSegmentUpdateTime;

    const byte segmentSequence[8][2] = {
        {128, 0},  // Top left segment on dozens display
        {0, 128},  // Top right segment on units display
        {0, 64},   // Upper right segment on units display
        {0, 32},   // Lower right segment on units display
        {0, 16},   // Bottom right segment on units display
        {16, 0},   // Bottom left segment on dozens display
        {8, 0},    // Lower left segment on dozens display
        {4, 0}     // Upper left segment on dozens display
    };

public:
    DisplayAnimator();
    void setup(TwoDigitDisplay* displayInstance);
    void animateTransition(int fromNumber, int toNumber, unsigned long durationPer10Units);
    void animateClockwiseInit(unsigned long segmentDurationMs);
    void animateCounterClockwiseInit(unsigned long segmentDurationMs);
    void stop();
    void update();
    unsigned long getElapsedTime() const;
    AnimationType getCurrentAnimation() const;
};
