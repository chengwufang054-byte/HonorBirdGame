#include<SFML/Graphics.hpp>

int main() 
{
	 // ===== 初始化区 =====
	sf::RenderWindow window(sf::VideoMode(800, 600), "HonorBirdGame");

	sf::RectangleShape ground(sf::Vector2f(800.f, 100.f));
	ground.setPosition(0.f, 500.f);
	ground.setFillColor(sf::Color(80, 180, 80));

	sf::RectangleShape slingPost(sf::Vector2f(12.f, 70.f));
	slingPost.setPosition(60.f, 410.f);
	slingPost.setFillColor(sf::Color(139, 69, 19));

	sf::CircleShape bird(20.f);
	bird.setPosition(80.f, 440.f);
	bird.setFillColor(sf::Color::Red);
	bird.setOutlineThickness(3.f);
	bird.setOutlineColor(sf::Color::Black);

	bool birdReady = true;

	// ===== 主循环 =====
	while (window.isOpen())
	{
		// ===== 事件处理区 =====
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event:: Closed) 
			{
				window.close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					birdReady = false;
				}

				if (event.key.code == sf::Keyboard::R)
				{
					bird.setPosition(80.f, 440.f);
					birdReady = true;
				}

			}
		}

		// ===== 更新区 =====
		if (birdReady)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				bird.move(-0.2, 0.f);
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

		// ===== 绘制区 =====
		window.clear(sf::Color(135,206,235));
		window.draw(ground);
		window.draw(slingPost);
		window.draw(bird);
		window.display();
	}	

	return 0;
}