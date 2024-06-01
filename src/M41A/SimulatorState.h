#pragma once

class SimulatorState {
public:
    SimulatorState();

    int getBulletsCount() const;
    void setBulletsCount(int count);
    void decrementBulletsCount();

    bool isGrenadeLoaded() const;
    void setGrenadeLoaded(bool loaded);
    void toggleGrenadeLoaded();

    // Další metody pro správu stavu simulátoru

private:
    int bulletsCount;
    bool grenadeLoaded;
    // Další proměnné pro uchování stavu simulátoru
};
