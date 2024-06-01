#include "SimulatorState.h"

SimulatorState::SimulatorState()
    : bulletsCount(0), grenadeLoaded(false) {
}

int SimulatorState::getBulletsCount() const {
    return bulletsCount;
}

void SimulatorState::setBulletsCount(int count) {
    bulletsCount = count;
}

void SimulatorState::decrementBulletsCount() {
    if (bulletsCount > 0) {
        bulletsCount--;
    }
}

bool SimulatorState::isGrenadeLoaded() const {
    return grenadeLoaded;
}

void SimulatorState::setGrenadeLoaded(bool loaded) {
    grenadeLoaded = loaded;
}

void SimulatorState::toggleGrenadeLoaded() {
    grenadeLoaded = !grenadeLoaded;
}
