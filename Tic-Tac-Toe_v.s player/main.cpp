#include <SFML/Graphics.hpp>
#include <iostream>
#include <Imgui.h>
#include <imgui-SFML.h>

int main() {
	srand(unsigned int(time(0)));

	const int fieldWidth = 3;
	const int fieldHeight = 3;
	enum class Type {
		None, Circle, Cross
	};
	Type field[fieldWidth][fieldHeight] = {};

	std::vector<std::string> filenames = {
		"background",
		"circle",
		"cross",
		"game_title",
		"coin_H",
		"coin_T"
	};
	std::map<std::string, sf::Texture> textures = {};
	std::map<std::string, sf::Sprite>sprites = {};
	for (const std::string& s : filenames) {
		if (!textures[s].loadFromFile(s + ".png")) return EXIT_FAILURE;
		sprites[s].setTexture(textures[s]);
	}
	sf::Font font;
	if (!font.loadFromFile("SHOWG.ttf")) {
		return EXIT_FAILURE;
	}

	enum class Mode { MAINMENU, COM, PLAYER};
	
	
	
	std::map<int, std::map<int, sf::RectangleShape>> blocks;
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			blocks[x][y].setSize(sf::Vector2f(150, 150));
			blocks[x][y].setFillColor(sf::Color::Transparent);
			blocks[x][y].setPosition(x * 150.f, y * 150.f);
		}
	}

	sf::Vector2f blockSize = blocks[0][0].getSize();
	sf::Vector2u Circle_And_Cross_Size = textures["circle"].getSize();
	Mode mode = Mode::MAINMENU;
	
	sf::Vector2i windowSize(textures["background"].getSize().x, textures["background"].getSize().y);
	sf::RenderWindow w(sf::VideoMode(windowSize.x, windowSize.y), L"¤«¦r¹CÀ¸");
	ImGui::SFML::Init(w);
	for (int i = 3; i <= 5; i++) {
		sprites[filenames[i]].setPosition(
			(float)(windowSize.x - textures[filenames[i]].getSize().x) / 2,
			(float)(windowSize.x - textures[filenames[i]].getSize().x) / 2);
	}
	bool backToMainMenu = false;
	int count = 1;
	bool gameOver = false;
	sf::RectangleShape winLine(sf::Vector2f(windowSize.x * (float)sqrt(2), 1));
	winLine.setOrigin(winLine.getLocalBounds().width / 2, winLine.getLocalBounds().height / 2);
	winLine.setFillColor(sf::Color::Red);
	winLine.setOutlineThickness(10);
	winLine.setOutlineColor(sf::Color::Red);
	float rotateDegree = 0;
	enum class Result {WIN_1P, WIN_2P, DRAW};
	Result result = Result::DRAW;
	sf::Text resultText("", font, 50);
	sf::Clock deltaClock, timer;
	auto& io = ImGui::GetIO();
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("SHOWG.ttf", 40);
	ImGui::SFML::UpdateFontTexture();

	while (w.isOpen()) {
		sf::Event evt;
		if (w.pollEvent(evt)) {
			ImGui::SFML::ProcessEvent(evt);

			if (evt.type == sf::Event::Closed) {
				w.close();
			}
			if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape) {
				backToMainMenu = true;
			}

		}

		if (mode == Mode::MAINMENU) {
			ImGui::SFML::Update(w, deltaClock.restart());
			ImGui::SetNextWindowPos(ImVec2(float(windowSize.x / 3.5), float(windowSize.y / 2)));
			ImGui::Begin("MainMenu", 0,
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoDecoration);
			if (ImGui::Button("    V.S. 2p   ")) mode = Mode::PLAYER;
			ImGui::End();
			timer.restart();
		}

		


		if (mode == Mode::PLAYER) {
			if (backToMainMenu) {
				for (int x = 0; x < 3; x++) {
					for (int y = 0; y < 3; y++) {
						field[x][y] = Type::None;
					}
				}
				count = 1;
				winLine.rotate(360 - rotateDegree);
				rotateDegree = 0;
				gameOver = false;
				mode = Mode::MAINMENU;
				backToMainMenu = false;
			}




			if (!gameOver && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				for (int x = 0; x < 3; x++) {
					for (int y = 0; y < 3; y++) {
						if (blocks[x][y].getGlobalBounds().
							contains((float)evt.mouseButton.x, (float)evt.mouseButton.y) &&
							field[x][y] == Type::None) {
							if (count % 2 == 0) {
								field[x][y] = Type::Circle;
							}
							else {
								field[x][y] = Type::Cross;
							}

							if (field[x][y] == field[0][0] &&
								field[0][0] == field[1][0] && field[1][0] == field[2][0]) {
								winLine.setPosition((float)windowSize.x / 2, blockSize.y * 0.5f);
							}
							else if (field[x][y] == field[0][1] &&
								field[0][1] == field[1][1] && field[1][1] == field[2][1]) {
								winLine.setPosition((float)windowSize.x / 2, blockSize.y * 1.5f);
							}
							else if (field[x][y] == field[0][2] &&
								field[0][2] == field[1][2] && field[1][2] == field[2][2]) {
								winLine.setPosition((float)windowSize.x / 2, blockSize.y * 2.5f);
							}
							else if (field[x][y] == field[0][0] &&
								field[0][0] == field[0][1] && field[0][1] == field[0][2]) {
								rotateDegree = 90;
								winLine.rotate(rotateDegree);
								winLine.setPosition(blockSize.x * 0.5f, (float)windowSize.y / 2);
							}
							else if (field[x][y] == field[1][0] &&
								field[1][0] == field[1][1] && field[1][1] == field[1][2]) {
								rotateDegree = 90;
								winLine.rotate(rotateDegree);
								winLine.setPosition(blockSize.x * 1.5f, (float)windowSize.y / 2);
							}
							else if (field[x][y] == field[2][0] &&
								field[2][0] == field[2][1] && field[2][1] == field[2][2]) {
								rotateDegree = 90;
								winLine.rotate(rotateDegree);
								winLine.setPosition(blockSize.x * 2.5f, (float)windowSize.y / 2);
							}
							else if (field[x][y] == field[0][0] &&
								field[0][0] == field[1][1] && field[1][1] == field[2][2]) {
								rotateDegree = 45;
								winLine.rotate(rotateDegree);
								winLine.setPosition((float)windowSize.x / 2, (float)windowSize.y / 2);
							}
							else if (field[x][y] == field[2][0] &&
								field[2][0] == field[1][1] && field[1][1] == field[0][2]) {
								rotateDegree = 135;
								winLine.rotate(rotateDegree);
								winLine.setPosition((float)windowSize.x / 2, (float)windowSize.y / 2);
							}
							else {
								count++;
								continue;
							}
							gameOver = true;
						}
					}
					if (count > 9) gameOver = true;
					if (gameOver) {
						break;
					}
				}
				timer.restart();
			}
		}
		

		w.clear();

		if (mode == Mode::PLAYER) {
			w.draw(sprites["background"]);
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					if (field[x][y] == Type::Circle) {
						sprites["circle"].setPosition(
							(float)(x * 150 + (blockSize.x - Circle_And_Cross_Size.x) / 2),
							(float)(y * 150 + (blockSize.y - Circle_And_Cross_Size.y) / 2));
						w.draw(sprites["circle"]);
					}
					else if (field[x][y] == Type::Cross) {
						sprites["cross"].setPosition(
							(float)(x * 150 + (blockSize.x - Circle_And_Cross_Size.x) / 2),
							(float)(y * 150 + (blockSize.y - Circle_And_Cross_Size.y) / 2));
						w.draw(sprites["cross"]);
					}
				}
			}
			
		}
		if (gameOver) {
			
			if (count <= 9 && count % 2 == 1) {
				result = Result::WIN_1P;
			}
			else if (count <= 9 && count % 2 == 0) {
				result = Result::WIN_2P;
			}
			if (result != Result::DRAW) w.draw(winLine);
			if (result == Result::WIN_1P) {
				resultText.setString("1P WIN !");
			}
			else if (result == Result::WIN_2P) {
				resultText.setString("2P WIN !");
			}
			else {
				resultText.setString("DRAW !");
			}
			
			resultText.setPosition(
				(float)(windowSize.x - resultText.getLocalBounds().width) / 2,
				(float)(windowSize.y - resultText.getLocalBounds().height) / 2);
			
			w.draw(resultText);

		}
		if (mode == Mode::MAINMENU) {
			if (mode == Mode::MAINMENU) w.draw(sprites["game_title"]);
			ImGui::SFML::Render(w);
		}

		w.display();

		if (gameOver) {
			sf::sleep(sf::seconds(2));
			backToMainMenu = true;
		}
	}
	return EXIT_SUCCESS;
}