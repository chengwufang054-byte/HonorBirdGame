#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <string>

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

struct BirdStats
{
    sf::Color color;
    float launchPower;
    int blockDamage;
};

struct Enemy
{
    sf::CircleShape shape;
    bool enabled = false;
    bool alive = false;
};

struct Block
{
    sf::RectangleShape shape;
    int maxHp = 2;    
    int hp = 2;
    bool enabled = false;
    bool alive = false;
    bool collisionHandled = false;
};

struct LevelData
{
    sf::Vector2f birdStartPos;
    std::array<sf::Vector2f, 3> enemyPositions;
    std::array<bool, 3> enemyEnabled;
    std::array<sf::Vector2f, 3> blockPositions;
    std::array<bool, 3> blockEnabled;
    std::array<int, 3> blockHp;
    int birdsCount = 3;
    bool needBothSupportsForBeam = false;
};

struct RuntimeState
{
    GameState gameState = GameState::StartMenu;
    int currentLevel = 1;
    int nextLevel = 1;

    bool birdReady = true;
    bool isDragging = false;
    bool roundOver = false;

    bool enemy2Dropped = false;
    bool enemy2SupportedByBeam = true;
    bool beamFalling = false;

    int birdsLeft = 3;

    float birdSpeedX = 0.f;
    float birdSpeedY = 0.f;
    float gravity = 0.0012f;
    float maxDragDistance = 120.f;

    BirdType currentBirdType = BirdType::Normal;
    BirdStats currentBirdStats{ sf::Color::Red, 0.03f, 1 };
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
    }
    else if (type == BirdType::Charlotte)
    {
        stats.color = sf::Color(255, 120, 180);
        stats.launchPower = 0.035f;
        stats.blockDamage = 1;
    }
    else if (type == BirdType::Mulan)
    {
        stats.color = sf::Color(255, 200, 80);
        stats.launchPower = 0.028f;
        stats.blockDamage = 2;
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

LevelData loadLevel(int level)
{
    LevelData data{};

    if (level == 1)
    {
        data.birdStartPos = sf::Vector2f(80.f, 320.f);

        data.enemyPositions = {
            sf::Vector2f(520.f, 440.f),
            sf::Vector2f(500.f, 360.f),
            sf::Vector2f(-200.f, -200.f) };

        data.enemyEnabled = { true, true, false };

        data.blockPositions = {
            sf::Vector2f(470.f, 420.f),
            sf::Vector2f(450.f, 400.f),
            sf::Vector2f(-200.f, -200.f) };

        data.blockEnabled = { true, true, false };
        data.blockHp = { 2, 2, 3 };
        data.birdsCount = 3;
        data.needBothSupportsForBeam = false;
    }
    else if (level == 2)
    {
        data.birdStartPos = sf::Vector2f(80.f, 320.f);

        data.enemyPositions = {
            sf::Vector2f(600.f, 440.f),
            sf::Vector2f(590.f, 320.f),
            sf::Vector2f(630.f, 440.f) };

        data.enemyEnabled = { true, true, true };

        data.blockPositions = {
            sf::Vector2f(540.f, 420.f),
            sf::Vector2f(520.f, 360.f),
            sf::Vector2f(650.f, 420.f) };

        data.blockEnabled = { true, true, true };
        data.blockHp = { 2, 2, 3 };
        data.birdsCount = 2;
        data.needBothSupportsForBeam = true;
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
    std::array<Enemy, 3>& enemies,
    std::array<Block, 3>& blocks,
    RuntimeState& state)
{
    bird.setPosition(level.birdStartPos);
    applyBirdStats(bird, state, BirdType::Normal);

    state.birdReady = true;
    state.isDragging = false;
    state.roundOver = false;
    state.enemy2Dropped = false;
    state.enemy2SupportedByBeam = true;
    state.beamFalling = false;
    state.birdsLeft = level.birdsCount;
    state.birdSpeedX = 0.f;
    state.birdSpeedY = 0.f;

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        enemies[i].enabled = level.enemyEnabled[i];
        enemies[i].alive = level.enemyEnabled[i];
        enemies[i].shape.setPosition(level.enemyPositions[i]);
    }

    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        blocks[i].enabled = level.blockEnabled[i];
        blocks[i].alive = level.blockEnabled[i];
        blocks[i].maxHp = level.blockHp[i];
        blocks[i].hp = level.blockHp[i];
        blocks[i].collisionHandled = false;
        blocks[i].shape.setPosition(level.blockPositions[i]);
        blocks[i].shape.setFillColor(sf::Color(120, 90, 60));
    }
}

// ==================== 4. 逻辑辅助函数区 ====================

bool hasRemainingEnemies(const std::array<Enemy, 3>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.enabled && enemy.alive)
        {
            return true;
        }
    }
    return false;
}

bool allEnemiesDefeated(const std::array<Enemy, 3>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.enabled && enemy.alive)
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
    if (!block.enabled || !block.alive)
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

void checkBirdHitEnemies(const sf::CircleShape& bird, std::array<Enemy, 3>& enemies)
{
    const float birdCenterX = bird.getPosition().x + 20.f;
    const float birdCenterY = bird.getPosition().y + 20.f;

    for (Enemy& enemy : enemies)
    {
        if (!enemy.enabled || !enemy.alive)
        {
            continue;
        }

        const float enemyCenterX = enemy.shape.getPosition().x + 20.f;
        const float enemyCenterY = enemy.shape.getPosition().y + 20.f;
        const float dx = birdCenterX - enemyCenterX;
        const float dy = birdCenterY - enemyCenterY;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= 40.f)
        {
            enemy.alive = false;
        }
    }
}

void drawEnemies(sf::RenderWindow& window, const std::array<Enemy, 3>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (enemy.enabled && enemy.alive)
        {
            window.draw(enemy.shape);
        }
    }
}

void drawBlocks(sf::RenderWindow& window, const std::array<Block, 3>& blocks)
{
    for (const Block& block : blocks)
    {
        if (block.enabled && block.alive)
        {
            window.draw(block.shape);
        }
    }
}

// ==================== 5. 主程序 ====================

int main()
{
    // ----- 窗口 / 字体 / 文本 -----
    sf::RenderWindow window(sf::VideoMode(800, 600), "HonorBirdGame");

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
    sf::RectangleShape ground(sf::Vector2f(800.f, 100.f));
    ground.setPosition(0.f, 500.f);
    ground.setFillColor(sf::Color(80, 180, 80));

    sf::RectangleShape slingPost(sf::Vector2f(12.f, 110.f));
    slingPost.setPosition(60.f, 290.f);
    slingPost.setFillColor(sf::Color(139, 69, 19));

    // ----- 关卡 / 运行时 -----
    RuntimeState state;
    LevelData currentLevelData = loadLevel(1);

    // ----- bird -----
    sf::CircleShape bird(20.f);
    bird.setOutlineThickness(3.f);
    bird.setOutlineColor(sf::Color::Black);

    // ----- enemies -----
    std::array<Enemy, 3> enemies;
    const std::array<sf::Color, 3> enemyColors = {
        sf::Color(160, 80, 200),
        sf::Color(255, 170, 0),
        sf::Color(80, 160, 255) };

    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        enemies[i].shape = sf::CircleShape(20.f);
        enemies[i].shape.setOutlineThickness(3.f);
        enemies[i].shape.setOutlineColor(sf::Color::Black);
        enemies[i].shape.setFillColor(enemyColors[i]);
    }

    // ----- blocks -----
    std::array<Block, 3> blocks;
    blocks[0].shape = sf::RectangleShape(sf::Vector2f(30.f, 80.f));   // 左支撑
    blocks[1].shape = sf::RectangleShape(sf::Vector2f(120.f, 20.f));  // 横梁
    blocks[2].shape = sf::RectangleShape(sf::Vector2f(30.f, 80.f));   // 右支撑

    for (Block& block : blocks)
    {
        block.shape.setFillColor(sf::Color(120, 90, 60));
    }

    // 先按第一关重置一次，确保所有对象状态统一
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
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        if (state.birdReady && !state.isDragging && state.birdsLeft > 0)
                        {
                            state.birdsLeft--;
                            state.birdReady = false;
                            state.birdSpeedX = 0.45f;
                            state.birdSpeedY = -0.55f;
                            state.roundOver = false;
                        }
                    }

                    if (event.key.code == sf::Keyboard::N)
                    {
                        if (state.roundOver && state.birdsLeft > 0 && hasRemainingEnemies(enemies))
                        {
                            bird.setPosition(currentLevelData.birdStartPos);
                            state.birdReady = true;
                            state.isDragging = false;
                            state.roundOver = false;
                            state.birdSpeedX = 0.f;
                            state.birdSpeedY = 0.f;

                            if (state.currentLevel == 1)
                            {
                                applyBirdStats(bird, state, BirdType::Normal);
                            }
                            else if (state.currentLevel == 2)
                            {
                                if (state.birdsLeft == 1)
                                {
                                    applyBirdStats(bird, state, BirdType::Mulan);
                                }
                                else
                                {
                                    applyBirdStats(bird, state, BirdType::Normal);
                                }
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
                    sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

                    if (bird.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        state.isDragging = true;
                    }
                }
            }

            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                state.isDragging && event.type == sf::Event::MouseMoved)
            {
                sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
                sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

                sf::Vector2f desiredPos(mousePos.x - 20.f, mousePos.y - 20.f);

                if (desiredPos.x > currentLevelData.birdStartPos.x)
                {
                    desiredPos.x = currentLevelData.birdStartPos.x;
                }

                if (desiredPos.y < 240.f)
                {
                    desiredPos.y = 240.f;
                }

                sf::Vector2f offset = desiredPos - currentLevelData.birdStartPos;
                float distance = std::sqrt(offset.x * offset.x + offset.y * offset.y);

                if (distance > state.maxDragDistance && distance > 0.f)
                {
                    offset.x = offset.x / distance * state.maxDragDistance;
                    offset.y = offset.y / distance * state.maxDragDistance;
                    desiredPos = currentLevelData.birdStartPos + offset;
                }

                bird.setPosition(desiredPos);
            }

            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                state.isDragging && event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (state.birdsLeft > 0)
                    {
                        sf::Vector2f dragOffset = bird.getPosition() - currentLevelData.birdStartPos;
                        state.birdSpeedX = -dragOffset.x * state.launchPower;
                        state.birdSpeedY = -dragOffset.y * state.launchPower;

                        bird.setPosition(currentLevelData.birdStartPos);
                        state.birdsLeft--;
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
                    bird.move(-0.2f, 0.f);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    bird.move(0.2f, 0.f);
                }

                if (bird.getPosition().x < 40.f)
                {
                    bird.setPosition(40.f, bird.getPosition().y);
                }
                if (bird.getPosition().x > 160.f)
                {
                    bird.setPosition(160.f, bird.getPosition().y);
                }
            }

            if (!state.birdReady)
            {
                bird.move(state.birdSpeedX, state.birdSpeedY);
                state.birdSpeedY += state.gravity;

                if (bird.getPosition().y + 40.f >= 500.f)
                {
                    bird.setPosition(bird.getPosition().x, 460.f);
                    state.birdSpeedX = 0.f;
                    state.birdSpeedY = 0.f;
                    bird.setFillColor(sf::Color(150, 150, 150));
                    state.roundOver = true;
                }

                handleBirdHitBlock(bird, blocks[0], state);
                handleBirdHitBlock(bird, blocks[1], state);
                handleBirdHitBlock(bird, blocks[2], state);

                if (!blocks[1].alive)
                {
                    state.enemy2SupportedByBeam = false;
                }

                checkBirdHitEnemies(bird, enemies);
            }

            // 横梁下落规则
            if (!currentLevelData.needBothSupportsForBeam)
            {
                if (!blocks[0].alive && blocks[1].alive)
                {
                    state.beamFalling = true;
                }
            }
            else
            {
                if (!blocks[0].alive && !blocks[2].alive && blocks[1].alive)
                {
                    state.beamFalling = true;
                }
            }

            // enemy2 随横梁下落
            if (state.beamFalling && blocks[1].alive)
            {
                if (blocks[1].shape.getPosition().y < 480.f)
                {
                    blocks[1].shape.move(0.f, 0.25f);

                    if (blocks[1].shape.getPosition().y > 480.f)
                    {
                        blocks[1].shape.setPosition(blocks[1].shape.getPosition().x, 480.f);
                    }

                    if (enemies[1].alive && state.enemy2SupportedByBeam)
                    {
                        enemies[1].shape.setPosition(
                            enemies[1].shape.getPosition().x,
                            blocks[1].shape.getPosition().y - 40.f);
                    }
                }
            }

            // 横梁砸死敌人
            if (blocks[1].alive && enemies[0].alive &&
                blocks[1].shape.getGlobalBounds().intersects(enemies[0].shape.getGlobalBounds()))
            {
                enemies[0].alive = false;
            }

            if (enemies[2].enabled && blocks[1].alive && enemies[2].alive &&
                blocks[1].shape.getGlobalBounds().intersects(enemies[2].shape.getGlobalBounds()))
            {
                enemies[2].alive = false;
            }

            // 横梁没了以后 enemy2 自己掉落
            if (!blocks[1].alive && enemies[1].alive && !state.enemy2SupportedByBeam)
            {
                if (enemies[1].shape.getPosition().y < 460.f)
                {
                    enemies[1].shape.move(0.f, 0.3f);

                    if (enemies[1].shape.getPosition().y > 460.f)
                    {
                        enemies[1].shape.setPosition(enemies[1].shape.getPosition().x, 460.f);
                    }
                }
                else if (!state.enemy2Dropped)
                {
                    enemies[1].alive = false;
                    state.enemy2Dropped = true;
                }
            }

            // 场景是否仍在运动
            bool sceneStillMoving = false;

            if (state.beamFalling && blocks[1].alive && blocks[1].shape.getPosition().y < 480.f)
            {
                sceneStillMoving = true;
            }

            if (!blocks[1].alive && enemies[1].alive && !state.enemy2SupportedByBeam &&
                enemies[1].shape.getPosition().y < 460.f)
            {
                sceneStillMoving = true;
            }

            // 胜负判断
            if (allEnemiesDefeated(enemies))
            {
                if (state.currentLevel == 1)
                {
                    state.nextLevel = 2;
                    state.gameState = GameState::LevelClear;
                }
                else if (state.currentLevel == 2)
                {
                    state.gameState = GameState::Win;
                }
            }

            if (state.roundOver && state.birdsLeft == 0 && !sceneStillMoving)
            {
                state.gameState = GameState::Lose;
            }

            // 文本提示
            if (state.roundOver && state.birdsLeft > 0)
            {
                window.setTitle("HonorBirdGame - Press N for next bird");
            }
            else
            {
                window.setTitle("HonorBirdGame");
            }

            if (state.roundOver && state.birdsLeft > 0)
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
                    "  Birds Left: " + std::to_string(state.birdsLeft) +
                    "  Bird: " + birdTypeToString(state.currentBirdType));
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
                    sf::Vertex(sf::Vector2f(currentLevelData.birdStartPos.x + 20.f, currentLevelData.birdStartPos.y + 20.f), sf::Color::Black),
                    sf::Vertex(sf::Vector2f(bird.getPosition().x + 20.f, bird.getPosition().y + 20.f), sf::Color::Black) };

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
