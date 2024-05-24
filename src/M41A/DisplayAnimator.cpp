#include "DisplayAnimator.h"

DisplayAnimator::DisplayAnimator()
    : display(nullptr), startNumber(0), endNumber(0), startTime(0), durationPer10Units(0), animating(false),
      currentAnimation(NONE), currentSegment(0), segmentDuration(0), lastSegmentUpdateTime(0) {}

void DisplayAnimator::setup(TwoDigitDisplay* displayInstance) {
    display = displayInstance;
}

void DisplayAnimator::animateTransition(int fromNumber, int toNumber, unsigned long durationPer10Units) {
    startNumber = fromNumber;
    endNumber = toNumber;
    startTime = millis();
    this->durationPer10Units = durationPer10Units;

    currentAnimation = TRANSITION;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Transition animation started");
    #endif
}

void DisplayAnimator::animateClockwiseInit(unsigned long segmentDurationMs) {
    startTime = millis();
    segmentDuration = segmentDurationMs;
    currentSegment = 0;
    lastSegmentUpdateTime = millis(); // Initialize segment update timing
    currentAnimation = CLOCKWISE_INIT;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Clockwise initialization animation started");
    #endif
}

void DisplayAnimator::animateCounterClockwiseInit(unsigned long segmentDurationMs) {
    startTime = millis();
    segmentDuration = segmentDurationMs;
    currentSegment = 7; // Start from the last segment in the sequence
    lastSegmentUpdateTime = millis(); // Initialize segment update timing
    currentAnimation = COUNTERCLOCKWISE_INIT;
    animating = true;
    #ifdef _DEBUG
    Serial.println("Counterclockwise initialization animation started");
    #endif
}

void DisplayAnimator::stop() {
    animating = false;
    currentAnimation = NONE;
    #ifdef _DEBUG
    Serial.println("Animation stopped");
    #endif
}

void DisplayAnimator::update() {
    if (!animating || display == nullptr) return;

    unsigned long currentTime = millis();
    #ifdef _DEBUG
    Serial.print("Elapsed Time: ");
    Serial.println(getElapsedTime());
    #endif

    switch (currentAnimation) {
        case TRANSITION: {
            int distance = abs(endNumber - startNumber);
            unsigned long totalDuration = (distance * durationPer10Units) / 10;
            unsigned long elapsedTime = currentTime - startTime;

            if (elapsedTime >= totalDuration) {
                display->displayNumbers(endNumber / 10, endNumber % 10);
                animating = false;
                currentAnimation = NONE;
                #ifdef _DEBUG
                Serial.println("Transition animation completed");
                #endif
                return;
            }

            float progress = (float)elapsedTime / (float)totalDuration;
            int currentNumber = startNumber + progress * (endNumber - startNumber);

            display->displayNumbers(currentNumber / 10, currentNumber % 10);
            break;
        }
        case CLOCKWISE_INIT: {
            if (currentTime - lastSegmentUpdateTime >= segmentDuration) {
                lastSegmentUpdateTime = currentTime;
                currentSegment = (currentSegment + 1) % 8;
            }

            display->displaySegments(segmentSequence[currentSegment][0], segmentSequence[currentSegment][1]);
            break;
        }
        case COUNTERCLOCKWISE_INIT: {
            if (currentTime - lastSegmentUpdateTime >= segmentDuration) {
                lastSegmentUpdateTime = currentTime;
                currentSegment = (currentSegment - 1 + 8) % 8;
            }

            display->displaySegments(segmentSequence[currentSegment][0], segmentSequence[currentSegment][1]);
            break;
        }
        case NONE:
        default:
            break;
    }
}

unsigned long DisplayAnimator::getElapsedTime() const {
    return animating ? millis() - startTime : 0;
}

AnimationType DisplayAnimator::getCurrentAnimation() const {
    return currentAnimation;
}
