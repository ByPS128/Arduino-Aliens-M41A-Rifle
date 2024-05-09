#include "M41aSimulator.h"

M41aSimulator simulator;

void setup ()
{
  simulator.setup();
}

void loop ()
{
  simulator.update();
}
