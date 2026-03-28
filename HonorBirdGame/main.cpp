#include<SFML/Graphics.hpp>
#include<cmath>
int main() 
{
	 // ===== 初始化区 =====
	sf::RenderWindow window(sf::VideoMode(800, 600), "HonorBirdGame");

	sf::RectangleShape ground(sf::Vector2f(800.f, 100.f));
	ground.setPosition(0.f, 500.f);
	ground.setFillColor(sf::Color(80, 180, 80));

	sf::RectangleShape slingPost(sf::Vector2f(12.f, 70.f));
	slingPost.setPosition(60.f, 290.f);
	slingPost.setFillColor(sf::Color(139, 69, 19));

	sf::CircleShape bird(20.f);
	bird.setPosition(80.f, 320.f);
	bird.setFillColor(sf::Color::Red);
	bird.setOutlineThickness(3.f);
	bird.setOutlineColor(sf::Color::Black);

	sf::CircleShape enemy(20.f);
	enemy.setPosition(520.f, 440.f);
	enemy.setFillColor(sf::Color(160, 80, 200));
	enemy.setOutlineThickness(3.f);
	enemy.setOutlineColor(sf::Color::Black);

	sf::Vector2f birdStartPos(80.f, 320.f);

	bool birdReady = true;
	bool enemyHit = false;
	bool enemyAlive = true;
	bool isDragging = false;

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
					birdReady = false;
					birdSpeedX = 0.45f;
					birdSpeedY = -0.55f;
				}

				if (event.key.code == sf::Keyboard::R)
				{
					bird.setPosition(birdStartPos);
					birdReady = true;
					isDragging = false;
					birdSpeedX = 0.f;
					birdSpeedY = 0.f;
					bird.setFillColor(sf::Color::Red);

					enemyHit = false;
					enemyAlive = true;
					enemy.setFillColor(sf::Color(160, 80, 200));

				}	

			}
			if (birdReady && event.type == sf::Event::MouseButtonPressed)
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

			if (birdReady && isDragging && event.type == sf::Event::MouseMoved)
			{
				sf::Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
				sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

				sf::Vector2f desiredPos(mousePos.x - 20.f, mousePos.y - 20.f);
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

			if (birdReady && isDragging && event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					birdReady = false;
					isDragging = false;

					birdSpeedX = -(bird.getPosition().x - birdStartPos.x) * launchPower;
					birdSpeedY = -(bird.getPosition().y - birdStartPos.y) * launchPower;

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

				}

				float birdCenterX = bird.getPosition().x + 20.f;
				float birdCenterY = bird.getPosition().y + 20.f;
				float enemyCenterX = enemy.getPosition().x + 20.f;
				float enemyCenterY = enemy.getPosition().y + 20.f;

				float dx = birdCenterX - enemyCenterX;
				float dy = birdCenterY - enemyCenterY;
				float distance = std::sqrt(dx * dx + dy * dy);

				if (!enemyHit && distance <= 40.f)
				{
					enemyHit = true;
					enemyAlive = false;
				}

			}

			if (!enemyAlive)
			{
				window.setTitle("HonorBirdGame - You Win!");
			}
			else
			{
				window.setTitle("HonorBirdGame");
			}

		// ===== 绘制区 =====
		window.clear(sf::Color(135,206,235));
		window.draw(ground);
		window.draw(slingPost);
		window.draw(bird);

		if (enemyAlive) 
		{
			window.draw(enemy);
		}

		window.display();
	}	

	return 0;
}