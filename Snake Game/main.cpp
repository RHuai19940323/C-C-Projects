#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <string>

int main() {
	srand(unsigned int(time(0)));
	using v2i = sf::Vector2i;
	using v2f = sf::Vector2f;
	v2i fieldsize(20, 15);
	sf::Texture blockTexture;
	if (!blockTexture.loadFromFile("block.png")) {
		return EXIT_FAILURE;
	}
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		return EXIT_FAILURE;
	}
	sf::Sprite block(blockTexture);
	block.setColor(sf::Color::Green);
	sf::Sprite blockF(blockTexture);
	std::vector<v2i> snake = { v2i(3, 4) };
	v2i food(rand() % (fieldsize.x - 4) + 4, rand() % (fieldsize.y - 1) + 1);
	v2i blocksize(blockTexture.getSize().x, blockTexture.getSize().y);
	v2i windowsize(fieldsize.x * blocksize.x, fieldsize.y * blocksize.y);
	sf::RenderWindow window(sf::VideoMode(windowsize.x, windowsize.y), "SnakeGame");

	int foodscore = 0;
	bool isDead = false;
	double speed = 0.3;
	sf::Text scoreText("Score: " + std::to_string(foodscore), font);
	bool restart = false;
	enum class Direction {
		Up,
		Down,
		Left,
		Right
	};
	Direction direction = Direction::Right;
	sf::Clock clock;
	bool bound = true;

	window.setMouseCursorVisible(false);

	while (window.isOpen()) {
		if (restart) {
			snake = { v2i(3, 4) };
			foodscore = 0;
			scoreText.setString("Score: " + std::to_string(foodscore));
			food.x = rand() % (fieldsize.x - 4) + 4;
			food.y = rand() % (fieldsize.y - 1) + 1;
			direction = Direction::Right;
			speed = 0.3;
			isDead = false;
			restart = false;
		}

		sf::Event evt;
		if (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				window.close();
			}
			if (evt.type == sf::Event::KeyPressed) {
				if (isDead) {
					restart = true;
				}
				switch (evt.key.code) {
				case sf::Keyboard::Key::Up:
					if (direction != Direction::Down) {
						direction = Direction::Up;
					}
					break;
				case sf::Keyboard::Key::Down:
					if (direction != Direction::Up) {
						direction = Direction::Down;
					}
					break;
				case sf::Keyboard::Key::Left:
					if (direction != Direction::Right) {
						direction = Direction::Left;
					}
					break;
				case sf::Keyboard::Key::Right:
					if (direction != Direction::Left) {
						direction = Direction::Right;
					}
					break;
				case sf::Keyboard::Key::B:
					if (bound) {
						bound = false;
					}
					else {
						bound = true;
					}
					break;
				}
			}
		}
		switch (foodscore) {
		case 5:
			speed = 0.2;
			break;
		case 10:
			speed = 0.15;
			break;
		case 15:
			speed = 0.075;
			break;
		case 20:
			speed = 0.0375;
			break;
		case 25:
			speed = 0.01875;
			break;
		default:
			speed = speed;
		}

		v2i head = snake[0];
		if (clock.getElapsedTime().asSeconds() > speed) {
			switch (direction) {
			case Direction::Up:
				head.y--;
				break;
			case Direction::Down:
				head.y++;
				break;
			case Direction::Left:
				head.x--;
				break;
			case Direction::Right:
				head.x++;
				break;
			}
			if (bound) {
				if (head.x < 0 || head.x >= fieldsize.x ||
					head.y < 0 || head.y >= fieldsize.y) {
					isDead = true;
				}
			}
			else {
				if (head.x >= fieldsize.x) {
					head.x = 0;
				}
				else if (head.x < 0) {
					head.x = fieldsize.x - 1;
				}
				else if (head.y >= fieldsize.y) {
					head.y = 0;
				}
				else if (head.y < 0) {
					head.y = fieldsize.y - 1;
				}
			}

			for (int i = 1; i < (int)snake.size(); i++) {
				if (head == snake[i]) {
					isDead = true;
					break;
				}
			}

			if (!isDead) {
				snake.insert(snake.begin(), head);

				if (head == food) {
					foodscore++;
					scoreText.setString("Score: " + std::to_string(foodscore));
					food.x = rand() % (fieldsize.x - 4) + 4;
					food.y = rand() % (fieldsize.y - 1) + 1;
				}
				else {
					snake.pop_back();
				}
			}
			clock.restart();
		}
		if (isDead) {
			window.clear(sf::Color::Red);
		}
		else {
			window.clear();
		}
		blockF.setPosition((float)food.x * blocksize.x, (float)food.y * blocksize.y);
		window.draw(blockF);
		for (const v2i& body : snake) {
			block.setPosition((float)body.x * blocksize.x, (float)body.y * blocksize.y);
			window.draw(block);
		}

		window.draw(scoreText);
		window.display();
	}
	return EXIT_SUCCESS;
}