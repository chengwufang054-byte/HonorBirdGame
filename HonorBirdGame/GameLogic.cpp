#include "GameLogic.h"
#include "GameConfig.h"

#include <algorithm>
#include <cmath>

void applyBirdStats(sf::CircleShape& bird, RuntimeState& state, BirdType type)
{
    state.currentBirdType = type;
    state.currentBirdStats = getBirdStats(type);
    state.launchPower = state.currentBirdStats.launchPower;
    bird.setFillColor(state.currentBirdStats.color);
}

void resetLevel(
    const LevelData& level,
    sf::CircleShape& bird,
    std::vector<Enemy>& enemies,
    std::vector<Block>& blocks,
    RuntimeState& state)
{
    state.currentBirdIndex = 0;
    state.birdReady = true;
    state.isDragging = false;
    state.roundOver = false;
    state.skillUsedThisBird = false;
    state.skillSequenceActive = false;
    state.skillDamageApplied = false;
    state.skillEffectTimer = 0.f;
    state.skillEffectDuration = 0.92f;
    state.skillVoiceDuration = 1.20f;
  
    state.mulanSkillStage = 0;
	state.mulanSkillLineTimer = 0.f;
    state.mulanFreezeTimer = 0.f;
    state.mulanSwordQi.active = false;
    state.mulanSwordQi.lifeTimer = 0.f;
    
    state.birdsRemaining = static_cast<int>(level.birdQueue.size());
    state.birdSpeedX = 0.f;
    state.birdSpeedY = 0.f;
    state.preLaunchViewOffsetX = 0.f;

    bird.setPosition(level.birdStartPos);
    applyBirdStats(bird, state, level.birdQueue[state.currentBirdIndex]);

    enemies.clear();
    for (const EnemySpawnData& spawn : level.enemySpawns)
    {
        Enemy enemy;
        enemy.type = spawn.type;
        enemy.maxHp = spawn.hp;
        enemy.hp = spawn.hp;
        enemy.alive = true;
        enemy.hitFlashTimer = 0.f;

        const float radius = (spawn.type == EnemyType::Boss) ? 46.f : 30.f;
        enemy.shape = sf::CircleShape(radius);
        enemy.shape.setPosition(spawn.position);
        enemy.shape.setOutlineThickness(3.f);
        enemy.shape.setOutlineColor(sf::Color::Black);
        enemy.shape.setFillColor(getEnemyColor(spawn.type));

        enemies.push_back(enemy);
    }

    blocks.clear();
    for (const BlockSpawnData& spawn : level.blockSpawns)
    {
        Block block;
        block.type = spawn.type;
        block.maxHp = spawn.hp;
        block.hp = spawn.hp;
        block.alive = true;
        block.collisionHandled = false;
        block.falling = false;
        block.canFall = spawn.canFall;
        block.fallDamage = spawn.fallDamage;
        block.supportIndices = spawn.supportIndices;
        block.damagedEnemyIndices.clear();
        block.isKeySupport = spawn.isKeySupport;
        block.hitFlashTimer = 0.f;

        block.shape = sf::RectangleShape(spawn.size);
        block.shape.setPosition(spawn.position);
        block.shape.setFillColor(sf::Color(120, 90, 60));

        blocks.push_back(block);
    }
}

sf::Vector2f getCircleCenter(const sf::CircleShape& shape)
{
    const float radius = shape.getRadius();
    return sf::Vector2f(shape.getPosition().x + radius, shape.getPosition().y + radius);
}

sf::Vector2f getRectCenter(const sf::RectangleShape& shape)
{
    return sf::Vector2f(
        shape.getPosition().x + shape.getSize().x * 0.5f,
        shape.getPosition().y + shape.getSize().y * 0.5f
    );
}

void applyEnemyDamage(Enemy& enemy, int damage)
{
    if (!enemy.alive)
    {
        return;
    }

    enemy.hp -= damage;
    if (enemy.hp <= 0)
    {
        enemy.hp = 0;
        enemy.alive = false;
    }
}

bool hasRemainingEnemies(const std::vector<Enemy>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.alive)
        {
            return true;
        }
    }
    return false;
}

bool allEnemiesDefeated(const std::vector<Enemy>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.alive)
        {
            return false;
        }
    }
    return true;
}

void handleBirdHitBlock(
    const sf::CircleShape& bird,
    Block& block,
    RuntimeState& state)
{
    if (!block.alive)
    {
        block.collisionHandled = false;
        return;
    }

    if (bird.getGlobalBounds().intersects(block.shape.getGlobalBounds()))
    {
        if (!block.collisionHandled)
        {
            block.hp -= state.currentBirdStats.blockDamage;
            state.birdSpeedX *= 0.7f;
            state.birdSpeedY *= 0.7f;

            if (block.hp == 2)
            {
                block.shape.setFillColor(sf::Color(100, 75, 50));
            }
            else if (block.hp == 1)
            {
                block.shape.setFillColor(sf::Color(80, 60, 40));
            }

            if (block.hp <= 0)
            {
                block.hp = 0;
                block.alive = false;
            }

            block.collisionHandled = true;
        }
    }
    else
    {
        block.collisionHandled = false;
    }
}

void checkBirdHitEnemies(
    const sf::CircleShape& bird,
    std::vector<Enemy>& enemies,
    const RuntimeState& state)
{
    const sf::Vector2f birdCenter = getCircleCenter(bird);
    const float birdRadius = bird.getRadius();

    for (Enemy& enemy : enemies)
    {
        if (!enemy.alive)
        {
            continue;
        }

        const sf::Vector2f enemyCenter = getCircleCenter(enemy.shape);
        const float enemyRadius = enemy.shape.getRadius();
        const float dx = birdCenter.x - enemyCenter.x;
        const float dy = birdCenter.y - enemyCenter.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= birdRadius + enemyRadius)
        {
            applyEnemyDamage(enemy, state.currentBirdStats.enemyDamage);
        }
    }
}

bool areAllSupportsDestroyed(const Block& block, const std::vector<Block>& blocks)
{
    if (block.supportIndices.empty())
    {
        return false;
    }

    for (int supportIndex : block.supportIndices)
    {
        if (supportIndex < 0 || supportIndex >= static_cast<int>(blocks.size()))
        {
            return false;
        }

        if (blocks[supportIndex].alive)
        {
            return false;
        }
    }

    return true;
}

void updateBeamFallingState(std::vector<Block>& blocks)
{
    for (Block& block : blocks)
    {
        if (!block.alive || !block.canFall || block.falling)
        {
            continue;
        }

        if (areAllSupportsDestroyed(block, blocks))
        {
            block.falling = true;
        }
    }
}

bool hasEnemyBeenDamagedByBlock(const Block& block, int enemyIndex)
{
    return std::find(block.damagedEnemyIndices.begin(), block.damagedEnemyIndices.end(), enemyIndex) != block.damagedEnemyIndices.end();
}

void checkFallingBlocksHitEnemies(std::vector<Block>& blocks, std::vector<Enemy>& enemies)
{
    for (Block& block : blocks)
    {
        if (!block.alive || !block.falling)
        {
            continue;
        }

        for (int i = 0; i < static_cast<int>(enemies.size()); ++i)
        {
            if (!enemies[i].alive)
            {
                continue;
            }

            if (hasEnemyBeenDamagedByBlock(block, i))
            {
                continue;
            }

            if (block.shape.getGlobalBounds().intersects(enemies[i].shape.getGlobalBounds()))
            {
                applyEnemyDamage(enemies[i], block.fallDamage);
                block.damagedEnemyIndices.push_back(i);
            }
        }
    }
}

void updateFallingBlocks(std::vector<Block>& blocks, float gameplayScale,float dt)
{
    for (Block& block : blocks)
    {
        if (!block.alive || !block.falling)
        {
            continue;
        }

        block.shape.move(0.f, 0.6f * gameplayScale*dt);

        const float blockBottom = block.shape.getPosition().y + block.shape.getSize().y;
        if (blockBottom >= GROUND_TOP)
        {
            block.shape.setPosition(block.shape.getPosition().x, GROUND_TOP - block.shape.getSize().y);
            block.falling = false;
        }
    }
}

bool hasPendingResolution(const std::vector<Block>& blocks)
{
    for (const Block& block : blocks)
    {
        if (block.falling)
        {
            return true;
        }
    }
    return false;
}

void updateGameResult(
    const std::vector<Enemy>& enemies,
    RuntimeState& state,
    bool pendingResolution)
{
    if (allEnemiesDefeated(enemies))
    {
        if (state.currentLevel < 3)
        {
            state.nextLevel = state.currentLevel + 1;
            state.gameState = GameState::LevelClear;
        }
        else
        {
            state.gameState = GameState::Win;
        }
        return;
    }

    if (state.roundOver && state.birdsRemaining == 0 && !pendingResolution)
    {
        state.gameState = GameState::Lose;
    }
}
