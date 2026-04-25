#include "GameConfig.h"

BirdStats getBirdStats(BirdType type)
{
    BirdStats stats{};

    if (type == BirdType::Normal)
    {
        stats.color = sf::Color::Red;
        stats.launchPower = 0.026f;
        stats.blockDamage = 1;
        stats.enemyDamage = 1;
    }
    else if (type == BirdType::Charlotte)
    {
        stats.color = sf::Color(255, 120, 180);
        stats.launchPower = 0.030f;
        stats.blockDamage = 1;
        stats.enemyDamage = 1;
    }
    else
    {
        stats.color = sf::Color(255, 200, 80);
        stats.launchPower = 0.024f;
        stats.blockDamage = 2;
        stats.enemyDamage = 2;
    }

    return stats;
}

std::string birdTypeToString(BirdType type)
{
    if (type == BirdType::Normal)
    {
        return "Normal";
    }
    if (type == BirdType::Charlotte)
    {
        return "Charlotte";
    }
    return "Mulan";
}

sf::Color getEnemyColor(EnemyType type)
{
    if (type == EnemyType::Boss)
    {
        return sf::Color(180, 60, 60);
    }
    return sf::Color(160, 80, 200);
}

LevelData loadLevel(int level)
{
    LevelData data{};
    data.birdStartPos = DEFAULT_BIRD_START;

    if (level == 1)
    {
        data.homeViewCenter = sf::Vector2f(835.f, 450.f);
        data.homeViewSize = sf::Vector2f(1450.f, 820.f);

        data.birdQueue = { BirdType::Normal, BirdType::Mulan, BirdType::Charlotte };

        data.enemySpawns = {
            {EnemyType::Normal, sf::Vector2f(1110.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(1560.f, 710.f), 1}
        };

        data.blockSpawns = {
            {BlockType::Pillar, sf::Vector2f(1050.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, false},
            {BlockType::Pillar, sf::Vector2f(1450.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Pillar, sf::Vector2f(1670.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Beam,   sf::Vector2f(1450.f, 560.f), sf::Vector2f(260.f, 40.f), 2, true,  2, {1, 2}, false}
        };
    }
    else if (level == 2)
    {
        data.homeViewCenter = sf::Vector2f(1025.f, 450.f);
        data.homeViewSize = sf::Vector2f(1850.f, 820.f);

        data.birdQueue = { BirdType::Normal, BirdType::Charlotte, BirdType::Mulan };

        data.enemySpawns = {
            {EnemyType::Normal, sf::Vector2f(1060.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(1710.f, 710.f), 1},
            {EnemyType::Boss,   sf::Vector2f(2550.f, 680.f), 5}
        };

        data.blockSpawns = {
            {BlockType::Pillar,      sf::Vector2f(1010.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, false},
            {BlockType::Pillar,      sf::Vector2f(1600.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Pillar,      sf::Vector2f(1860.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Beam,        sf::Vector2f(1600.f, 560.f), sf::Vector2f(300.f, 40.f), 2, true,  2, {1, 2}, false},
            {BlockType::DecoyPillar, sf::Vector2f(2320.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, false},
            {BlockType::Pillar,      sf::Vector2f(2450.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Pillar,      sf::Vector2f(2710.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}, true},
            {BlockType::Beam,        sf::Vector2f(2450.f, 560.f), sf::Vector2f(300.f, 40.f), 2, true,  2, {5, 6}, false}
        };
    }
    else
    {
        data.homeViewCenter = sf::Vector2f(1125.f, 450.f);
        data.homeViewSize = sf::Vector2f(2050.f, 820.f);

        data.birdQueue = {
            BirdType::Normal,
            BirdType::Charlotte,
            BirdType::Charlotte,
            BirdType::Mulan,
            BirdType::Mulan
        };

        data.enemySpawns = {
            {EnemyType::Normal, sf::Vector2f(1050.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(1550.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(2050.f, 710.f), 1},
            {EnemyType::Boss,   sf::Vector2f(2640.f, 680.f), 5}
        };

        data.blockSpawns = {
            {BlockType::Pillar,      sf::Vector2f(930.f, 600.f),  sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1190.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(930.f, 560.f),  sf::Vector2f(300.f, 40.f), 2, true,  2, {0, 1}},

            {BlockType::Pillar,      sf::Vector2f(1430.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1690.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(1430.f, 560.f), sf::Vector2f(300.f, 40.f), 2, true,  2, {3, 4}},

            {BlockType::Pillar,      sf::Vector2f(1930.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2190.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(1930.f, 560.f), sf::Vector2f(300.f, 40.f), 2, true,  2, {6, 7}},

            {BlockType::DecoyPillar, sf::Vector2f(2420.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2550.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2810.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(2550.f, 560.f), sf::Vector2f(300.f, 40.f), 2, true,  2, {10, 11}}
        };
    }

    return data;
}
