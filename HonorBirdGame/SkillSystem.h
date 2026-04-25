#pragma once

#include "GameTypes.h"
#include <vector>

void applyCharlotteAreaSkill(const sf::CircleShape& bird, std::vector<Enemy>& enemies, std::vector<Block>& blocks, RuntimeState& state);

void drawPentagram(sf::RenderWindow& window, const sf::Vector2f& center, float outerRadius, const sf::Color& color);
sf::ConvexShape makeRoundedPetal(const sf::Vector2f& center, float angleDeg, float length, float halfWidth, const sf::Color& color);
void drawCharlotteSkillEffect(sf::RenderWindow& window, const RuntimeState& state);
