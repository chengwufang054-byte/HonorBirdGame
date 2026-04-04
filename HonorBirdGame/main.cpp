#include<SFML/Graphics.hpp>
#include<cmath>
#include<string>


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

//构建LevelDate统一管理关卡数据
struct LevelData
{
	sf::Vector2f birdStartPos;
	sf::Vector2f enemyStartPos;
	sf::Vector2f enemy2StartPos;
	sf::Vector2f enemy3StartPos;

	sf::Vector2f blockStartPos;
	sf::Vector2f block2StartPos;
	sf::Vector2f block3StartPos;

	int birdsCount;

	bool hasEnemy3;
	bool hasBlock3;
	bool needBothSupportsForBlock2;
};

//判断还有没有敌人
bool hasRemainingEnemies(int currentLevel, bool enemyAlive,
	bool enemy2Alive, bool enemy3Alive)
{
	if (currentLevel == 1)
	{
		return enemyAlive || enemy2Alive;
	}
	else if (currentLevel==2)
	{
		return enemyAlive || enemy2Alive || enemy3Alive;
	}

	return false;
}
 
//判断是不是全清了
bool allEnemiesDefeated(int currentLevel, bool enemyAlive, bool enemy2Alive,
	bool enemy3Alive)
{ 
	if (currentLevel == 1)
	{
		return !enemyAlive && !enemy2Alive;
	}
	else if (currentLevel == 2)
	{
		return !enemyAlive && !enemy2Alive && !enemy3Alive;
	}

	return false;
};

//bird命中敌人
void checkBirdHitEnemies(
	const sf::CircleShape& bird,
	int currentLevel,
	sf::CircleShape& enemy,
	bool& enemyAlive,
	sf::CircleShape& enemy2,
	bool& enemy2Alive,
	sf::CircleShape& enemy3,
	bool& enemy3Alive)
{
	float birdCenterX = bird.getPosition().x + 20.f;
	float birdCenterY = bird.getPosition().y + 20.f;

	float enemyCenterX = enemy.getPosition().x + 20.f;
	float enemyCenterY = enemy.getPosition().y + 20.f;

	float dx = birdCenterX - enemyCenterX;
	float dy = birdCenterY - enemyCenterY;
	float distance = std::sqrt(dx * dx + dy * dy);

	if (enemyAlive && distance <= 40.f)
	{
		enemyAlive = false;
	}

	float enemy2CenterX = enemy2.getPosition().x + 20.f;
	float enemy2CenterY = enemy2.getPosition().y + 20.f;

	float dx2 = birdCenterX - enemy2CenterX;
	float dy2 = birdCenterY - enemy2CenterY;
	float distance2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

	if (enemy2Alive && distance2 <= 40.f)
	{
		enemy2Alive = false;
	}

	if (currentLevel == 2)
	{
		float enemy3CenterX = enemy3.getPosition().x + 20.f;
		float enemy3CenterY = enemy3.getPosition().y + 20.f;

		float dx3 = birdCenterX - enemy3CenterX;
		float dy3 = birdCenterY - enemy3CenterY;
		float distance3 = std::sqrt(dx3 * dx3 + dy3 * dy3);

		if (enemy3Alive && distance3 <= 40.f)
		{
			enemy3Alive = false;
		}
	}
}

//统一绘制所有敌人
void drawEnemies(
	sf::RenderWindow& window,
	int currentLevel,
	const sf::CircleShape& enemy,
	bool enemyAlive,
	const sf::CircleShape& enemy2,
	bool enemy2Alive,
	const sf::CircleShape& enemy3,
	bool enemy3Alive
)
{
	if (enemyAlive)
	{
		window.draw(enemy);
	}

	if (enemy2Alive)
	{
		window.draw(enemy2);
	}

	if (currentLevel==2&&enemy3Alive)
	{
		window.draw(enemy3);
	}
}

LevelData loadLevel(int level)
{
	LevelData data;

	if (level == 1)
	{
		data.birdStartPos = sf::Vector2f(80.f, 320.f);
		data.enemyStartPos = sf::Vector2f(520.f, 440.f);
		data.enemy2StartPos = sf::Vector2f(500.f, 360.f);
		data.enemy3StartPos = sf::Vector2f(-200.f, -200.f);

		data.blockStartPos = sf::Vector2f(470.f, 420.f);
		data.block2StartPos = sf::Vector2f(450.f, 400.f);
		data.block3StartPos = sf::Vector2f(-200.f, -200.f);

		data.birdsCount = 3;

		data.hasEnemy3 = false;
		data.hasBlock3 = false;
		data.needBothSupportsForBlock2 = false;
	}
	else if (level == 2)
	{
		data.birdStartPos = sf::Vector2f(80.f, 320.f);

		data.enemyStartPos = sf::Vector2f(600.f, 440.f);
		data.enemy2StartPos = sf::Vector2f(590.f, 320.f);
		data.enemy3StartPos = sf::Vector2f(630.f, 440.f);

		data.blockStartPos = sf::Vector2f(540.f, 420.f);
		data.block2StartPos = sf::Vector2f(520.f, 360.f);
		data.block3StartPos = sf::Vector2f(650.f, 420.f);

		data.birdsCount = 2;

		data.hasEnemy3 = true;
		data.hasBlock3 = true;
		data.needBothSupportsForBlock2 = true;
	}

	return data;
}

void resetRoundForCurrentLevel(
	sf::CircleShape& bird,
	sf::CircleShape& enemy,
	sf::CircleShape& enemy2,
	sf::CircleShape& enemy3,
	sf::RectangleShape& block,
	sf::RectangleShape& block2,
	sf::RectangleShape& block3,
	const LevelData& currentLevelData,
	bool& birdReady,
	bool& isDragging,
	bool& enemyAlive,
	bool& enemy2Alive,
	bool& enemy3Alive,
	bool& enemy2Dropped,
	bool& enemy2SupportedByBlock2,
	bool& blockAlive,
	int& blockHp,
	bool& blockCollisionHandled,
	bool& block2Alive,
	int& block2Hp,
	bool& block2CollisionHandled,
	bool& block2Falling,
	bool& block3Alive,
	int& block3Hp,
	bool& block3CollisionHandled,
	bool& roundOver,
	int& birdsLeft,
	float& birdSpeedX,
	float& birdSpeedY,
    int currentLevel)
{
	bird.setPosition(currentLevelData.birdStartPos);
	birdReady = true;
	isDragging = false;
	birdSpeedX = 0.f;
	birdSpeedY = 0.f;
	bird.setFillColor(sf::Color::Red);

	birdsLeft = currentLevelData.birdsCount;

	enemyAlive = true;
	enemy2Alive = true;
	enemy2Dropped = false;
	enemy2SupportedByBlock2 = true;
	enemy.setPosition(currentLevelData.enemyStartPos);
	enemy2.setPosition(currentLevelData.enemy2StartPos);
	enemy3.setPosition(currentLevelData.enemy3StartPos);
	enemy.setFillColor(sf::Color(160, 80, 200));
	enemy2.setFillColor(sf::Color(255, 170, 0));
	enemy3.setFillColor(sf::Color(80, 160, 255));

	enemy3Alive = currentLevelData.hasEnemy3;


	blockAlive = true;
	blockHp = 2;
	blockCollisionHandled = false;
	block.setPosition(currentLevelData.blockStartPos);
	block.setFillColor(sf::Color(120, 90, 60));

	block2Alive = true;
	block2Hp = 2;
	block2CollisionHandled = false;
	block2Falling = false;
	block2.setPosition(currentLevelData.block2StartPos);
	block2.setFillColor(sf::Color(120, 90, 60));
	
	block3Alive = currentLevelData.hasBlock3;
	block3Hp = 3;
	block3CollisionHandled = false;
	block3.setPosition(currentLevelData.block3StartPos);
	block3.setFillColor(sf::Color(120, 90, 60));

	roundOver = false;
}

//block碰撞函数
void handleBirdHitBlock(
	const sf::CircleShape& bird,
	sf::RectangleShape& blockShape,
	bool& blockAlive,
	int& blockHp,
	bool& blockCollisionHandled,
	float& birdSpeedX,
	float& birdSpeedY)
{
	if (blockAlive && bird.getGlobalBounds().intersects(blockShape.getGlobalBounds()))
	{
		if (!blockCollisionHandled)
		{
			blockHp--;
			birdSpeedX *= 0.7f;
			birdSpeedY *= 0.7f;

			if (blockHp == 2)
			{
				blockShape.setFillColor(sf::Color(100, 75, 50));
			}
			else if (blockHp == 1)
			{
				blockShape.setFillColor(sf::Color(80, 60, 40));
			}

			if (blockHp <= 0)
			{
				blockAlive = false;
			}

			blockCollisionHandled = true;
		}
	}
	else
	{
		blockCollisionHandled = false;
	}
}

int main()
{
	// ===== 初始化区 =====
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

	sf::RectangleShape ground(sf::Vector2f(800.f, 100.f));
	ground.setPosition(0.f, 500.f);
	ground.setFillColor(sf::Color(80, 180, 80));

	sf::RectangleShape slingPost(sf::Vector2f(12.f, 110.f));
	slingPost.setPosition(60.f, 290.f);
	slingPost.setFillColor(sf::Color(139, 69, 19));

	sf::Vector2f birdStartPos(80.f, 320.f);
	sf::Vector2f enemyStartPos(520.f, 440.f);
	sf::Vector2f enemy2StartPos(500.f, 360.f);
	sf::Vector2f enemy3StartPos(-200.f, -200.f);
	sf::Vector2f blockStartPos(470.f, 420.f);
	sf::Vector2f block2StartPos(450.f, 400.f);
	sf::Vector2f block3StartPos(-200.f, -200.f);

	sf::CircleShape bird(20.f);
	bird.setPosition(birdStartPos);
	bird.setFillColor(sf::Color::Red);
	bird.setOutlineThickness(3.f);
	bird.setOutlineColor(sf::Color::Black);

	sf::CircleShape enemy(20.f);
	enemy.setPosition(enemyStartPos);
	enemy.setFillColor(sf::Color(160, 80, 200));
	enemy.setOutlineThickness(3.f);
	enemy.setOutlineColor(sf::Color::Black);

	sf::CircleShape enemy2(20.f);
	enemy2.setPosition(enemy2StartPos);
	enemy2.setFillColor(sf::Color(255, 170, 0));
	enemy2.setOutlineThickness(3.f);
	enemy2.setOutlineColor(sf::Color::Black);

	sf::CircleShape enemy3(20.f);
	enemy3.setPosition(enemy3StartPos);
	enemy3.setFillColor(sf::Color(80, 160, 255));
	enemy3.setOutlineThickness(3.f);
	enemy3.setOutlineColor(sf::Color::Black);

	sf::RectangleShape block(sf::Vector2f(30.f, 80.f));
	block.setPosition(blockStartPos);
	block.setFillColor(sf::Color(120, 90, 60));

	sf::RectangleShape block2(sf::Vector2f(120.f, 20.f));
	block2.setPosition(block2StartPos);
	block2.setFillColor(sf::Color(120, 90, 60));

	sf::RectangleShape block3(sf::Vector2f(30.f, 80.f));
	block3.setPosition(block3StartPos);
	block3.setFillColor(sf::Color(120, 90, 60));

	GameState gameState = GameState::StartMenu;
	bool birdReady = true;
	bool isDragging = false;

	int currentLevel = 1;
	int nextLevel = 1;

	LevelData currentLevelData = loadLevel(1);

	bool enemyAlive = true;
	bool enemy2Alive = true;
	bool enemy3Alive = true;
	bool enemy2Dropped = false;
	bool enemy2SupportedByBlock2 = true;

	bool blockAlive = true;
	int blockHp = 2;
	bool blockCollisionHandled = false;
	bool block2Alive = true;
	int block2Hp = 2;
	bool block2CollisionHandled = false;
	bool block2Falling = false;
	bool block3Alive = true;
	int block3Hp = 3;
	bool block3CollisionHandled = false;

	bool roundOver = false;

	int birdsLeft = 3;

	float birdSpeedX = 0.f;
	float birdSpeedY = 0.f;
	float gravity = 0.0012f;
	float maxDragDistance = 120.f;
	float launchPower = 0.03f;

	// ===== 主循环 =====
	while (window.isOpen())
	{
		// ===== 事件处理区 =====
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
					if (gameState == GameState::StartMenu)
					{
						currentLevel = 1;

						currentLevelData = loadLevel(currentLevel);

						resetRoundForCurrentLevel(
							bird, enemy, enemy2, enemy3, block, block2, block3,
							currentLevelData,
							birdReady, isDragging,
							enemyAlive, enemy2Alive, enemy3Alive, enemy2Dropped, enemy2SupportedByBlock2,
							blockAlive, blockHp, blockCollisionHandled,
							block2Alive, block2Hp, block2CollisionHandled, block2Falling,
							block3Alive, block3Hp, block3CollisionHandled,
							roundOver, birdsLeft, birdSpeedX, birdSpeedY, currentLevel);

						gameState = GameState::Playing;
					}

					if (gameState == GameState::LevelClear)
					{
						currentLevel = nextLevel;

						currentLevelData = loadLevel(currentLevel);
						
						resetRoundForCurrentLevel(bird, enemy, enemy2, enemy3, block, block2, block3,
							currentLevelData,
							birdReady, isDragging,
							enemyAlive, enemy2Alive, enemy3Alive, enemy2Dropped, enemy2SupportedByBlock2,
							blockAlive, blockHp, blockCollisionHandled,
							block2Alive, block2Hp, block2CollisionHandled, block2Falling,
							block3Alive, block3Hp, block3CollisionHandled,
							roundOver, birdsLeft, birdSpeedX, birdSpeedY, currentLevel
						);

						gameState = GameState::Playing;
					}
				}

				if (gameState == GameState::Playing)
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						if (birdReady && !isDragging && birdsLeft > 0)
						{
							birdsLeft--;
							birdReady = false;
							birdSpeedX = 0.45f;
							birdSpeedY = -0.55f;
							roundOver = false;
						}
					}

					if (event.key.code == sf::Keyboard::N)
					{
						if (roundOver && birdsLeft > 0 &&
							hasRemainingEnemies(currentLevel,enemyAlive,enemy2Alive,enemy3Alive))
						{
							bird.setPosition(currentLevelData.birdStartPos);
							birdReady = true;
							isDragging = false;
							roundOver = false;
							birdSpeedX = 0.f;
							birdSpeedY = 0.f;
							bird.setFillColor(sf::Color::Red);
						}
					}
				}
				if (event.key.code == sf::Keyboard::R)
				{
					if (gameState == GameState::Playing
						|| gameState == GameState::Win
						|| gameState == GameState::Lose)
					{
						currentLevel = 1;

						currentLevelData = loadLevel(currentLevel);

						resetRoundForCurrentLevel(
							bird, enemy, enemy2, enemy3, block, block2, block3,
							currentLevelData,
							birdReady, isDragging,
							enemyAlive, enemy2Alive, enemy3Alive, enemy2Dropped, enemy2SupportedByBlock2,
							blockAlive, blockHp, blockCollisionHandled,
							block2Alive, block2Hp, block2CollisionHandled, block2Falling,
							block3Alive, block3Hp, block3CollisionHandled,
							roundOver, birdsLeft, birdSpeedX, birdSpeedY, currentLevel);

						gameState = GameState::StartMenu;
					}
				}

			}
			if (gameState == GameState::Playing && birdReady && !roundOver && event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
					sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

					if (bird.getGlobalBounds().contains(mousePos.x, mousePos.y))
					{
						isDragging = true;
					}
				}
			}

			if (gameState == GameState::Playing && birdReady && !roundOver && isDragging && event.type == sf::Event::MouseMoved)
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

				if (distance > maxDragDistance && distance > 0.f)
				{
					offset.x = offset.x / distance * maxDragDistance;
					offset.y = offset.y / distance * maxDragDistance;
					desiredPos = currentLevelData.birdStartPos + offset;
				}

				bird.setPosition(desiredPos);
			}

			if (gameState == GameState::Playing && birdReady && !roundOver && isDragging && event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (birdsLeft > 0)
					{
						sf::Vector2f dragOffset = bird.getPosition() - currentLevelData.birdStartPos;

						birdSpeedX = -dragOffset.x * launchPower;
						birdSpeedY = -dragOffset.y * launchPower;

						bird.setPosition(currentLevelData.birdStartPos);

						birdsLeft--;
						birdReady = false;
						isDragging = false;
						roundOver = false;
					}
				}
			}

		}

		// ===== 更新区 =====
		if (gameState == GameState::Playing)
		{
			if (birdReady)
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
			if (!birdReady)
			{
				bird.move(birdSpeedX, birdSpeedY);
				birdSpeedY += gravity;

				if (bird.getPosition().y + 40.f >= 500.f)
				{
					bird.setPosition(bird.getPosition().x, 460.f);
					birdSpeedX = 0.f;
					birdSpeedY = 0.f;
					bird.setFillColor(sf::Color(150, 150, 150));
					roundOver = true;

				}
				//第一个障碍快：两段耐久
				handleBirdHitBlock(
					bird, block,
					blockAlive, blockHp,
					blockCollisionHandled,
					birdSpeedX, birdSpeedY
				);

				//第二个障碍块：两段耐久
				handleBirdHitBlock(
					bird, block2,
					block2Alive, block2Hp,
					block2CollisionHandled,
					birdSpeedX, birdSpeedY
				);

				//第三个障碍快：两段耐久
				handleBirdHitBlock(
					bird, block3,
					block3Alive, block3Hp,
					block3CollisionHandled,
					birdSpeedX, birdSpeedY
				);

				//判断enemy2是否下落
				if (!block2Alive)
				{
					enemy2SupportedByBlock2 = false;
				}

				//判断小鸟是否命中敌人
				checkBirdHitEnemies(
					bird,
					currentLevel,
					enemy, enemyAlive,
					enemy2, enemy2Alive,
					enemy3, enemy3Alive
				);
			}
			//判断block2是否下落
			if (!currentLevelData.needBothSupportsForBlock2)
			{
				if (!blockAlive && block2Alive)
				{
					block2Falling = true;
				}
			}
			else 
			{
				if (!blockAlive && !block3Alive && block2Alive)
				{
					block2Falling = true;
				}
			}

			//enemy2随着block2一起下落
			if (block2Falling && block2Alive)
			{
				if (block2.getPosition().y < 480.f)
				{
					block2.move(0.f, 0.25f);

					if (block2.getPosition().y > 480.f)
					{
						block2.setPosition(block2.getPosition().x, 480.f);
					}

					if (enemy2Alive && enemy2SupportedByBlock2)
					{
						enemy2.setPosition(enemy2.getPosition().x, block2.getPosition().y - 40.f);
					}
				}
			}

			//enemy被block2砸死
			if (block2Alive && enemyAlive && block2.getGlobalBounds().intersects(enemy.getGlobalBounds()))
			{
				enemyAlive = false;
			}

			//enemy3被block2砸死
			if (currentLevel == 2 && block2Alive && enemy3Alive &&
				block2.getGlobalBounds().intersects(enemy3.getGlobalBounds()))
			{
				enemy3Alive=false; 
			}

			//block2 被打掉后，enemy2下落
			if (!block2Alive && enemy2Alive && !enemy2SupportedByBlock2)
			{
				if (enemy2.getPosition().y < 460.f)
				{
					enemy2.move(0.f, 0.3f);

					if (enemy2.getPosition().y > 460.f)
					{
						enemy2.setPosition(enemy2.getPosition().x, 460.f);
					}
				}
				else if (!enemy2Dropped)
				{
					enemy2Alive = false;
					enemy2Dropped = true;
				}
			}

			//判断场景中有没有移动的物体，从而判断游戏是否结束
			bool sceneStillMoving = false;

			if (block2Falling && block2Alive && block2.getPosition().y < 480.f)
			{
				sceneStillMoving = true;
			}

			if (!block2Alive && enemy2Alive && !enemy2SupportedByBlock2
				&& enemy2.getPosition().y < 460.f)
			{
				sceneStillMoving = true;
			}

			// 判断关卡和是否胜利
			if (allEnemiesDefeated(currentLevel, enemyAlive, enemy2Alive, enemy3Alive))
			{
				if (currentLevel == 1)
				{
					nextLevel = 2;
					gameState = GameState::LevelClear;
				}
				else if (currentLevel == 2)
				{
					gameState = GameState::Win;
				}
			}

			if (roundOver && birdsLeft == 0 && !sceneStillMoving)
			{
				gameState = GameState::Lose;
			}

			// ===== 胜利提示 =====

			if (roundOver && birdsLeft > 0)
			{
				window.setTitle("HonorBirdGame - Press N for next bird");
			}
			else
			{
				window.setTitle("HonorBirdGame");
			}



			if (roundOver && birdsLeft > 0)
			{
				infoText.setString("Press N for next bird");
			}
			else if (isDragging)
			{
				infoText.setString("Release mouse to Launch");
			}
			else if (birdReady)
			{
				infoText.setString("Level " + std::to_string(currentLevel) +
					"  Birds Left: " + std::to_string(birdsLeft) + "  Drag or press Space to launch");
			}
			else
			{
				infoText.setString("");
			}
		}
		if (gameState == GameState::Win)
		{
			window.setTitle("HonorBirdGame - Win");
			resultTitleText.setString("You Win!");
			resultHintText.setString("Press R to Return to Menu");
		}
		else if (gameState == GameState::Lose)
		{
			window.setTitle("HonorBirdGame - Lose");
			resultTitleText.setString("You Lose!");
			resultHintText.setString("Press R to Return to Menu");
		}
		else if (gameState == GameState::StartMenu)
		{
			window.setTitle("HonorBirdGame - Start Menu");
			titleText.setString("Honor Bird Game");
			startText.setString("Press ENTER to Start");
		}
		else if (gameState == GameState::LevelClear)
		{
			window.setTitle("HonorBirdGame - Level Clear!");
			resultTitleText.setString("Level" + std::to_string(currentLevel) + "Clear!");
			resultHintText.setString("Press ENTER for Level " + std::to_string(nextLevel));
		}

		// ===== 绘制区 =====
		window.clear(sf::Color(135, 206, 235));

		if (gameState == GameState::StartMenu)
		{
			window.draw(titleText);
			window.draw(startText);
		}
		else if (gameState == GameState::Playing)
		{
			window.draw(ground);
			window.draw(slingPost);

			if (isDragging)
			{
				sf::Vertex line[] =
				{
				sf::Vertex(sf::Vector2f(birdStartPos.x + 20.f, birdStartPos.y + 20.f),sf::Color::Black),
				sf::Vertex(sf::Vector2f(bird.getPosition().x + 20.f, bird.getPosition().y + 20.f),sf::Color::Black)
				};

				window.draw(line, 2, sf::Lines);
			}

			window.draw(bird);

			if (blockAlive)
			{
				window.draw(block);
			}

			if (block2Alive)
			{
				window.draw(block2);
			}

			if (block3Alive)
			{
				window.draw(block3);
			}

			drawEnemies(window,
				currentLevel,
				enemy, enemyAlive,
				enemy2, enemy2Alive,
				enemy3, enemy3Alive
			);

			window.draw(infoText);
		}
		else if (gameState == GameState::Win
			|| gameState == GameState::Lose
			|| gameState == GameState::LevelClear)
		{
			window.draw(resultTitleText);
			window.draw(resultHintText);
		}

		window.display();
	}

	return 0;
}