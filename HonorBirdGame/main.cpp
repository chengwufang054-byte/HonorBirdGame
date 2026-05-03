#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <vector>

#include "GameTypes.h"
#include "GameConfig.h"
#include "GameLogic.h"
#include "SkillSystem.h"
#include "RenderSystem.h"

// 这个函数加载一个圆形裁剪的纹理，适用于游戏中敌人和角色的贴图。它会将图片中圆形区域以外的部分设置为透明，从而实现圆形效果。
bool loadCircularTexture(sf::Texture& texture, const std::string& path)
{
    sf::Image image;

    if (!image.loadFromFile(path))
    {
        return false;
    }

    sf::Vector2u size = image.getSize();

    float centerX = size.x * 0.5f;
    float centerY = size.y * 0.5f;

    float radius = size.x < size.y ? size.x * 0.48f : size.y * 0.48f;
    float radiusSq = radius * radius;

    for (unsigned int y = 0; y < size.y; ++y)
    {
        for (unsigned int x = 0; x < size.x; ++x)
        {
            float dx = static_cast<float>(x) - centerX;
            float dy = static_cast<float>(y) - centerY;

            if (dx * dx + dy * dy > radiusSq)
            {
                sf::Color color = image.getPixel(x, y);
                color.a = 0;
                image.setPixel(x, y, color);
            }
        }
    }

    if (!texture.loadFromImage(image))
    {
        return false;
    }

    texture.setSmooth(true);
    return true;
}

void drawTextureOnCircle(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::CircleShape& circle,
    float targetDiameter)
{
    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::Vector2u textureSize = texture.getSize();

    sprite.setOrigin(
        textureSize.x * 0.5f,
        textureSize.y * 0.5f
    );

    float baseSize = textureSize.x < textureSize.y ?
        static_cast<float>(textureSize.x) :
        static_cast<float>(textureSize.y);

    float scale = targetDiameter / baseSize;
    sprite.setScale(scale, scale);

    sprite.setPosition(
        circle.getPosition().x + circle.getRadius(),
        circle.getPosition().y + circle.getRadius()
    );

    window.draw(sprite);
}


int main()
{

    //====================初始数据=====================

    // 1. 窗口和游戏视角
    // window 是真实窗口，gameView 决定当前能看到世界地图的哪一部分。
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "HonorBirdGame");

    window.setFramerateLimit(60);

    sf::View gameView(sf::FloatRect(0.f, 0.f, DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT));
    gameView.setCenter(DEFAULT_VIEW_CENTER);
    window.setView(gameView);


    //2.字体和UI文本
	//这些文字属于界面层，用默认视角绘制，不随游戏视角移动
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/msyh.ttc"))
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
    titleText.setCharacterSize(58);
    titleText.setFillColor(sf::Color::Black);
    titleText.setPosition(650.f, 95.f);

    sf::Text startText;
    startText.setFont(font);
    startText.setCharacterSize(32);
    startText.setFillColor(sf::Color(180,40,40));
    startText.setPosition(635.f, 780.f);

    sf::RectangleShape menuPanel(sf::Vector2f(1040.f, 520.f));
    menuPanel.setFillColor(sf::Color(255, 255, 255, 190));
    menuPanel.setOutlineThickness(3.f);
    menuPanel.setOutlineColor(sf::Color(60, 60, 60));
    menuPanel.setPosition(280.f, 205.f);

    sf::Text controlTitleText;
    controlTitleText.setFont(font);
    controlTitleText.setCharacterSize(32);
    controlTitleText.setFillColor(sf::Color::Black);
    controlTitleText.setPosition(330.f, 235.f);
    controlTitleText.setString(L"玩法说明");

    sf::Text controlText;
    controlText.setFont(font);
    controlText.setCharacterSize(22);
    controlText.setFillColor(sf::Color::Black);
    controlText.setPosition(330.f, 290.f);
    controlText.setLineSpacing(1.18f);
    controlText.setString(
        L"基本操作：\n"
        L"鼠标拖拽：调整发射方向和力度\n"
        L"SPACE：按固定速度发射小鸟\n"
        L"A / D：发射前左右查看关卡\n"
        L"C：回到发射视角\n"
        L"N：当前小鸟落地后切换下一只\n"
        L"R：返回主菜单"
    );

    sf::Text skillIntroText;
    skillIntroText.setFont(font);
    skillIntroText.setCharacterSize(22);
    skillIntroText.setFillColor(sf::Color::Black);
    skillIntroText.setPosition(820.f, 290.f);
    skillIntroText.setLineSpacing(1.18f);
    skillIntroText.setString(
        L"英雄技能：\n"
        L"夏洛特：按 F 释放五芒星与\n"
        L"        彼岸花范围爆发\n"
        L"花木兰：第一次 F 轻剑突进\n"
        L"        第二次 F 释放剑气\n"
        L"\n"
        L"通关目标：\n"
        L"击败关卡中的全部敌人\n"
        L"合理利用建筑倒塌和英雄技能"
    );

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


    sf::Text skillLineText;
    skillLineText.setFont(font);
    skillLineText.setCharacterSize(30);
    skillLineText.setFillColor(sf::Color(255, 228, 235));
    skillLineText.setString(L"七星光芒剑！");


    //3.静态场景物体
    sf::RectangleShape ground(sf::Vector2f(WORLD_WIDTH, WORLD_HEIGHT - GROUND_TOP));
    ground.setPosition(0.f, GROUND_TOP);
    ground.setFillColor(sf::Color(80, 180, 80));

    sf::RectangleShape slingPost(sf::Vector2f(34.f, 180.f));
    slingPost.setPosition(132.f, 580.f);
    slingPost.setFillColor(sf::Color(139, 69, 19));


    //状态以及鸟，建筑，敌人,技能特效等
    RuntimeState state;
   
	//加载剑气图片并初始化剑气状态
    if (!initMulanSwordQi(state, "assets/images/effects/mulan_jianqi.jpg"))
    {
        return -1;
    }

    sf::Texture charlotteBirdTexture;
    sf::Texture mulanBirdTexture;
    sf::Texture enemyMageTexture;

    if (!loadCircularTexture(charlotteBirdTexture, "assets/images/characters/charlotte_bird.png"))
    {
        return -1;
    }

    if (!loadCircularTexture(mulanBirdTexture, "assets/images/characters/mulan_bird.png"))
    {
        return -1;
    }

    if (!loadCircularTexture(enemyMageTexture, "assets/images/characters/enemy_mage.png"))
    {
        return -1;
    }

    LevelData currentLevelData = loadLevel(1);

    sf::CircleShape bird(30.f);
    bird.setOutlineThickness(3.f);
    bird.setOutlineColor(sf::Color::Black);

    std::vector<Enemy> enemies;
    std::vector<Block> blocks;

    resetLevel(currentLevelData, bird, enemies, blocks, state);

    gameView.setCenter(currentLevelData.homeViewCenter);
    gameView.setSize(currentLevelData.homeViewSize);
    window.setView(gameView);

    sf::Clock frameClock;


    //=========================事件处理区===========================
    while (window.isOpen())
    {
        float dt = frameClock.restart().asSeconds();

        //稳定帧率；
        if (dt > 0.033)
        {
            dt = 0.033f;
        }


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
                        gameView.setCenter(currentLevelData.homeViewCenter);
                        gameView.setSize(currentLevelData.homeViewSize);
                        window.setView(gameView);
                        state.gameState = GameState::Playing;
                    }
                    else if (state.gameState == GameState::LevelClear)
                    {
                        state.currentLevel = state.nextLevel;
                        currentLevelData = loadLevel(state.currentLevel);
                        resetLevel(currentLevelData, bird, enemies, blocks, state);
                        gameView.setCenter(currentLevelData.homeViewCenter);
                        gameView.setSize(currentLevelData.homeViewSize);
                        window.setView(gameView);
                        state.gameState = GameState::Playing;
                    }
                }

                if (state.gameState == GameState::Playing)
                {
					//C：切换回发射视角；
                    if (event.key.code == sf::Keyboard::C)
                    {
                        state.preLaunchViewOffsetX = 0.f;
                        gameView.setSize(currentLevelData.homeViewSize);
                        gameView.setCenter(currentLevelData.homeViewCenter);
                        window.setView(gameView);
                    }

                    // Space：不用鼠标拖拽，直接给 bird 一个默认初速度发射，主要作为备用发射方式。
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        if (state.birdReady && !state.isDragging && state.birdsRemaining > 0)
                        {
                            state.birdsRemaining--;
                            state.birdReady = false;
                            state.birdSpeedX = 1500.f;
                            state.birdSpeedY = -1050.f;
                            state.roundOver = false;
                        }
                    }

                    // N：当前 bird 回合结束后，切换到队列中的下一只 bird。
                    if (event.key.code == sf::Keyboard::N)
                    {
                        if (state.roundOver && state.birdsRemaining > 0 && hasRemainingEnemies(enemies))
                        {
                            bird.setPosition(currentLevelData.birdStartPos);
                            state.birdReady = true;
                            state.isDragging = false;
                            state.roundOver = false;
                            state.skillUsedThisBird = false;
                            state.skillSequenceActive = false;
                            state.skillDamageApplied = false;
                            state.skillEffectTimer = 0.f;
                            state.skillEffectDuration = 0.92f;

                            state.mulanSkillStage = 0;
                            state.mulanSkillLineTimer = 0.f;
                            state.mulanFreezeTimer = 0.f;
                            state.mulanSwordQi.active = false;
                            state.mulanSwordQi.lifeTimer = 0.f;

                            state.birdSpeedX = 0.f;
                            state.birdSpeedY = 0.f;
                            state.preLaunchViewOffsetX = 0.f;

                            gameView.setCenter(currentLevelData.homeViewCenter);
                            gameView.setSize(currentLevelData.homeViewSize);
                            window.setView(gameView);

                            if (state.currentBirdIndex + 1 < static_cast<int>(currentLevelData.birdQueue.size()))
                            {
                                state.currentBirdIndex++;
                                applyBirdStats(bird, state, currentLevelData.birdQueue[state.currentBirdIndex]);
                            }
                        }
                    }

                    // F：释放当前英雄技能。
                    // 夏洛特：范围技能。
                    // 花木兰：两段式技能，第一段轻剑突进，第二段后面再接剑气。
                    if (event.key.code == sf::Keyboard::F)
                    {
                        if (!state.birdReady && !state.roundOver)
                        {
                            if (!state.skillUsedThisBird &&
                                state.currentBirdType == BirdType::Charlotte)
                            {
                                state.skillUsedThisBird = true;
                                state.skillSequenceActive = true;
                                state.skillDamageApplied = false;
                                state.skillEffectCenter = getCircleCenter(bird);
                                state.skillEffectTimer = 0.f;

                                state.skillVoiceDuration = 1.20f;
                                state.skillEffectDuration = state.skillVoiceDuration + 0.75f;

								skillLineText.setString(L"夏洛特: 七星光芒剑！");
                            }
                            else if (state.currentBirdType == BirdType::Mulan)
                            {
                                if (state.mulanSkillStage == 0)
                                {
                                    // 第一段：轻剑突进
                                    state.birdSpeedX *= 1.35f;

                                    if (state.birdSpeedY > -650.f)
                                    {
                                        state.birdSpeedY = -650.f;
                                    }
                                    else
                                    {
                                        state.birdSpeedY *= 0.85f;
                                    }

                                    state.mulanSkillStage = 1;
                                    state.mulanSkillLineTimer = 1.0f;
                                    skillLineText.setString(L"花木兰: 展示高端操作!");
                                }
                                else if (state.mulanSkillStage == 1)
                                {
                                    // 第二段：原地停住，释放剑气，然后进入竖直下落
                                    state.birdSpeedX = 0.f;
                                    state.birdSpeedY = 0.f;

                                    state.mulanFreezeTimer = 0.35f;
                                    state.mulanSkillStage = 2;
                                    state.mulanSkillLineTimer = 1.0f;

                                    spawnMulanSwordQi(bird, state);

                                    skillLineText.setString(L"花木兰: 剑指，勇往无前!");
                                }
                            }
                        }
                    }
                }
				// R：重置关卡
                if (event.key.code == sf::Keyboard::R)
                {
                    if (state.gameState == GameState::Playing ||
                        state.gameState == GameState::Win ||
                        state.gameState == GameState::Lose)
                    {
                        state.currentLevel = 1;
                        currentLevelData = loadLevel(state.currentLevel);
                        resetLevel(currentLevelData, bird, enemies, blocks, state);
                        gameView.setCenter(currentLevelData.homeViewCenter);
                        gameView.setSize(currentLevelData.homeViewSize);
                        window.setView(gameView);
                        state.gameState = GameState::StartMenu;
                    }
                }
            }

            // 鼠标按下：只有点中 bird 时，才进入拖拽状态。
            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    const sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    const sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, gameView);

                    if (bird.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        state.isDragging = true;
                    }
                }
            }

            // 鼠标移动：让 bird 跟随鼠标拖拽，同时限制拖拽方向和最大拖拽距离。
            if (state.gameState == GameState::Playing && state.birdReady && !state.roundOver &&
                state.isDragging && event.type == sf::Event::MouseMoved)
            {
                const sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
                const sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, gameView);

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

            // 鼠标松开：把拖拽距离转换成发射速度，然后发射 bird。
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


		//================================更新区==================================
        if (state.gameState == GameState::Playing)
        {
			float gameplayScale = 1.f;//用来控制游戏内所有基于时间的变化

            if (state.birdReady && !state.isDragging)
            {
                float panSpeed = 1000.f;

                ///发射前看图：bird 还没发射且没有拖拽时，按 A / D 可以左右移动视角查看关卡。
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    state.preLaunchViewOffsetX -= panSpeed * dt;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    state.preLaunchViewOffsetX += panSpeed * dt;
                }

                float minCenterX = currentLevelData.homeViewSize.x * 0.5f;
                float maxCenterX = WORLD_WIDTH - currentLevelData.homeViewSize.x * 0.5f;
                float targetCenterX = currentLevelData.homeViewCenter.x + state.preLaunchViewOffsetX;

                if (targetCenterX < minCenterX)
                {
                    targetCenterX = minCenterX;
                    state.preLaunchViewOffsetX = targetCenterX - currentLevelData.homeViewCenter.x;
                }

                if (targetCenterX > maxCenterX)
                {
                    targetCenterX = maxCenterX;
                    state.preLaunchViewOffsetX = targetCenterX - currentLevelData.homeViewCenter.x;
                }
            }

            //花木兰技能台词显示计时
            if (state.mulanSkillLineTimer > 0.f)
            {
                state.mulanSkillLineTimer -= dt;
                if (state.mulanSkillLineTimer < 0.f)
                {
                    state.mulanSkillLineTimer = 0.f;
                }

            }

			//夏洛特技能逻辑：先暂停显示台词，再结算伤害，最后慢动作过渡回正常
            if (state.skillSequenceActive)
            {
                state.skillEffectTimer += dt;

                if (!state.skillDamageApplied)
                {
                    state.skillEffectCenter = getCircleCenter(bird);
                }

                float freezeEnd = state.skillVoiceDuration;
                float damageTime = freezeEnd;
                float slowEnd = damageTime + 0.28f;

                if (state.skillEffectTimer < freezeEnd)
                {
                    gameplayScale = 0.f;
                }
                else if (state.skillEffectTimer < slowEnd)
                {
                    gameplayScale = 0.22f;
                }
                else
                {
                    gameplayScale = 1.f;
                }

                if (!state.skillDamageApplied && state.skillEffectTimer >= damageTime)
                {
                    state.skillEffectCenter = getCircleCenter(bird);
                    applyCharlotteAreaSkill(bird, enemies, blocks, state);
                    state.skillDamageApplied = true;
                }

                if (state.skillEffectTimer >= state.skillEffectDuration)
                {
                    state.skillSequenceActive = false;
                    state.skillDamageApplied = false;
                    state.skillEffectTimer = state.skillEffectDuration;
                }
            }

            // 准备状态下的 bird 微调：发射前可以用方向键轻微调整 bird 的水平位置。
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

            // 飞行更新：更新 bird 位置和重力，并检测落地、撞建筑、撞敌人。
            if (!state.birdReady)
            {
                if (state.mulanFreezeTimer > 0.f)
                {
                    state.mulanFreezeTimer -= dt;

                    if (state.mulanFreezeTimer < 0.f)
                    {
                        state.mulanFreezeTimer = 0.f;

                        state.birdSpeedX = 0.f;
                        state.birdSpeedY = 780.f;
                    }
                    else 
                    {
                        state.birdSpeedX = 0.f;
                        state.birdSpeedY = 0.f;
                    }
                }
                else
                {
                    bird.move(state.birdSpeedX * gameplayScale * dt, state.birdSpeedY * gameplayScale * dt);
                    state.birdSpeedY += state.gravity * gameplayScale * dt;

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
            }

            updateBeamFallingState(blocks);
            updateFallingBlocks(blocks, gameplayScale,dt);
            checkFallingBlocksHitEnemies(blocks, enemies);
            updateMulanSwordQi(enemies,blocks,state, dt);

            const bool hasPendingResolutionNow = hasPendingResolution(blocks);
            updateGameResult(enemies, state, hasPendingResolutionNow);
          
            // 受击闪白计时：敌人和建筑被打中后会短暂闪白，这里逐帧减少闪白时间。
            for (Enemy& enemy : enemies)
            {
                if (enemy.hitFlashTimer > 0.f)
                {
                    enemy.hitFlashTimer -= dt;
                    if (enemy.hitFlashTimer <= 0.f)
                    {
                        enemy.hitFlashTimer = 0.f;
                    }
                }
            }

            for (Block& block : blocks)
            {
                if (block.hitFlashTimer > 0.f)
                {
                    block.hitFlashTimer -= dt;
                    if (block.hitFlashTimer <= 0.f)
                    {
                        block.hitFlashTimer = 0.f;
                    }
                }
            }

            // 视角更新：发射前使用看图偏移；发射后跟随 bird；回合结束后回到发射位。
            float targetViewCenterX = currentLevelData.homeViewCenter.x + state.preLaunchViewOffsetX;

            if (!state.roundOver && !state.birdReady && bird.getPosition().x > 700.f)
            {
                targetViewCenterX = bird.getPosition().x + bird.getRadius() + 350.f;

                const float minCenterX = currentLevelData.homeViewSize.x * 0.5f;
                const float maxCenterX = WORLD_WIDTH - currentLevelData.homeViewSize.x * 0.5f;

                if (targetViewCenterX < minCenterX)
                {
                    targetViewCenterX = minCenterX;
                }
                else if (targetViewCenterX > maxCenterX)
                {
                    targetViewCenterX = maxCenterX;
                }
            }

            if (state.roundOver && state.birdsRemaining > 0)
            {
                targetViewCenterX = currentLevelData.homeViewCenter.x;
            }

            gameView.setCenter(targetViewCenterX, currentLevelData.homeViewCenter.y);
            gameView.setSize(currentLevelData.homeViewSize);
            window.setView(gameView);


			//更新标题和提示文本
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
                infoText.setString(L"按 N 切换下一只小鸟");
            }
            else if (state.isDragging)
            {
                infoText.setString(L"松开鼠标发射");
            }
            else if (state.birdReady)
            {
                std::wstring birdName = L"普通小鸟";

                if (state.currentBirdType == BirdType::Charlotte)
                {
                    birdName = L"夏洛特";
                }
                else if (state.currentBirdType == BirdType::Mulan)
                {
                    birdName = L"花木兰";
                }

                infoText.setString(
                    L"第 " + std::to_wstring(state.currentLevel) + L" 关" +
                    L"  剩余小鸟：" + std::to_wstring(state.birdsRemaining) +
                    L"  当前角色：" + birdName +
                    L"  序号：" + std::to_wstring(state.currentBirdIndex + 1)
                );
            }
            else
            {
                infoText.setString("");
            }
        }

        if (state.gameState == GameState::Win)
        {
            window.setTitle("HonorBirdGame - Win");
            resultTitleText.setString(L"胜利！");
            resultHintText.setString(L"按 R 返回主菜单");
        }
        else if (state.gameState == GameState::Lose)
        {
            window.setTitle("HonorBirdGame - Lose");
            resultTitleText.setString(L"失败！");
            resultHintText.setString(L"按 R 返回主菜单");
        }
        else if (state.gameState == GameState::StartMenu)
        {
            window.setTitle("HonorBirdGame - Start Menu");
            titleText.setString(L"荣耀小鸟");
            startText.setString(L"按 ENTER 开始游戏");
        }
        else if (state.gameState == GameState::LevelClear)
        {
            window.setTitle("HonorBirdGame - Level Clear!");
            resultTitleText.setString(
                L"第 " + std::to_wstring(state.currentLevel) + L" 关通过！"
            );

            resultHintText.setString(
                L"按 ENTER 进入第 " + std::to_wstring(state.nextLevel) + L" 关"
            );
        }


		//=============================绘制区===============================
        window.clear(sf::Color(135, 206, 235));

        if (state.gameState == GameState::StartMenu)
        {
            window.setView(window.getDefaultView());

            window.draw(titleText);
            window.draw(menuPanel);
            window.draw(controlTitleText);
            window.draw(controlText);
			window.draw(skillIntroText);
            window.draw(startText);
        }
        else if (state.gameState == GameState::Playing)
        {
            // 用 gameView 绘制世界层。
            // 地面、弹弓、bird、建筑、敌人、技能特效都属于世界坐标。
			window.setView(gameView);

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

			// 根据当前 bird 类型选择不同的绘制方式。夏洛特和花木兰的鸟使用圆形裁剪纹理绘制，普通鸟直接用圆形绘制。
            if (state.currentBirdType == BirdType::Charlotte)
            {
                drawTextureOnCircle(window, charlotteBirdTexture, bird, 78.f);
            }
            else if (state.currentBirdType == BirdType::Mulan)
            {
                drawTextureOnCircle(window, mulanBirdTexture, bird, 84.f);
            }
            else
            {
                window.draw(bird);
            }

            drawBlocks(window, blocks);
            drawEnemies(window, enemies);

            for (const Enemy& enemy : enemies)
            {
                if (enemy.alive)
                {
                    drawTextureOnCircle(window, enemyMageTexture, enemy.shape, 76.f);
                }
            }

            drawCharlotteSkillEffect(window, state);
            drawMulanSwordQi(window, state.mulanSwordQi);

            drawCharlotteSkillEffect(window, state);
            drawMulanSwordQi(window, state.mulanSwordQi);

            // 切换到默认视角绘制 UI 层。
            // UI 文字固定在屏幕上，不应该跟随地图镜头移动。
			window.setView(window.getDefaultView());

			// 技能台词：当英雄技能发动时，在屏幕左上角显示台词提示。
            if ((state.skillUsedThisBird||state.mulanSkillLineTimer >0.f) && !state.roundOver)
            {
                sf::View uiView = window.getDefaultView();
                sf::FloatRect textBounds = skillLineText.getLocalBounds();

                float leftX = uiView.getCenter().x - uiView.getSize().x * 0.5f + 48.f;
                float topY = uiView.getCenter().y - uiView.getSize().y * 0.5f + 52.f;

                skillLineText.setOrigin(textBounds.left, textBounds.top + textBounds.height * 0.5f);
                skillLineText.setPosition(leftX, topY);

                sf::RectangleShape subtitleBack;
                subtitleBack.setSize(sf::Vector2f(textBounds.width + 40.f, textBounds.height + 22.f));
                subtitleBack.setOrigin(0.f, subtitleBack.getSize().y * 0.5f);
                subtitleBack.setPosition(leftX - 16.f, topY + 2.f);
                subtitleBack.setFillColor(sf::Color(10, 10, 14, 145));

                sf::Text subtitleShadow = skillLineText;
                subtitleShadow.setFillColor(sf::Color(20, 8, 12, 200));
                subtitleShadow.move(2.f, 2.f);

                window.draw(subtitleBack);
                window.draw(subtitleShadow);
                window.draw(skillLineText);
            }

            window.draw(infoText);
            window.setView(gameView);
        }
        else if (state.gameState == GameState::Win ||
            state.gameState == GameState::Lose ||
            state.gameState == GameState::LevelClear)
        {
            window.setView(window.getDefaultView());
            window.draw(resultTitleText);


            window.draw(resultHintText);
        }

        window.display();
    }

    return 0;
}
