#include "RenderSystem.h"

void drawEnemies(sf::RenderWindow& window, const std::vector<Enemy>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (!enemy.alive)
        {
            continue;
        }

        window.draw(enemy.shape);

        if (enemy.hitFlashTimer > 0.f)
        {
            sf::CircleShape flash = enemy.shape;
            flash.setFillColor(sf::Color(255, 255, 255, 120));
            window.draw(flash);
        }
    }
}

void drawBlocks(sf::RenderWindow& window, const std::vector<Block>& blocks)
{
    for (const Block& block : blocks)
    {
        if (!block.alive)
        {
            continue;
        }

        window.draw(block.shape);

        if (block.hitFlashTimer > 0.f)
        {
            sf::RectangleShape flash = block.shape;
            flash.setFillColor(sf::Color(255, 255, 255, 120));
            window.draw(flash);
        }
    }
}
