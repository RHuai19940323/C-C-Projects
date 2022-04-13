#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <Imgui.h>
#include <imgui-SFML.h>

enum class Type {
	None, O, I, L, J, S, Z, T, Obstacle
};

enum class Action {
	Donothing, MoveDown, MoveLeft, MoveRight, Rotate, MoveDownPlus, DashDown
};
void getNewSpeed(const int& stages, float& speed) {

	switch (stages) {
	case 5:
		speed = 0.25f;
		break;
	case 10:
		speed = 0.2f;
		break;
	case 15:
		speed = 0.15f;
		break;
	case 20:
		speed = 0.1f;
		break;
	case 25:
		speed = 0.05f;
		break;
	case 30:
		speed = 0.025f;
		break;
	case 35:
		speed = 0.0125f;
		break;
	default:
		speed = speed;
	}
}

void resetPosAndShape(sf::Vector2i& pos, sf::Vector2i& origin, Type& currentType, int& currentIndex) {
	pos = origin;
	currentType = Type(rand() % 7 + 1);
	currentIndex = 0;
}

void swapHold(Type& currentType, Type& holdType) {
	Type t_currentType = currentType;
	currentType = holdType;
	holdType = t_currentType;
}

void setNewPos(Action& action, sf::Vector2i& nextPos) {
	switch (action) {
	case Action::Donothing:
		break;
	case Action::MoveDown:
	case Action::MoveDownPlus:
		nextPos.y++;
		break;
	case Action::MoveLeft:
		nextPos.x--;
		break;
	case Action::MoveRight:
		nextPos.x++;
		break;
	}
}

void scoreAdd(int& countFull, int& scores) {
	switch (countFull) {
	case 1:
		scores += 100;
		break;
	case 2:
		scores += 250;
		break;
	case 3:
		scores += 400;
		break;
	case 4:
		scores += 600;
		break;
	}
}

sf::Vector2f getPixelPos(sf::Vector2i& blockSize, sf::Vector2i& pos) {
	return sf::Vector2f((float)blockSize.x * pos.x, (float)blockSize.y * pos.y);
}

int main() {
	srand(unsigned int(time(0)));
	const int fieldWidth = 10;
	const int fieldHeight = 20;
	const int field2p_Lbound = 25;
	const int field2p_Rbound = fieldWidth + field2p_Lbound;
	sf::Texture mainMenuTitleTexture;
	if (!mainMenuTitleTexture.loadFromFile("Textures/MainMenuTitle.png")) {
		return EXIT_FAILURE;
	}
	sf::Texture helpCommandTexture;
	if (!helpCommandTexture.loadFromFile("Textures/help_command.png")) {
		return EXIT_FAILURE;
	}
	std::vector<std::string> mainMenuButtonFilenames = {
		"Textures/buttons/single.png",
		"Textures/buttons/vs2p.png",
		"Textures/buttons/vscom.png",
		"Textures/buttons/help.png"
	};
	sf::Texture mainMenuButtonTextures[4];
	sf::Sprite mainMenuButtonSprites[4];
	
	for (int i = 0; i < 4; i++) {
		if (!mainMenuButtonTextures[i].loadFromFile(mainMenuButtonFilenames[i])) {
			return EXIT_FAILURE;
		}
		else {
			mainMenuButtonSprites[i].setTexture(mainMenuButtonTextures[i]);
		}
	}

	sf::Texture mainBackgroundTexture;
	if (!mainBackgroundTexture.loadFromFile("Textures/tetrisbackground(cosmos).png")) {
		return EXIT_FAILURE;
	}

	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("Textures/tetrisbackground(cosmos)-frame.png")) {
		return EXIT_FAILURE;
	}
	sf::Texture block_gridTexture;
	block_gridTexture.setRepeated(true);
	if (!block_gridTexture.loadFromFile("Textures/block-grid.png")) {
		return EXIT_FAILURE;
	}
	sf::Font font;
	if (!font.loadFromFile("SHOWG.ttf")) {
		return EXIT_FAILURE;
	}

	sf::Vector2i blockSize(block_gridTexture.getSize().x, block_gridTexture.getSize().y);

	sf::Sprite mainBackgroundSprite(mainBackgroundTexture);
	sf::Sprite helpCommandSprite(helpCommandTexture);
	helpCommandSprite.setPosition(
		(float)(backgroundTexture.getSize().x - helpCommandTexture.getSize().x) / 2,
		(float)(backgroundTexture.getSize().y - helpCommandTexture.getSize().y) / 2);
	sf::Sprite block_gridSprite_1p(block_gridTexture,
		sf::IntRect(0, 0, block_gridTexture.getSize().x * fieldWidth, block_gridTexture.getSize().y * fieldHeight));
	int window2p_x = 1000;
	sf::Sprite block_gridSprite_2p(block_gridTexture,
		sf::IntRect(0, 0, block_gridTexture.getSize().x * fieldWidth, block_gridTexture.getSize().y * fieldHeight));
	sf::Sprite backgroundSprite_1p(backgroundTexture);
	sf::Sprite backgroundSprite_2p(backgroundTexture);
	block_gridSprite_2p.setPosition((float)window2p_x, 0);
	backgroundSprite_2p.setPosition((float)window2p_x, 0);
	sf::Sprite mainMenuTitleSprite(mainMenuTitleTexture);
	mainMenuTitleSprite.setPosition(100, 100);

	Type field[field2p_Rbound][fieldHeight] = {};
	for (int x = 10; x < field2p_Lbound; x++) {
		for (int y = 0; y < fieldHeight; y++) {
			field[x][y] = Type::None;
		}
	}
	std::vector<std::string> blockFilenames = {
		"Textures/blocks/yellow.png",
		"Textures/blocks/red.png",
		"Textures/blocks/purple.png",
		"Textures/blocks/pink.png",
		"Textures/blocks/green.png",
		"Textures/blocks/brown.png",
		"Textures/blocks/blue.png",
		"Textures/blocks/gray.png"

	};
	sf::Texture blockTextures[8];
	std::map<Type, sf::Sprite> blockSprites_1p = {};
	for (int i = 0; i < 8; i++) {
		if (!blockTextures[i].loadFromFile(blockFilenames[i])) {
			return EXIT_FAILURE;
		}
		else {
			sf::Sprite sprite(blockTextures[i]);
			blockSprites_1p[Type(i + 1)] = sprite;
		}
	}

	std::map<Type, sf::Sprite> blockSprites_2p = {};
	for (int i = 0; i < 8; i++) {
		sf::Sprite sprite(blockTextures[i]);
		blockSprites_2p[Type(i + 1)] = sprite;
	}

	std::map<Type, std::vector<std::vector<sf::Vector2i>>> shapes = {
		{ Type::O,
			{
				{
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(0, 1),
					sf::Vector2i(1, 1)
				}
			}
		},
		{ Type::I,
			{
				{
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(2, 0)
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1),
					sf::Vector2i(0, 2)
				}
			}
		},
		{ Type::L,
			{
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1),
					sf::Vector2i(1, 1)
				},
				{
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(-1, 1)
				},
				{
					sf::Vector2i(-1, -1),
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1)
				},
				{
					sf::Vector2i(1, -1),
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0)
				}
			}
		},
		{ Type::J,
			{
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(-1, 1),
					sf::Vector2i(0, 1)
				},
				{
					sf::Vector2i(-1, -1),
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0)
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(1, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1)
				},
				{
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(1, 1)
				}
			}
		},
		{ Type::S,
			{
				{
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(0, 1),
					sf::Vector2i(-1, 1)
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(1, 1)
				}
			}
		},
		{ Type::Z,
			{
				{
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1),
					sf::Vector2i(1, 1)
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(-1, 1)
				}
			}
		},
		{ Type::T,
			{
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(0, 1),
				},
				{
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(1, 0),
					sf::Vector2i(0, 1),
				},
				{
					sf::Vector2i(0, -1),
					sf::Vector2i(-1, 0),
					sf::Vector2i(0, 0),
					sf::Vector2i(0, 1),
				}
			}

		}
	};

	enum class Mode {
		MainMenu, Single, VS2p, Help, Pause
	};
	Mode mode = Mode::MainMenu;
	
	sf::Vector2i windowSize_Main(backgroundTexture.getSize().x, backgroundTexture.getSize().y);
	sf::Vector2u windowSize_2p(backgroundTexture.getSize().x * 2 + 200, backgroundTexture.getSize().y);
	sf::RenderWindow window_Main(sf::VideoMode(windowSize_Main.x, windowSize_Main.y), "Tetris++");
	sf::RenderWindow window_1p(sf::VideoMode(windowSize_Main.x, windowSize_Main.y), "Tetris++");
	sf::RenderWindow window_2p(sf::VideoMode(windowSize_2p.x, windowSize_2p.y), "Tetris++");
	window_1p.setVisible(false);
	window_2p.setVisible(false);
	const sf::Uint8 tetris = 255;
	window_Main.setIcon(1, 1, &tetris);
	window_1p.setIcon(1, 1, &tetris);
	window_2p.setIcon(1, 1, &tetris);
	ImGui::SFML::Init(window_Main);
	sf::Clock timer;
	sf::Text holdTitle_1p("Hold", font, 40);
	sf::Text holdTitle_2p("Hold", font, 40);
	sf::Text timerTitle_1p("Timer", font, 40);
	sf::Text timerTitle_2p("Timer", font, 40);
	int minutes = 0;
	int seconds = 0;
	sf::Text timerText_1p(std::to_string(minutes) + ":" + std::to_string(seconds), font, 40);
	sf::Text timerText_2p(std::to_string(minutes) + ":" + std::to_string(seconds), font, 40);
	int stages_1p = 1;
	sf::Text stageText_1p("Stage" + std::to_string(stages_1p), font, 40);
	int stages_2p = 1;
	sf::Text stageText_2p("Stage" + std::to_string(stages_2p), font, 40);
	sf::Text linesTitle_1p("Lines", font, 40);
	int lines_1p = 0;
	sf::Text linesText_1p(std::to_string(lines_1p), font, 40);
	sf::Text linesTitle_2p("Lines", font, 40);
	int lines_2p = 0;
	sf::Text linesText_2p(std::to_string(lines_1p), font, 40);
	sf::Text scoresTitle_1p("Scores", font, 40);
	int scores_1p = 0;
	sf::Text scoresText_1p(std::to_string(scores_1p), font, 40);
	sf::Text scoresTitle_2p("Scores", font, 40);
	int scores_2p = 0;
	sf::Text scoresText_2p(std::to_string(scores_1p), font, 40);
	std::vector<sf::Text*> stateTexts_1p = {
		&holdTitle_1p,
		&timerTitle_1p,
		&timerText_1p,
		&stageText_1p,
		&linesTitle_1p,
		&linesText_1p,
		&scoresTitle_1p,
		&scoresText_1p,

	};

	std::vector<sf::Text*> stateTexts_2p = {
		&holdTitle_2p,
		&timerTitle_2p,
		&timerText_2p,
		&stageText_2p,
		&linesTitle_2p,
		&linesText_2p,
		&scoresTitle_2p,
		&scoresText_2p
	};
	for (sf::Text* t : stateTexts_1p) {
		(*t).setFillColor(sf::Color(255, 174, 201));
	}
	for (sf::Text* t : stateTexts_2p) {
		(*t).setFillColor(sf::Color(255, 174, 201));
	}
	int statePosOffset = 15;
	holdTitle_1p.setPosition((float)blockSize.x * statePosOffset, 0);
	holdTitle_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, 0);
	timerTitle_1p.setPosition((float)blockSize.x * statePosOffset, (float)blockSize.y * 7);
	timerTitle_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, (float)blockSize.y * 7);
	timerText_1p.setPosition((float)blockSize.x * statePosOffset, (float)blockSize.y * 8);
	timerText_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, (float)blockSize.y * 8);
	stageText_1p.setPosition((float)blockSize.x * statePosOffset, (float)blockSize.y * 11);
	stageText_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, (float)blockSize.y * 11);
	linesTitle_1p.setPosition((float)blockSize.x * statePosOffset, (float)blockSize.y * 13);
	linesText_1p.setPosition((float)blockSize.x * (statePosOffset + 1), (float)blockSize.y * 14);
	linesTitle_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, (float)blockSize.y * 13);
	linesText_2p.setPosition((float)window2p_x + blockSize.x * (statePosOffset + 1), (float)blockSize.y * 14);
	scoresTitle_1p.setPosition((float)blockSize.x * statePosOffset, (float)blockSize.y * 16);
	scoresText_1p.setPosition((float)blockSize.x * (statePosOffset + 1), (float)blockSize.y * 17);
	scoresTitle_2p.setPosition((float)window2p_x + blockSize.x * statePosOffset, (float)blockSize.y * 16);
	scoresText_2p.setPosition((float)window2p_x + blockSize.x * (statePosOffset + 1), (float)blockSize.y * 17);

	sf::Vector2i origin_1p(fieldWidth / 2, -2);
	sf::Vector2i pos_1p(origin_1p);
	Type currentType_1p = Type(rand() % 7 + 1);
	int currentIndex_1p = 0;
	std::vector<sf::Vector2i> currentShape_1p;

	sf::Vector2i origin_2p(field2p_Rbound - fieldWidth / 2, -2);
	sf::Vector2i pos_2p(origin_2p);
	Type currentType_2p = Type(rand() % 7 + 1);
	int currentIndex_2p = 0;
	std::vector<sf::Vector2i> currentShape_2p;
	Action action_1p = Action::Donothing;
	Action action_2p = Action::Donothing;
	sf::Clock clock_1p, clock_2p;
	float speed_1p = 0.3f;
	float speed_2p = 0.3f;
	bool hold_1p = false;
	Type holdType_1p = Type::None;
	std::vector<sf::Vector2i> holdShape_1p;
	bool hold_2p = false;
	Type holdType_2p = Type::None;
	std::vector<sf::Vector2i> holdShape_2p;
	bool gameOver_1p = false;
	bool gameOver_2p = false;
	sf::Text LoseText("", font, 80);
	LoseText.setFillColor(sf::Color::White);
	sf::Text WinText("", font, 80);
	bool restartGame = false;
	sf::Clock deltaClock;
	sf::Text countdownText("3", font, 80);
	countdownText.setPosition(
		(windowSize_Main.x - countdownText.getLocalBounds().width) / 2,
		(windowSize_Main.y - countdownText.getLocalBounds().height) / 2);

	while (window_Main.isOpen()) {

		sf::Event evt_Main;
		if (window_Main.pollEvent(evt_Main)) {
			ImGui::SFML::ProcessEvent(evt_Main);
			switch (evt_Main.type) {
			case sf::Event::Closed:
				window_Main.close();
				break;
			case sf::Event::KeyPressed:
				if (mode == Mode::Help) {
					mode = Mode::MainMenu;
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (mode == Mode::Help) {
					mode = Mode::MainMenu;
				}
				break;
			}

		}
		ImGui::SFML::Update(window_Main, deltaClock.restart());
		ImGui::SetNextWindowPos(ImVec2(windowSize_Main.x / 2.6f, windowSize_Main.y / 3.f));
		sf::Color bgColor = sf::Color::Black;
		sf::Color tintColor = sf::Color::White;
		ImGui::Begin("MainMenu", 0,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground);
		if (ImGui::ImageButton(mainMenuButtonSprites[0], 1, bgColor, tintColor)) {
			mode = Mode::Single;
		}
		else if (ImGui::ImageButton(mainMenuButtonSprites[1], 1, bgColor, tintColor)) {
			mode = Mode::VS2p;
		}
		else if (ImGui::ImageButton(mainMenuButtonSprites[3], 1, bgColor, tintColor)) {
			mode = Mode::Help;
		}
		ImGui::End();

		window_Main.clear();
		window_Main.draw(mainBackgroundSprite);
		window_Main.draw(mainMenuTitleSprite);
		ImGui::SFML::Render(window_Main);
		if (mode == Mode::Help) {
			window_Main.draw(helpCommandSprite);
		}
		window_Main.display();

		if (mode == Mode::Single) {
			for (int i = 2; i >= 0; i--) {
				window_Main.clear();
				window_Main.draw(countdownText);
				window_Main.display();
				sf::sleep(sf::seconds(1));
				countdownText.setString(std::to_string(i));
			}
			countdownText.setString(std::to_string(3));
			window_1p.setVisible(true);
			window_Main.setVisible(false);
			timer.restart();

			while (window_1p.isOpen()) {
				if (restartGame) {
					for (int x = 0; x < fieldWidth; x++) {
						for (int y = 0; y < fieldHeight; y++) {
							field[x][y] = Type::None;
						}
					}
					for (int i = 1; i <= 7; i++) {
						blockSprites_1p[Type(i)].setColor(sf::Color::White);
					}
					timer.restart();
					minutes = 0;
					seconds = 0;
					stages_1p = 1;
					stageText_1p.setString("Stage" + std::to_string(stages_1p));
					lines_1p = 0;
					linesText_1p.setString(std::to_string(lines_1p));
					scores_1p = 0;
					scoresText_1p.setString(std::to_string(scores_1p));
					pos_1p = origin_1p;
					currentType_1p = Type(rand() % 7 + 1);
					currentIndex_1p = 0;
					clock_1p.restart();
					speed_1p = 0.3f;
					hold_1p = false;
					holdType_1p = Type::None;
					gameOver_1p = false;
					restartGame = false;
				}
				currentShape_1p = shapes[currentType_1p][currentIndex_1p];
				action_1p = Action::Donothing;
				sf::Event evt_1p;
				if (window_1p.pollEvent(evt_1p)) {
					if (evt_1p.type == sf::Event::Closed || mode == Mode::MainMenu) {
						window_1p.setVisible(false);
						mode = Mode::MainMenu;
						window_Main.setVisible(true);
						restartGame = true;
						break;
					}
					else if (evt_1p.type == sf::Event::KeyPressed) {
						if (!gameOver_1p) {
							switch (evt_1p.key.code) {
							case sf::Keyboard::Key::Left:
								action_1p = Action::MoveLeft;
								break;
							case sf::Keyboard::Key::Right:
								action_1p = Action::MoveRight;
								break;
							case sf::Keyboard::Key::Up:
								action_1p = Action::Rotate;
								break;
							case sf::Keyboard::Key::Down:
								action_1p = Action::MoveDownPlus;
								break;
							case sf::Keyboard::Key::Z:
								action_1p = Action::DashDown;
								break;
							case sf::Keyboard::Key::X:
								hold_1p = true;
								break;
							case sf::Keyboard::Key::Escape:
								if (mode != Mode::Pause) {
									mode = Mode::Pause;
								}
								else {
									mode = Mode::Single;
								}
								break;
							case sf::Keyboard::Key::Space:
								if (mode == Mode::Pause) {
									restartGame = true;
									mode = Mode::Single;
								}
								break;
							case sf::Keyboard::Key::Enter:
								if (mode == Mode::Pause) {
									mode = Mode::MainMenu;
								}
								break;
							}
						}
					}
				}

				if (mode == Mode::Single) {
					if (clock_1p.getElapsedTime().asSeconds() >= speed_1p) {
						action_1p = Action::MoveDown;
						clock_1p.restart();
					}
					if (hold_1p) {
						if (holdType_1p == Type::None) {
							holdType_1p = currentType_1p;
							holdShape_1p = shapes[holdType_1p][0];
							resetPosAndShape(pos_1p, origin_1p, currentType_1p, currentIndex_1p);
						}
						else {
							swapHold(currentType_1p, holdType_1p);
							holdShape_1p = shapes[holdType_1p][0];
							pos_1p = origin_1p;
							currentIndex_1p = 0;
						}
						hold_1p = false;
					}
					sf::Vector2i nextPos_1p = pos_1p;
					int nextIndex_1p = currentIndex_1p;
					if (action_1p == Action::Rotate) {
						nextIndex_1p = (nextIndex_1p + 1) % shapes[currentType_1p].size();
					}
					std::vector<sf::Vector2i> nextShape_1p = shapes[currentType_1p][nextIndex_1p];
					if (!gameOver_1p) {
						setNewPos(action_1p, nextPos_1p);
					}

					if (action_1p == Action::DashDown) {
						while (nextPos_1p.y < fieldHeight) {
							int countEmpty = 0;
							for (const sf::Vector2i& d : nextShape_1p) {
								sf::Vector2i np = nextPos_1p + d;
								if (np.x >= 0 && np.x < fieldWidth &&
									np.y < fieldHeight &&
									(np.y < 0 || field[np.x][np.y] == Type::None)) {
									countEmpty++;
								}
							}
							if (countEmpty == 4) {
								pos_1p = nextPos_1p;
								nextPos_1p.y++;
							}
							else {
								break;
							}
						}
					}
					int countEmpty_1p = 0;
					for (const sf::Vector2i& d : nextShape_1p) {
						sf::Vector2i np = nextPos_1p + d;
						if (np.x >= 0 && np.x < fieldWidth &&
							np.y < fieldHeight &&
							(np.y < 0 || field[np.x][np.y] == Type::None)) {
							countEmpty_1p++;
						}
					}
					if (countEmpty_1p == 4) {
						pos_1p = nextPos_1p;
						currentIndex_1p = nextIndex_1p;
						currentShape_1p = nextShape_1p;
					}
					else {
						if (action_1p == Action::MoveDown ||
							action_1p == Action::MoveDownPlus ||
							action_1p == Action::DashDown) {

							for (const sf::Vector2i& d : currentShape_1p) {
								sf::Vector2i np = pos_1p + d;
								if (np.y >= 0) {
									field[np.x][np.y] = currentType_1p;
								}
							}
							int countFull = 0;
							for (int y = 0; y < fieldHeight; y++) {
								bool isFull = true;
								for (int x = 0; x < fieldWidth; x++) {
									if (field[x][y] == Type::None) {
										isFull = false;
										break;
									}
								}
								if (isFull) {
									lines_1p++;
									countFull++;
									for (int ty = y; ty >= 0; ty--) {
										for (int tx = 0; tx < fieldWidth; tx++) {
											if (ty > 0) {
												field[tx][ty] = field[tx][ty - 1];
											}
											else {
												field[tx][ty] = Type::None;
											}
										}
									}
									linesText_1p.setString(std::to_string(lines_1p));
									if (lines_1p > 0 && lines_1p % 5 == 0) {
										stages_1p++;
										stageText_1p.setString("Stage" + std::to_string(stages_1p));
										getNewSpeed(stages_1p, speed_1p);

									}
								}
							}

							if (countFull != 0) {
								scoreAdd(countFull, scores_1p);
								scoresText_1p.setString(std::to_string(scores_1p));
							}

							for (int x = 0; x < fieldWidth; x++) {
								if (field[x][0] != Type::None) {
									for (const sf::Vector2i& d : currentShape_1p) {
										sf::Vector2i np = pos_1p + d;
										if (np.y < 0) {
											gameOver_1p = true;
											break;
										}
									}
								}
							}

							if (!gameOver_1p) {
								resetPosAndShape(pos_1p, origin_1p, currentType_1p, currentIndex_1p);
							}
						}
					}

					window_1p.clear();
					window_1p.draw(backgroundSprite_1p);
					window_1p.draw(block_gridSprite_1p);
					for (int x = 0; x < fieldWidth; x++) {
						for (int y = 0; y < fieldHeight; y++) {
							if (field[x][y] != Type::None) {
								sf::Vector2i pos(x, y);
								blockSprites_1p[field[x][y]].setPosition(getPixelPos(blockSize, pos));
								window_1p.draw(blockSprites_1p[field[x][y]]);
							}
						}
					}

					if (!gameOver_1p) {
						for (const sf::Vector2i& d : currentShape_1p) {
							sf::Vector2i np = pos_1p + d;
							blockSprites_1p[currentType_1p].setPosition(getPixelPos(blockSize, np));
							window_1p.draw(blockSprites_1p[currentType_1p]);
						}

						if (holdType_1p != Type::None) {
							for (const sf::Vector2i& d : holdShape_1p) {
								sf::Vector2i np = sf::Vector2i(fieldWidth + 6, 3) + d;
								blockSprites_1p[holdType_1p].setPosition(getPixelPos(blockSize, np));
								window_1p.draw(blockSprites_1p[holdType_1p]);
							}
						}
					}
					else {
						for (int i = 1; i <= 7; i++) {
							blockSprites_1p[Type(i)].setColor(sf::Color(70, 70, 70));
						}
						LoseText.setString("Game\nOver");
						LoseText.setPosition(
							(backgroundTexture.getSize().x / 2.f - LoseText.getLocalBounds().width) / 2,
							(backgroundTexture.getSize().y - LoseText.getLocalBounds().height) / 2);
						window_1p.draw(LoseText);
					}

					if (!gameOver_1p) {
						if (mode == Mode::Pause) {
							timer.restart();
						}
						if ((int)timer.getElapsedTime().asSeconds() >= 1) {
							seconds++;
							if (seconds == 60) {
								minutes++;
								seconds = 0;
							}
							timer.restart();
						}
					}
					if (seconds < 10) {
						timerText_1p.setString(std::to_string(minutes) + ":0" + std::to_string(seconds));
					}
					else {
						timerText_1p.setString(std::to_string(minutes) + ":" + std::to_string(seconds));
					}
					for (const sf::Text* t : stateTexts_1p) {
						window_1p.draw((*t));
					}
				}
				
				window_1p.display();
				if (gameOver_1p) {
					sf::sleep(sf::seconds(3));
					mode = Mode::MainMenu;
					window_1p.setVisible(false);
					window_Main.setVisible(true);
				}
			}
		}
		else if (mode == Mode::VS2p) {
			for (int i = 2; i >= 0; i--) {
				window_Main.clear();
				window_Main.draw(countdownText);
				window_Main.display();
				sf::sleep(sf::seconds(1));
				countdownText.setString(std::to_string(i));
			}
			countdownText.setString(std::to_string(3));
			window_2p.setVisible(true);
			window_Main.setVisible(false);
			timer.restart();
			clock_1p.restart();
			clock_2p.restart();

			while (window_2p.isOpen()) {
				if (restartGame) {
					for (int x = 0; x < field2p_Rbound; x++) {
						for (int y = 0; y < fieldHeight; y++) {
							field[x][y] = Type::None;
						}
					}
					for (int i = 1; i <= 7; i++) {
						blockSprites_1p[Type(i)].setColor(sf::Color::White);
						blockSprites_2p[Type(i)].setColor(sf::Color::White);
					}
					timer.restart();
					minutes = 0;
					seconds = 0;
					stages_1p = 1;
					stages_2p = 1;
					stageText_1p.setString("Stage" + std::to_string(stages_1p));
					stageText_2p.setString("Stage" + std::to_string(stages_2p));
					lines_1p = 0;
					lines_2p = 0;
					linesText_1p.setString(std::to_string(lines_1p));
					linesText_2p.setString(std::to_string(lines_1p));
					scores_1p = 0;
					scores_2p = 0;
					scoresText_1p.setString(std::to_string(scores_1p));
					scoresText_2p.setString(std::to_string(scores_1p));
					pos_1p = origin_1p;
					pos_2p = origin_2p;
					currentType_1p = Type(rand() % 7 + 1);
					currentType_2p = Type(rand() % 7 + 1);
					currentIndex_1p = 0;
					currentIndex_2p = 0;
					clock_1p.restart();
					clock_2p.restart();
					speed_1p = 0.3f;
					speed_2p = 0.3f;
					hold_1p = false;
					holdType_1p = Type::None;
					hold_2p = false;
					holdType_2p = Type::None;
					gameOver_1p = false;
					gameOver_2p = false;
					restartGame = false;
				}
				currentShape_1p = shapes[currentType_1p][currentIndex_1p];
				action_1p = Action::Donothing;
				currentShape_2p = shapes[currentType_2p][currentIndex_2p];
				action_2p = Action::Donothing;
				sf::Event evt_2p;
				if (window_2p.pollEvent(evt_2p)) {
					if (evt_2p.type == sf::Event::Closed || mode == Mode::MainMenu) {
						window_2p.setVisible(false);
						mode = Mode::MainMenu;
						window_Main.setVisible(true);
						restartGame = true;
						break;
					}
					else if (evt_2p.type == sf::Event::KeyPressed) {
						if (!gameOver_1p && !gameOver_2p) {
							switch (evt_2p.key.code) {
								// 1p control
							case sf::Keyboard::Key::A:
								action_1p = Action::MoveLeft;
								break;
							case sf::Keyboard::Key::D:
								action_1p = Action::MoveRight;
								break;
							case sf::Keyboard::Key::W:
								action_1p = Action::Rotate;
								break;
							case sf::Keyboard::Key::S:
								action_1p = Action::MoveDownPlus;
								break;
							case sf::Keyboard::Key::F:
								action_1p = Action::DashDown;
								break;
							case sf::Keyboard::Key::G:
								hold_1p = true;
								break;

								// 2p control
							case sf::Keyboard::Key::Left:
								action_2p = Action::MoveLeft;
								break;
							case sf::Keyboard::Key::Right:
								action_2p = Action::MoveRight;
								break;
							case sf::Keyboard::Key::Up:
								action_2p = Action::Rotate;
								break;
							case sf::Keyboard::Key::Down:
								action_2p = Action::MoveDownPlus;
								break;
							case sf::Keyboard::Key::Comma:
								action_2p = Action::DashDown;
								break;
							case sf::Keyboard::Key::Period:
								hold_2p = true;
								break;
							case sf::Keyboard::Key::Escape:
								if (mode != Mode::Pause) {
									mode = Mode::Pause;
								}
								else {
									mode = Mode::VS2p;
								}
								break;
							case sf::Keyboard::Key::Space:
								if (mode == Mode::Pause) {
									restartGame = true;
									mode = Mode::VS2p;
								}
								break;
							case sf::Keyboard::Key::Enter:
								if (mode == Mode::Pause) {
									mode = Mode::MainMenu;
								}
								break;
							}


						}
					}
				}
				if (mode == Mode::VS2p) {
					if (clock_1p.getElapsedTime().asSeconds() >= speed_1p) {
						action_1p = Action::MoveDown;
						clock_1p.restart();
					}

					if (clock_2p.getElapsedTime().asSeconds() >= speed_2p) {
						action_2p = Action::MoveDown;
						clock_2p.restart();
					}

					if (hold_1p) {
						if (holdType_1p == Type::None) {
							holdType_1p = currentType_1p;
							holdShape_1p = shapes[holdType_1p][0];
							resetPosAndShape(pos_1p, origin_1p, currentType_1p, currentIndex_1p);
						}
						else {
							swapHold(currentType_1p, holdType_1p);
							holdShape_1p = shapes[holdType_1p][0];
							pos_1p = origin_1p;
							currentIndex_1p = 0;
						}
						hold_1p = false;
					}
					if (hold_2p) {
						if (holdType_2p == Type::None) {
							holdType_2p = currentType_2p;
							holdShape_2p = shapes[holdType_2p][0];
							resetPosAndShape(pos_2p, origin_2p, currentType_2p, currentIndex_2p);
						}
						else {
							swapHold(currentType_2p, holdType_2p);
							holdShape_2p = shapes[holdType_2p][0];
							pos_2p = origin_2p;
							currentIndex_2p = 0;
						}
						hold_2p = false;
					}
					sf::Vector2i nextPos_1p = pos_1p;
					int nextIndex_1p = currentIndex_1p;
					if (action_1p == Action::Rotate) {
						nextIndex_1p = (nextIndex_1p + 1) % shapes[currentType_1p].size();
					}
					std::vector<sf::Vector2i> nextShape_1p = shapes[currentType_1p][nextIndex_1p];

					sf::Vector2i nextPos_2p = pos_2p;
					int nextIndex_2p = currentIndex_2p;
					if (action_2p == Action::Rotate) {
						nextIndex_2p = (nextIndex_2p + 1) % shapes[currentType_2p].size();
					}
					std::vector<sf::Vector2i> nextShape_2p = shapes[currentType_2p][nextIndex_2p];

					if (!gameOver_1p && !gameOver_2p) {
						setNewPos(action_1p, nextPos_1p);
						setNewPos(action_2p, nextPos_2p);
					}

					if (action_1p == Action::DashDown) {
						while (nextPos_1p.y < fieldHeight) {
							int countEmpty = 0;
							for (const sf::Vector2i& d : nextShape_1p) {
								sf::Vector2i np = nextPos_1p + d;
								if (np.x >= 0 && np.x < fieldWidth &&
									np.y < fieldHeight &&
									(np.y < 0 || field[np.x][np.y] == Type::None)) {
									countEmpty++;
								}
							}
							if (countEmpty == 4) {
								pos_1p = nextPos_1p;
								nextPos_1p.y++;
							}
							else {
								break;
							}
						}
					}
					if (action_2p == Action::DashDown) {
						while (nextPos_2p.y < fieldHeight) {
							int countEmpty = 0;
							for (const sf::Vector2i& d : nextShape_2p) {
								sf::Vector2i np = nextPos_2p + d;
								if (np.x >= field2p_Lbound && np.x < field2p_Rbound &&
									np.y < fieldHeight &&
									(np.y < 0 || field[np.x][np.y] == Type::None)) {
									countEmpty++;
								}
							}
							if (countEmpty == 4) {
								pos_2p = nextPos_2p;
								nextPos_2p.y++;
							}
							else {
								break;
							}
						}
					}

					int countEmpty_1p = 0;
					for (const sf::Vector2i& d : nextShape_1p) {
						sf::Vector2i np = nextPos_1p + d;
						if (np.x >= 0 && np.x < fieldWidth &&
							np.y < fieldHeight &&
							(np.y < 0 || field[np.x][np.y] == Type::None)) {
							countEmpty_1p++;
						}
					}
					if (countEmpty_1p == 4) {
						pos_1p = nextPos_1p;
						currentIndex_1p = nextIndex_1p;
						currentShape_1p = nextShape_1p;
					}
					else {
						if (action_1p == Action::MoveDown ||
							action_1p == Action::MoveDownPlus ||
							action_1p == Action::DashDown) {

							for (const sf::Vector2i& d : currentShape_1p) {
								sf::Vector2i np = pos_1p + d;
								if (np.y >= 0) {
									field[np.x][np.y] = currentType_1p;
								}
							}
							int countFull = 0;
							for (int y = 0; y < fieldHeight; y++) {
								bool isFull = true;
								for (int x = 0; x < fieldWidth; x++) {
									if (field[x][y] == Type::None) {
										isFull = false;
										break;
									}
								}
								if (isFull) {
									lines_1p++;
									countFull++;
									for (int ty = y; ty >= 0; ty--) {
										for (int tx = 0; tx < fieldWidth; tx++) {
											if (ty > 0) {
												field[tx][ty] = field[tx][ty - 1];
											}
											else {
												field[tx][ty] = Type::None;
											}
										}
									}
									linesText_1p.setString(std::to_string(lines_1p));
									if (lines_1p > 0 && lines_1p % 5 == 0) {
										stages_1p++;
										stageText_1p.setString("Stage" + std::to_string(stages_1p));
										getNewSpeed(stages_1p, speed_1p);

									}
								}
							}
							// 1p滿條給2p長高
							if (countFull >= 2) {
								int obstacleLines = countFull / 2;

								for (int x = field2p_Lbound; x < field2p_Rbound; x++) {
									for (int y = 0; y < fieldHeight; y++) {
										if (y - obstacleLines >= 0) {
											field[x][y - obstacleLines] = field[x][y];
										}
									}
								}

								for (int y = fieldHeight - 1; y >= fieldHeight - obstacleLines; y--) {
									int r = rand() % 10;
									for (int k = 0; k <= 9; k++) {
										if (k == r) {
											field[k + field2p_Lbound][y] = Type::None;
										}
										else {
											field[k + field2p_Lbound][y] = Type::Obstacle;
										}
									}
								}

							}

							if (countFull != 0) {
								scoreAdd(countFull, scores_1p);
								scoresText_1p.setString(std::to_string(scores_1p));

							}

							for (int x = 0; x < fieldWidth; x++) {
								if (field[x][0] != Type::None) {
									for (const sf::Vector2i& d : currentShape_1p) {
										sf::Vector2i np = pos_1p + d;
										if (np.y < 0) {
											gameOver_1p = true;
											break;
										}
									}
								}
							}

							if (!gameOver_1p) {
								resetPosAndShape(pos_1p, origin_1p, currentType_1p, currentIndex_1p);
							}
						}
					}
					// 2p
					int countEmpty_2p = 0;
					for (const sf::Vector2i& d : nextShape_2p) {
						sf::Vector2i np = nextPos_2p + d;
						if (np.x >= field2p_Lbound && np.x < field2p_Rbound &&
							np.y < fieldHeight &&
							(np.y < 0 || field[np.x][np.y] == Type::None)) {
							countEmpty_2p++;
						}
					}
					if (countEmpty_2p == 4) {
						pos_2p = nextPos_2p;
						currentIndex_2p = nextIndex_2p;
						currentShape_2p = nextShape_2p;
					}
					else {
						if (action_2p == Action::MoveDown ||
							action_2p == Action::MoveDownPlus ||
							action_2p == Action::DashDown) {

							for (const sf::Vector2i& d : currentShape_2p) {
								sf::Vector2i np = pos_2p + d;
								if (np.y >= 0) {
									field[np.x][np.y] = currentType_2p;
								}
							}
							int countFull = 0;
							for (int y = 0; y < fieldHeight; y++) {
								bool isFull = true;
								for (int x = field2p_Lbound; x < field2p_Rbound; x++) {
									if (field[x][y] == Type::None) {
										isFull = false;
										break;
									}
								}
								if (isFull) {
									lines_2p++;
									countFull++;
									for (int ty = y; ty >= 0; ty--) {
										for (int tx = field2p_Lbound; tx < field2p_Rbound; tx++) {
											if (ty > 0) {
												field[tx][ty] = field[tx][ty - 1];
											}
											else {
												field[tx][ty] = Type::None;
											}
										}
									}
									linesText_2p.setString(std::to_string(lines_2p));
									if (lines_2p > 0 && lines_2p % 5 == 0) {
										stages_2p++;
										stageText_2p.setString("Stage" + std::to_string(stages_2p));
										getNewSpeed(stages_2p, speed_2p);
									}
								}
							}
							// 2p滿條給1p長高
							if (countFull >= 2) {
								int obstacleLines = countFull / 2;

								for (int x = 0; x < fieldWidth; x++) {
									for (int y = 0; y < fieldHeight; y++) {
										if (y - obstacleLines >= 0) {
											field[x][y - obstacleLines] = field[x][y];
										}
									}
								}

								for (int y = fieldHeight - 1; y >= fieldHeight - obstacleLines; y--) {
									int r = rand() % 10;
									for (int k = 0; k <= 9; k++) {
										if (k == r) {
											field[k][y] = Type::None;
										}
										else {
											field[k][y] = Type::Obstacle;
										}
									}
								}

							}


							if (countFull != 0) {
								scoreAdd(countFull, scores_2p);
								scoresText_2p.setString(std::to_string(scores_2p));
							}

							for (int x = field2p_Lbound; x < field2p_Rbound; x++) {
								if (field[x][0] != Type::None) {
									for (const sf::Vector2i& d : currentShape_2p) {
										sf::Vector2i np = pos_2p + d;
										if (np.y < 0) {
											gameOver_2p = true;
											break;
										}
									}
								}
							}

							if (!gameOver_2p) {
								resetPosAndShape(pos_2p, origin_2p, currentType_2p, currentIndex_2p);
							}
						}
					}

					window_2p.clear();
					window_2p.draw(backgroundSprite_1p);
					window_2p.draw(block_gridSprite_1p);
					window_2p.draw(backgroundSprite_2p);
					window_2p.draw(block_gridSprite_2p);

					for (int x = 0; x < fieldWidth; x++) {
						for (int y = 0; y < fieldHeight; y++) {
							if (field[x][y] != Type::None) {
								sf::Vector2i pos(x, y);
								blockSprites_1p[field[x][y]].setPosition(getPixelPos(blockSize, pos));
								window_2p.draw(blockSprites_1p[field[x][y]]);
							}
						}
					}
					for (int x = field2p_Lbound; x < field2p_Rbound; x++) {
						for (int y = 0; y < fieldHeight; y++) {
							if (field[x][y] != Type::None) {
								sf::Vector2i pos(x, y);
								blockSprites_2p[field[x][y]].setPosition(getPixelPos(blockSize, pos));
								window_2p.draw(blockSprites_2p[field[x][y]]);
							}
						}
					}

					if (!gameOver_1p && !gameOver_2p) {
						for (const sf::Vector2i& d : currentShape_1p) {
							sf::Vector2i np = pos_1p + d;
							blockSprites_1p[currentType_1p].setPosition(getPixelPos(blockSize, np));
							window_2p.draw(blockSprites_1p[currentType_1p]);
						}

						if (holdType_1p != Type::None) {
							for (const sf::Vector2i& d : holdShape_1p) {
								sf::Vector2i np = sf::Vector2i(fieldWidth + 6, 3) + d;
								blockSprites_1p[holdType_1p].setPosition(getPixelPos(blockSize, np));
								window_2p.draw(blockSprites_1p[holdType_1p]);
							}
						}

						for (const sf::Vector2i& d : currentShape_2p) {
							sf::Vector2i np = pos_2p + d;
							blockSprites_2p[currentType_2p].setPosition(getPixelPos(blockSize, np));
							window_2p.draw(blockSprites_2p[currentType_2p]);
						}

						if (holdType_2p != Type::None) {
							for (const sf::Vector2i& d : holdShape_2p) {
								sf::Vector2i np = sf::Vector2i(field2p_Rbound + 6, 3) + d;
								blockSprites_2p[holdType_2p].setPosition(getPixelPos(blockSize, np));
								window_2p.draw(blockSprites_2p[holdType_2p]);
							}
						}
					}
					else {
						if (gameOver_1p) {
							for (int i = 1; i <= 7; i++) {
								blockSprites_1p[Type(i)].setColor(sf::Color(70, 70, 70));
							}
							LoseText.setString("    1P\nLose");
							LoseText.setPosition(
								(backgroundTexture.getSize().x / 2.f - LoseText.getLocalBounds().width) / 2,
								(backgroundTexture.getSize().y - LoseText.getLocalBounds().height) / 2);
							WinText.setString("   2P\nWin");
							WinText.setPosition(
								window2p_x + (backgroundTexture.getSize().x / 2.f - WinText.getLocalBounds().width) / 2,
								(backgroundTexture.getSize().y - WinText.getLocalBounds().height) / 2);
						}
						else if (gameOver_2p) {
							for (int i = 1; i <= 7; i++) {
								blockSprites_2p[Type(i)].setColor(sf::Color(70, 70, 70));
							}
							LoseText.setString("    2P\nLose");
							LoseText.setPosition(
								window2p_x + (backgroundTexture.getSize().x / 2.f - LoseText.getLocalBounds().width) / 2,
								(backgroundTexture.getSize().y - LoseText.getLocalBounds().height) / 2);
							WinText.setString("   1P\nWin");
							WinText.setPosition(
								(backgroundTexture.getSize().x / 2.f - WinText.getLocalBounds().width) / 2,
								(backgroundTexture.getSize().y - WinText.getLocalBounds().height) / 2);
						}
						window_2p.draw(LoseText);
						window_2p.draw(WinText);
					}

					if (!gameOver_1p && !gameOver_2p) {
						if (mode == Mode::Pause) {
							timer.restart();
						}
						if ((int)timer.getElapsedTime().asSeconds() >= 1) {
							seconds++;
							if (seconds == 60) {
								minutes++;
								seconds = 0;
							}
							timer.restart();
						}

						if (seconds < 10) {
							timerText_1p.setString(std::to_string(minutes) + ":0" + std::to_string(seconds));
							timerText_2p.setString(std::to_string(minutes) + ":0" + std::to_string(seconds));
						}
						else {
							timerText_1p.setString(std::to_string(minutes) + ":" + std::to_string(seconds));
							timerText_2p.setString(std::to_string(minutes) + ":" + std::to_string(seconds));
						}
					}
				}
				for (const sf::Text* t : stateTexts_1p) {
					window_2p.draw((*t));
				}
				for (const sf::Text* t : stateTexts_2p) {
					window_2p.draw((*t));
				}
				window_2p.display();
				if (gameOver_1p || gameOver_2p) {
					sf::sleep(sf::seconds(3));
					mode = Mode::MainMenu;
					window_2p.setVisible(false);
					window_Main.setVisible(true);
				}
			}
		}
	}
	ImGui::SFML::Shutdown();
	return EXIT_SUCCESS;
}