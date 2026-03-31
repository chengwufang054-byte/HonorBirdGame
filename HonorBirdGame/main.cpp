#include<SFML/Graphics.hpp>
#include<cmath>
#include<string>

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

	sf::RectangleShape ground(sf::Vector2f(800.f, 100.f));
	ground.setPosition(0.f, 500.f);
	ground.setFillColor(sf::Color(80, 180, 80));

	sf::RectangleShape slingPost(sf::Vector2f(12.f, 110.f));
	slingPost.setPosition(60.f, 290.f);
	slingPost.setFillColor(sf::Color(139, 69, 19));

	sf::Vector2f birdStartPos(80.f, 320.f);
	sf::Vector2f enemyStartPos(520.f, 440.f);
	sf::Vector2f enemy2StartPos(500.f, 360.f);
	sf::Vector2f blockStartPos(470.f, 420.f);
	sf::Vector2f block2StartPos(450.f, 400.f);

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

	sf::RectangleShape block(sf::Vector2f(30.f, 80.f));
	block.setPosition(blockStartPos);
	block.setFillColor(sf::Color(120, 90, 60));

	sf::RectangleShape block2(sf::Vector2f(120.f, 20.f));
	block2.setPosition(block2StartPos);
	block2.setFillColor(sf::Color(120, 90, 60));

	bool birdReady = true;
	bool isDragging = false;

	bool enemyAlive = true;
	bool enemy2Alive = true;
	bool enemy2Dropped = false;
	bool enemy2SupportedByBlock2 = true;

	bool blockAlive = true;
	int blockHp = 2;
	bool blockCollisionHandled = false;
	bool block2Alive = true;
	int block2Hp = 2;
	bool block2CollisionHandled = false;
	bool block2Falling = false;

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

				if (event.key.code == sf::Keyboard::R)
				{
					bird.setPosition(birdStartPos);
					birdReady = true;
					isDragging = false;
					birdSpeedX = 0.f;
					birdSpeedY = 0.f;
					bird.setFillColor(sf::Color::Red);
					birdsLeft = 3;

					enemyAlive = true;
					enemy2Alive = true;
					enemy2Dropped = false;
					enemy2SupportedByBlock2 = true;
					enemy.setPosition(enemyStartPos);
					enemy2.setPosition(enemy2StartPos);
					enemy.setFillColor(sf::Color(160, 80, 200));
					enemy2.setFillColor(sf::Color(255, 170, 0));

					blockAlive = true;
					blockHp = 2;
					blockCollisionHandled = false;
					block.setPosition(blockStartPos);
					block.setFillColor(sf::Color(120, 90, 60));

					block2Alive = true;
					block2Hp = 2;
					block2CollisionHandled = false;
					block2Falling = false;
					block2.setPosition(block2StartPos);
					block2.setFillColor(sf::Color(120, 90, 60));


					roundOver = false;
				}

				if (event.key.code == sf::Keyboard::N)
				{
					if (roundOver && birdsLeft > 0 && (enemyAlive || enemy2Alive))
					{
						bird.setPosition(birdStartPos);
						birdReady = true;
						isDragging = false;
						roundOver = false;
						birdSpeedX = 0.f;
						birdSpeedY = 0.f;
						bird.setFillColor(sf::Color::Red);
					}
				}

			}
			if (birdReady && !roundOver && event.type == sf::Event::MouseButtonPressed)
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

			if (birdReady && !roundOver && isDragging && event.type == sf::Event::MouseMoved)
			{
				sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
				sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

				sf::Vector2f desiredPos(mousePos.x - 20.f, mousePos.y - 20.f);

				if (desiredPos.x > birdStartPos.x)
				{
					desiredPos.x = birdStartPos.x;
				}

				if (desiredPos.y < 240.f)
				{
					desiredPos.y = 240.f;
				}
				sf::Vector2f offset = desiredPos - birdStartPos;

				float distance = std::sqrt(offset.x * offset.x + offset.y * offset.y);

				if (distance > maxDragDistance && distance > 0.f)
				{
					offset.x = offset.x / distance * maxDragDistance;
					offset.y = offset.y / distance * maxDragDistance;
					desiredPos = birdStartPos + offset;
				}

				bird.setPosition(desiredPos);
			}

			if (birdReady && !roundOver && isDragging && event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (birdsLeft > 0)
					{
						sf::Vector2f dragOffset = bird.getPosition() - birdStartPos;

						birdSpeedX = -dragOffset.x * launchPower;
						birdSpeedY = -dragOffset.y * launchPower;

						bird.setPosition(birdStartPos);

						birdsLeft--;
						birdReady = false;
						isDragging = false;
						roundOver = false;
					}
				}
			}

		}

		// ===== 更新区 =====
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
			if (blockAlive && bird.getGlobalBounds().intersects(block.getGlobalBounds()))
			{
				if (!blockCollisionHandled)
				{
					blockHp--;
					birdSpeedX *= 0.7;
					birdSpeedY *= 0.7;

					if (blockHp == 1)
					{
						block.setFillColor(sf::Color(80, 60, 40));
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

			//第二个障碍块：两段耐久
			if (block2Alive && bird.getGlobalBounds().intersects(block2.getGlobalBounds()))
			{
				if (!block2CollisionHandled)
				{
					block2Hp--;
					birdSpeedX *= 0.7f;
					birdSpeedY *= 0.7f;

					if (block2Hp == 1)
					{
						block2.setFillColor(sf::Color(80, 60, 40));
					}

					if (block2Hp <= 0)
					{
						block2Alive = false;
						enemy2SupportedByBlock2 = false;
					}

					block2CollisionHandled = true;
				}
			}
			else
			{
				block2CollisionHandled = false;
			}

			//命中第一个敌人
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

			//命中第二个敌人
			float enemy2CenterX = enemy2.getPosition().x + 20.f;
			float enemy2CenterY = enemy2.getPosition().y + 20.f;

			float dx2 = birdCenterX - enemy2CenterX;
			float dy2 = birdCenterY - enemy2CenterY;
			float distance2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

			if (enemy2Alive && distance2 <= 40.f)
			{
				enemy2Alive = false;
			}
		}

		if (!blockAlive && block2Alive)
		{
			block2Falling = true;
		}

		if (block2Falling && block2Alive)
		{
			if (block2.getPosition().y < 480.f)
			{
				block2.move(0.f, 0.25f);

				if (block2.getPosition().y > 480.f)
				{
					block2.setPosition(block2.getPosition().x, 480.f);
				}

				if (enemy2Alive&&enemy2SupportedByBlock2)
				{
					enemy2.setPosition(enemy2.getPosition().x, block2.getPosition().y - 40.f);
				}
			}
		}

		if (block2Alive && enemyAlive && block2.getGlobalBounds().intersects(enemy.getGlobalBounds()))
		{
			enemyAlive = false;
		}

		//block2 被打掉后，enemy2下落
		if (!block2Alive && enemy2Alive&&!enemy2SupportedByBlock2)
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

		// ===== 胜利提示 =====
		if (!enemyAlive && !enemy2Alive)
		{
			window.setTitle("HonorBirdGame - You Win!");
		}
		else if (roundOver && birdsLeft > 0)
		{
			window.setTitle("HonorBirdGame - Press N for next bird");
		}
		else if (roundOver && birdsLeft == 0)
		{
			window.setTitle("HonorBirdGame - Press R to Retry");
		}
		else
		{
			window.setTitle("HonorBirdGame");
		}

		if (!enemyAlive && !enemy2Alive)
		{
			infoText.setString("You Win! Press R to Restart");
		}
		else if (roundOver && birdsLeft > 0)
		{
			infoText.setString("Press N for next bird");
		}
		else if (roundOver && birdsLeft == 0)
		{
			infoText.setString("Press R to Retry");
		}
		else if (isDragging)
		{
			infoText.setString("Release mouse to Launch");
		}
		else if (birdReady)
		{
			infoText.setString("Birds Left: " + std::to_string(birdsLeft) + " Drag or press Space to launch");
		}
		else
		{
			infoText.setString("");
		}

		// ===== 绘制区 =====
		window.clear(sf::Color(135, 206, 235));
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

		if (enemyAlive)
		{
			window.draw(enemy);
		}

		if (enemy2Alive)
		{
			window.draw(enemy2);
		}

		window.draw(infoText);

		window.display();
	}

	return 0;
}