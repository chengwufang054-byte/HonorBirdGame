#pragma once

#include "GameTypes.h"
#include <vector>

void drawEnemies(sf::RenderWindow& window, const std::vector<Enemy>& enemies);
void drawBlocks(sf::RenderWindow& window, const std::vector<Block>& blocks);
