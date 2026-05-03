#pragma once

#include "GameTypes.h"
#include <vector>

void applyBirdStats(sf::CircleShape& bird, RuntimeState& state, BirdType type);
void resetLevel(const LevelData& level, sf::CircleShape& bird, std::vector<Enemy>& enemies, std::vector<Block>& blocks, RuntimeState& state);

sf::Vector2f getCircleCenter(const sf::CircleShape& shape);
sf::Vector2f getRectCenter(const sf::RectangleShape& shape);
void applyEnemyDamage(Enemy& enemy, int damage);

bool hasRemainingEnemies(const std::vector<Enemy>& enemies);
bool allEnemiesDefeated(const std::vector<Enemy>& enemies);
void handleBirdHitBlock(const sf::CircleShape& bird, Block& block, RuntimeState& state);
void checkBirdHitEnemies(const sf::CircleShape& bird, std::vector<Enemy>& enemies, const RuntimeState& state);
bool areAllSupportsDestroyed(const Block& block, const std::vector<Block>& blocks);
void updateBeamFallingState(std::vector<Block>& blocks);
bool hasEnemyBeenDamagedByBlock(const Block& block, int enemyIndex);
void checkFallingBlocksHitEnemies(std::vector<Block>& blocks, std::vector<Enemy>& enemies);
void updateFallingBlocks(std::vector<Block>& blocks, float gameplayScale,float dt);
bool hasPendingResolution(const std::vector<Block>& blocks);
void updateGameResult(const std::vector<Enemy>& enemies, RuntimeState& state, bool pendingResolution);
