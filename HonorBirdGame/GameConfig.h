#pragma once

#include "GameTypes.h"
#include <string>

BirdStats getBirdStats(BirdType type);
std::string birdTypeToString(BirdType type);
sf::Color getEnemyColor(EnemyType type);
LevelData loadLevel(int level);
