
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

// ==================== 0. 全局尺寸参数 ====================

const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

const float WORLD_WIDTH = 3000.f;
const float WORLD_HEIGHT = 900.f;

const float DEFAULT_VIEW_WIDTH = 1600.f;
const float DEFAULT_VIEW_HEIGHT = 900.f;

const float GROUND_TOP = 760.f;
const sf::Vector2f DEFAULT_VIEW_CENTER(800.f, 450.f);
const sf::Vector2f DEFAULT_BIRD_START(180.f, 700.f);

// ==================== 1. 基础类型定义 ====================

enum class GameState
{
    StartMenu,
    Playing,
    LevelClear,
    Win,
    Lose
};

enum class BirdType
{
    Normal,
    Charlotte,
    Mulan
};

enum class EnemyType
{
    Normal,
    Boss
};

enum class BlockType
{
    Pillar,
    Beam,
    DecoyPillar
};

struct BirdStats
{
    sf::Color color;
    float launchPower;
    int blockDamage;
    int enemyDamage;
};

struct Enemy
{
    sf::CircleShape shape;
    EnemyType type = EnemyType::Normal;
    int maxHp = 1;
    int hp = 1;
    bool alive = true;
};

struct Block
{
    sf::RectangleShape shape;
    BlockType type = BlockType::Pillar;
    int maxHp = 2;
    int hp = 2;
    bool alive = true;
    bool collisionHandled = false;
    bool falling = false;
    int fallDamage = 2;
    std::vector<int> supportIndices;
    std::vector<int> damagedEnemyIndices;
};

struct EnemySpawnData
{
    EnemyType type = EnemyType::Normal;
    sf::Vector2f position{ 0.f, 0.f };
    int hp = 1;
};

struct BlockSpawnData
{
    BlockType type = BlockType::Pillar;
    sf::Vector2f position{ 0.f, 0.f };
    sf::Vector2f size{ 40.f, 160.f };
    int hp = 2;
    bool canFall = false;
    int fallDamage = 2;
    std::vector<int> supportIndices;
};

struct LevelData
{
    sf::Vector2f birdStartPos = DEFAULT_BIRD_START;
    std::vector<BirdType> birdQueue;
    std::vector<EnemySpawnData> enemySpawns;
    std::vector<BlockSpawnData> blockSpawns;
};

struct RuntimeState
{
    GameState gameState = GameState::StartMenu;
    int currentLevel = 1;
    int nextLevel = 1;

    bool birdReady = true;
    bool isDragging = false;
    bool roundOver = false;

    int birdsRemaining = 0;
    int currentBirdIndex = 0;

    float birdSpeedX = 0.f;
    float birdSpeedY = 0.f;
    float gravity = 0.0012f;
    float maxDragDistance = 150.f;

    BirdType currentBirdType = BirdType::Normal;
    BirdStats currentBirdStats{ sf::Color::Red, 0.03f, 1, 1 };
    float launchPower = 0.03f;
};

// ==================== 2. 配置函数区 ====================

BirdStats getBirdStats(BirdType type)
{
    BirdStats stats{};

    if (type == BirdType::Normal)
    {
        stats.color = sf::Color::Red;
        stats.launchPower = 0.03f;
        stats.blockDamage = 1;
        stats.enemyDamage = 1;
    }
    else if (type == BirdType::Charlotte)
    {
        stats.color = sf::Color(255, 120, 180);
        stats.launchPower = 0.035f;
        stats.blockDamage = 1;
        stats.enemyDamage = 1;
    }
    else
    {
        stats.color = sf::Color(255, 200, 80);
        stats.launchPower = 0.028f;
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
        data.birdQueue = { BirdType::Normal, BirdType::Charlotte, BirdType::Mulan };

        data.enemySpawns = {
            {EnemyType::Normal, sf::Vector2f(1110.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(1775.f, 710.f), 1}
        };

        // 0=V1, 1=V2, 2=V3, 3=H1
        data.blockSpawns = {
            {BlockType::Pillar, sf::Vector2f(1050.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar, sf::Vector2f(1650.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar, sf::Vector2f(1910.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,   sf::Vector2f(1630.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {1, 2}}
        };
    }
    else if (level == 2)
    {
        data.birdQueue = { BirdType::Normal, BirdType::Charlotte, BirdType::Mulan };

        data.enemySpawns = {
            {EnemyType::Normal, sf::Vector2f(1060.f, 710.f), 1},
            {EnemyType::Normal, sf::Vector2f(1710.f, 710.f), 1},
            {EnemyType::Boss,   sf::Vector2f(2550.f, 680.f), 5}
        };

        // 0=V1, 1=V2, 2=V3, 3=H1, 4=V4, 5=V5, 6=V6, 7=H2
        data.blockSpawns = {
            {BlockType::Pillar,      sf::Vector2f(1010.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1600.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1860.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(1580.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {1, 2}},
            {BlockType::DecoyPillar, sf::Vector2f(2320.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2450.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2710.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(2430.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {5, 6}}
        };
    }
    else
    {
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

        // 0=V1,1=V2,2=H1, 3=V3,4=V4,5=H2, 6=V5,7=V6,8=H3, 9=V7,10=V8,11=V9,12=H4
        data.blockSpawns = {
            {BlockType::Pillar,      sf::Vector2f(930.f, 600.f),  sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1190.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(910.f, 560.f),  sf::Vector2f(320.f, 40.f), 2, true,  2, {0, 1}},

            {BlockType::Pillar,      sf::Vector2f(1430.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(1690.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(1410.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {3, 4}},

            {BlockType::Pillar,      sf::Vector2f(1930.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2190.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(1910.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {6, 7}},

            {BlockType::DecoyPillar, sf::Vector2f(2420.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2550.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Pillar,      sf::Vector2f(2810.f, 600.f), sf::Vector2f(40.f, 160.f), 2, false, 2, {}},
            {BlockType::Beam,        sf::Vector2f(2530.f, 560.f), sf::Vector2f(320.f, 40.f), 2, true,  2, {10, 11}}
        };
    }

    return data;
}

// ==================== 3. 初始化 / 重置函数区 ====================

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
    state.birdsRemaining = static_cast<int>(level.birdQueue.size());
    state.birdSpeedX = 0.f;
    state.birdSpeedY = 0.f;

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

        const float radius = (spawn.type == EnemyType::Boss) ? 40.f : 25.f;
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
        block.fallDamage = spawn.fallDamage;
        block.supportIndices = spawn.supportIndices;
        block.damagedEnemyIndices.clear();

        block.shape = sf::RectangleShape(spawn.size);
        block.shape.setPosition(spawn.position);
        block.shape.setFillColor(sf::Color(120, 90, 60));

        blocks.push_back(block);
    }
}

// ==================== 4. 逻辑辅助函数区 ====================

sf::Vector2f getCircleCenter(const sf::CircleShape& shape)
{
    const float radius = shape.getRadius();
    return sf::Vector2f(shape.getPosition().x + radius, shape.getPosition().y + radius);
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
        if (!block.alive || block.type != BlockType::Beam || block.falling)
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

void updateFallingBlocks(std::vector<Block>& blocks)
{
    for (Block& block : blocks)
    {
        if (!block.alive || !block.falling)
        {
            continue;
        }

        block.shape.move(0.f, 0.6f);

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

void drawEnemies(sf::RenderWindow& window, const std::vector<Enemy>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.alive)
        {
            window.draw(enemy.shape);
        }
    }
}

void drawBlocks(sf::RenderWindow& window, const std::vector<Block>& blocks)
{
    for (const Block& block : blocks)
    {
        if (block.alive)
        {
            window.draw(block.shape);
        }
    }
}

// ==================== 5. 主程序 ====================

int main()
{
    // ----- 窗口 / 视角 / 字体 / 文本 -----
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "HonorBirdGame");

    sf::View gameView(sf::FloatRect(0.f, 0.f, DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT));
    gameView.setCenter(DEFAULT_VIEW_CENTER);
    window.setView(gameView);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return -1;
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(20.f, 20.f);

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(42);
    titleText.setFillColor(sf::Color::Black);
    titleText.setPosition(200.f, 180.f);

    sf::Text startText;
    startText.setFont(font);
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color::Black);
    startText.setPosition(220.f, 280.f);

    sf::Text resultTitleText;
    resultTitleText.setFont(font);
    resultTitleText.setCharacterSize(42);
    resultTitleText.setFillColor(sf::Color::Black);
    resultTitleText.setPosition(250.f, 180.f);

    sf::Text resultHintText;
    resultHintText.setFont(font);
    resultHintText.setCharacterSize(24);
    resultHintText.setFillColor(sf::Color::Black);
    resultHintText.setPosition(210.f, 280.f);

    // ----- 静态场景 -----
    sf::RectangleShape ground(sf::Vector2f(WORLD_WIDTH, WORLD_HEIGHT - GROUND_TOP));
    ground.setPosition(0.f, GROUND_TOP);
    ground.setFillColor(sf::Color(80, 180, 80));

    sf::RectangleShape slingPost(sf::Vector2f(20.f, 180.f));
    slingPost.setPosition(140.f, 580.f);
    slingPost.setFillColor(sf::Color(139, 69, 19));

    // ----- 关卡 / 运行时 -----
    RuntimeState state;
    LevelData currentLevelData = loadLevel(1);

    // ----- bird -----
    sf::CircleShape bird(25.f);
    bird.setOutlineThickness(3.f);
    bird.setOutlineColor(sf::Color::Black);

    // ----- 运行时对象 -----
    std::vector<Enemy> enemies;
    std::vector<Block> blocks;

    resetLevel(currentLevelData, bird, enemies, blocks, state);

    while (window.isOpen())
    {
        // ==================== 事件处理区 ====================
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Enter)
                {
                    if (state.gameState == GameState::StartMenu)
                    {
                        state.currentLevel = 1;
                        currentLevelData = loadLevel(state.currentLevel);
                        resetLevel(currentLevelData, bird, enemies, blocks, state);
                        state.gameState = GameState::Playing;
                    }
                    else if (state.gameState == GameState::LevelClear)
                    {
                        state.currentLevel = state.nextLevel;
                        currentLevelData = loadLevel(state.currentLevel);
                        resetLevel(currentLevelData, bird, enemies, blocks, state);
                        state.gameState = GameState::Playing;
                    }
                }

                if (state.gameState == GameState::Playing)
                {
                    // 发射当前 bird：一旦发射，birdsRemaining 减 1
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        if (state.birdReady && !state.isDragging && state.birdsRemaining > 0)
                        {
                            state.birdsRemaining--;
                            state.birdReady = false;
                            state.birdSpeedX = 15.f * state.launchPower;
                            state.birdSpeedY = -18.f * state.launchPower;
                            state.roundOver = false;
                        }
                    }

                    // N 键：切到队列中的下一只 bird，不减少 birdsRemaining
                    if (event.key.code == sf::Keyboard::N)
                    {
                        if (state.roundOver && state.birdsRemaining > 0 && hasRemainingEnemies(enemies))
                        {
                            bird.setPosition(currentLevelData.birdStartPos);
                            state.birdReady = true;
                            state.isDragging = false;
                            state.roundOver = false;
                            state.birdSpeedX = 0.f;
                            state.birdSpeedY = 0.f;

                            if (state.currentBirdIndex + 1 < static_cast<int>(currentLevelData.birdQueue.size()))
                            {
                                state.currentBirdIndex++;
                                applyBirdStats(bird, state, currentLevelData.birdQueue[state.currentBirdIndex]);
                            }
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::R)
                {
                    if (state.gameState == GameState::Playing ||
                        state.gameState == GameState::Win ||
                        state.gameState == GameState::Lose)
                    {
                        state.currentLevel = 1;
                        currentLevelData = loadLevel(state.currentLevel);
                        resetLevel(currentLevelData, bird, enemies, blocks, state);
                        state.gameState = GameState::StartMenu;
                    }
                }
            }

            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    const sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    const sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

                    if (bird.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        state.isDragging = true;
                    }
                }
            }

            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                state.isDragging && event.type == sf::Event::MouseMoved)
            {
                const sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
                const sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

                sf::Vector2f desiredPos(mousePos.x - bird.getRadius(), mousePos.y - bird.getRadius());

                if (desiredPos.x > currentLevelData.birdStartPos.x)
                {
                    desiredPos.x = currentLevelData.birdStartPos.x;
                }

                if (desiredPos.y < currentLevelData.birdStartPos.y - 120.f)
                {
                    desiredPos.y = currentLevelData.birdStartPos.y - 120.f;
                }

                sf::Vector2f offset = desiredPos - currentLevelData.birdStartPos;
                const float distance = std::sqrt(offset.x * offset.x + offset.y * offset.y);

                if (distance > state.maxDragDistance && distance > 0.f)
                {
                    offset.x = offset.x / distance * state.maxDragDistance;
                    offset.y = offset.y / distance * state.maxDragDistance;
                    desiredPos = currentLevelData.birdStartPos + offset;
                }

                bird.setPosition(desiredPos);
            }

            // 拖拽发射当前 bird：一旦发射，birdsRemaining 减 1
            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                state.isDragging && event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (state.birdsRemaining > 0)
                    {
                        const sf::Vector2f dragOffset = bird.getPosition() - currentLevelData.birdStartPos;
                        state.birdSpeedX = -dragOffset.x * state.launchPower;
                        state.birdSpeedY = -dragOffset.y * state.launchPower;

                        bird.setPosition(currentLevelData.birdStartPos);
                        state.birdsRemaining--;
                        state.birdReady = false;
                        state.isDragging = false;
                        state.roundOver = false;
                    }
                }
            }
        }

        // ==================== 更新区 ====================
        if (state.gameState == GameState::Playing)
        {
            if (state.birdReady)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    bird.move(-0.3f, 0.f);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    bird.move(0.3f, 0.f);
                }

                if (bird.getPosition().x < 120.f)
                {
                    bird.setPosition(120.f, bird.getPosition().y);
                }
                if (bird.getPosition().x > 260.f)
                {
                    bird.setPosition(260.f, bird.getPosition().y);
                }
            }

            if (!state.birdReady)
            {
                bird.move(state.birdSpeedX, state.birdSpeedY);
                state.birdSpeedY += state.gravity;

                if (bird.getPosition().y + bird.getRadius() * 2.f >= GROUND_TOP)
                {
                    bird.setPosition(bird.getPosition().x, GROUND_TOP - bird.getRadius() * 2.f);
                    state.birdSpeedX = 0.f;
                    state.birdSpeedY = 0.f;
                    bird.setFillColor(sf::Color(150, 150, 150));
                    state.roundOver = true;
                }

                for (Block& block : blocks)
                {
                    handleBirdHitBlock(bird, block, state);
                }

                checkBirdHitEnemies(bird, enemies, state);
            }

            updateBeamFallingState(blocks);
            updateFallingBlocks(blocks);
            checkFallingBlocksHitEnemies(blocks, enemies);

            const bool hasPendingResolutionNow = hasPendingResolution(blocks);
            updateGameResult(enemies, state, hasPendingResolutionNow);

            // 文本提示
            if (state.roundOver && state.birdsRemaining > 0)
            {
                window.setTitle("HonorBirdGame - Press N for next bird");
            }
            else
            {
                window.setTitle("HonorBirdGame");
            }

            if (state.roundOver && state.birdsRemaining > 0)
            {
                infoText.setString("Press N for next bird");
            }
            else if (state.isDragging)
            {
                infoText.setString("Release mouse to Launch");
            }
            else if (state.birdReady)
            {
                infoText.setString(
                    "Level " + std::to_string(state.currentLevel) +
                    "  Birds Remaining: " + std::to_string(state.birdsRemaining) +
                    "  Bird: " + birdTypeToString(state.currentBirdType) +
                    "  Slot: " + std::to_string(state.currentBirdIndex + 1));
            }
            else
            {
                infoText.setString("");
            }
        }

        if (state.gameState == GameState::Win)
        {
            window.setTitle("HonorBirdGame - Win");
            resultTitleText.setString("You Win!");
            resultHintText.setString("Press R to Return to Menu");
        }
        else if (state.gameState == GameState::Lose)
        {
            window.setTitle("HonorBirdGame - Lose");
            resultTitleText.setString("You Lose!");
            resultHintText.setString("Press R to Return to Menu");
        }
        else if (state.gameState == GameState::StartMenu)
        {
            window.setTitle("HonorBirdGame - Start Menu");
            titleText.setString("Honor Bird Game");
            startText.setString("Press ENTER to Start");
        }
        else if (state.gameState == GameState::LevelClear)
        {
            window.setTitle("HonorBirdGame - Level Clear!");
            resultTitleText.setString("Level " + std::to_string(state.currentLevel) + " Clear!");
            resultHintText.setString("Press ENTER for Level " + std::to_string(state.nextLevel));
        }

        // ==================== 绘制区 ====================
        window.clear(sf::Color(135, 206, 235));

        if (state.gameState == GameState::StartMenu)
        {
            window.draw(titleText);
            window.draw(startText);
        }
        else if (state.gameState == GameState::Playing)
        {
            window.draw(ground);
            window.draw(slingPost);

            if (state.isDragging)
            {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(currentLevelData.birdStartPos.x + bird.getRadius(),
                                            currentLevelData.birdStartPos.y + bird.getRadius()), sf::Color::Black),
                    sf::Vertex(sf::Vector2f(bird.getPosition().x + bird.getRadius(),
                                            bird.getPosition().y + bird.getRadius()), sf::Color::Black)
                };

                window.draw(line, 2, sf::Lines);
            }

            window.draw(bird);
            drawBlocks(window, blocks);
            drawEnemies(window, enemies);
            window.draw(infoText);
        }
        else if (state.gameState == GameState::Win ||
            state.gameState == GameState::Lose ||
            state.gameState == GameState::LevelClear)
        {
            window.draw(resultTitleText);
            window.draw(resultHintText);
        }

        window.display();
    }

    return 0;
}
