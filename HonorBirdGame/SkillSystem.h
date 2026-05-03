#pragma once

#include <SFML/Graphics.hpp>
#include "GameTypes.h"
#include <vector>

//夏洛特技能函数声明
void applyCharlotteAreaSkill(const sf::CircleShape& bird, std::vector<Enemy>& enemies, std::vector<Block>& blocks, RuntimeState& state);

void drawPentagram(sf::RenderWindow& window, const sf::Vector2f& center, float outerRadius, const sf::Color& color);
sf::ConvexShape makeRoundedPetal(const sf::Vector2f& center, float angleDeg, float length, float halfWidth, const sf::Color& color);
void drawCharlotteSkillEffect(sf::RenderWindow& window, const RuntimeState& state);

//花木兰技能函数声明
bool initMulanSwordQi(RuntimeState& state, const std::string& texturePath);//程序开始时加载剑气贴图；

void spawnMulanSwordQi(
    const sf::CircleShape& bird,
    RuntimeState& state);

void updateMulanSwordQi(
    std::vector<Enemy>& enemies,
    std::vector<Block>& blocks,
    RuntimeState& state,
    float dt);

void applySwordQiDamageToBlock(Block& block, int damage);