#pragma once

#include <Arduino.h>
#include "AppConstants.h"
#include "TwoDigitDisplay.h"

enum class AnimationType {
    NONE,
    TRANSITION,
    CLOCKWISE_LOADING,
    COUNTERCLOCKWISE_LOADING
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

private:
    const byte LOADING_ANIMATION_SEGMENT_SEQUENCE[8][2] = {
        {TwoDigitDisplay::TOP_SEGMENT, 0},           // Top left segment on dozens display
        {0, TwoDigitDisplay::TOP_SEGMENT},           // Top right segment on units display
        {0, TwoDigitDisplay::UPPER_RIGHT_SEGMENT},   // Upper right segment on units display
        {0, TwoDigitDisplay::LOWER_RIGHT_SEGMENT},   // Lower right segment on units display
        {0, TwoDigitDisplay::BOTTOM_SEGMENT},        // Bottom right segment on units display
        {TwoDigitDisplay::BOTTOM_SEGMENT, 0},        // Bottom left segment on dozens display
        {TwoDigitDisplay::LOWER_LEFT_SEGMENT, 0},    // Lower left segment on dozens display
        {TwoDigitDisplay::UPPER_LEFT_SEGMENT, 0}     // Upper left segment on dozens display
    };

public:
    DisplayAnimator();
    void setup(TwoDigitDisplay* displayInstance);
    void animateTransition(int fromNumber, int toNumber, unsigned long durationPer10Units);
    void animateClockwiseLoading(unsigned long segmentDurationMs);
    void animateCounterClockwiseLoading(unsigned long segmentDurationMs);
    void stop();
    void update();
    unsigned long getElapsedTime() const;
    AnimationType getCurrentAnimation() const;

private:
  void updateTransitionAnimation(const unsigned long currentTime);
  void updateClockwiseLoadingAnimation(const unsigned long currentTime);
  void updateCounterClockwiseLoadingAnimation(const unsigned long currentTime);
};
