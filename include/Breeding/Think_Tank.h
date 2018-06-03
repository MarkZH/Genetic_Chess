#ifndef THINK_TANK_H
#define THINK_TANK_H

#include <string>
#include <vector>

#include "Players/Neural_AI.h"

void think_tank(const std::string& load_file);

using Think_Tank = std::vector<Neural_AI>;
std::vector<Think_Tank> load_think_tank_file(const std::string& load_file);

#endif // THINK_TANK_H
