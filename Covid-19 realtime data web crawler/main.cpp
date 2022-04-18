#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <SFML/Graphics.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <ctime>
#include <cstdio>


int main() {
	// 更新時間
	struct tm newtime;
	__time64_t long_time;
	char timebuf[26];
	errno_t err;
	_time64(&long_time);
	err = _localtime64_s(&newtime, &long_time);
	if (err) {
		std::cout << "Error Code: " << (int)err << std::endl;
		return EXIT_FAILURE;
	}
	err = asctime_s(timebuf, 26, &newtime);
	if (err) {
		std::cout << "Error Code: " << (int)err << std::endl;
		return EXIT_FAILURE;
	}
	strftime(timebuf, 26, "%F %R", &newtime);

	//更新全球數據
	httplib::SSLClient client("www.worldometers.info");
	std::string data = client.Get("/coronavirus/")->body;
	size_t spanNum = data.find("<span style=\"color:#aaa\">");
	std::string span = "<span style=\"color:#aaa\">";
	size_t confirmedBegin = spanNum + span.size();
	size_t confirmedEnd = data.find("</span>", confirmedBegin);
	std::string confirmed = data.substr(confirmedBegin, confirmedEnd - confirmedBegin);
	size_t deathBegin = data.find("<span>", confirmedEnd) + 6;
	size_t deathEnd = data.find("</span>", deathBegin);
	std::string death = data.substr(deathBegin, deathEnd - deathBegin);
	size_t recoveredBegin = data.find("<span>", deathEnd) + 6;
	size_t recoveredEnd = data.find("</span>", recoveredBegin);
	std::string recovered = data.substr(recoveredBegin, recoveredEnd - recoveredBegin);

	//更新台灣數據

	std::string dataTW = client.Get("/coronavirus/country/taiwan/")->body;
	size_t spanNumTW = dataTW.find("<span style=\"color:#aaa\">");
	size_t confirmedBeginTW = spanNumTW + span.size();
	size_t confirmedEndTW = dataTW.find("</span>", confirmedBeginTW);
	std::string confirmedTW = dataTW.substr(confirmedBeginTW, confirmedEndTW - confirmedBeginTW);
	size_t deathBeginTW = dataTW.find("<span>", confirmedEndTW) + 6;
	size_t deathEndTW = dataTW.find("</span>", deathBeginTW);
	std::string deathTW = dataTW.substr(deathBeginTW, deathEndTW - deathBeginTW);
	size_t recoveredBeginTW = dataTW.find("<span>", deathEndTW) + 6;
	size_t recoveredEndTW = dataTW.find("</span>", recoveredBeginTW);
	std::string recoveredTW = dataTW.substr(recoveredBeginTW, recoveredEndTW - recoveredBeginTW);
	
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		return EXIT_FAILURE;
	}
	//全球數據顯示的圖片及文字來源
	sf::Texture confirmedTexture;
	if (!confirmedTexture.loadFromFile("confirmed.png")) return EXIT_FAILURE;
	sf::Texture deathTexture;
	if (!deathTexture.loadFromFile("death.png")) return EXIT_FAILURE;
	sf::Texture recoveredTexture;
	if (!recoveredTexture.loadFromFile("recovered.png")) return EXIT_FAILURE;

	sf::Text globalTitle("Global", font, 50);
	globalTitle.setFillColor(sf::Color(255, 128, 192));
	globalTitle.setOutlineColor(sf::Color(255, 128, 192));
	globalTitle.setOutlineThickness(1.5f);
	globalTitle.setPosition((confirmedTexture.getSize().x - globalTitle.getLocalBounds().width) / 2.f, 10);
	sf::String confirmedString(confirmed);
	sf::Text confirmedText(confirmedString, font);
	confirmedText.setPosition(200, 10 + globalTitle.getLocalBounds().height * 1.5f);
	sf::String deathString(death);
	sf::Text deathText(deathString, font);
	deathText.setPosition(200, confirmedText.getPosition().y + confirmedTexture.getSize().y * 1.05f);
	sf::String recoveredString(recovered);
	sf::Text recoveredText(recoveredString, font);
	recoveredText.setPosition(200, deathText.getPosition().y + confirmedTexture.getSize().y * 1.05f);

	sf::Sprite confirmedSprite(confirmedTexture);
	confirmedSprite.setPosition(15, confirmedText.getPosition().y);
	sf::Sprite deathSprite(deathTexture);
	deathSprite.setPosition(15, deathText.getPosition().y);
	sf::Sprite recoveredSprite(recoveredTexture);
	recoveredSprite.setPosition(15, recoveredText.getPosition().y);

	//台灣數據顯示的圖片及文字來源
	sf::Text taiwanTitle("Taiwan", font, 50);
	taiwanTitle.setFillColor(sf::Color(255, 128, 192));
	taiwanTitle.setOutlineColor(sf::Color(255, 128, 192));
	taiwanTitle.setOutlineThickness(1.5f);
	taiwanTitle.setPosition(globalTitle.getPosition().x,
		recoveredSprite.getPosition().y * 1.1f + confirmedTexture.getSize().y * 1.05f);
	sf::Text confirmedTextTW(confirmedTW, font);
	sf::Text deathTextTW(deathTW, font);
	sf::Text recoveredTextTW(recoveredTW, font); 
	confirmedTextTW.setPosition(200, taiwanTitle.getPosition().y + taiwanTitle.getLocalBounds().height * 1.5f);
	deathTextTW.setPosition(200, confirmedTextTW.getPosition().y + confirmedTexture.getSize().y * 1.05f);
	recoveredTextTW.setPosition(200, deathTextTW.getPosition().y + confirmedTexture.getSize().y * 1.05f);
	sf::Sprite confirmedSpriteTW(confirmedTexture);
	confirmedSpriteTW.setPosition(15, confirmedTextTW.getPosition().y);
	sf::Sprite deathSpriteTW(deathTexture);
	deathSpriteTW.setPosition(15, deathTextTW.getPosition().y);
	sf::Sprite recoveredSpriteTW(recoveredTexture);
	recoveredSpriteTW.setPosition(15, recoveredTextTW.getPosition().y);
	bool showTW = false;
	
	
	//視窗設置
	const int windowWidth = 1600;
	const int windowHeight = 900;
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), L"COVID-19");
	window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y / 2));
	window.setFramerateLimit(30);

	//更新時間設置
	sf::Clock clock;
	sf::Text updatingTime("Last Updated: " + std::string(timebuf), font, 20);
	updatingTime.setPosition(
		float(windowWidth - updatingTime.getLocalBounds().width * 1.05),
		float(windowHeight - updatingTime.getLocalBounds().height * 1.2));

	//繪製趨勢圖
	//https://covid-19.nchc.org.tw/api/covid19?CK=covid-19@nchc.org.tw&querydata=4001&limited=TWN
	httplib::SSLClient clientTW("covid-19.nchc.org.tw");
	std::string dataTWtrend = clientTW.Get("/api/covid19?CK=covid-19@nchc.org.tw&querydata=4001&limited=TWN")->body;
	std::vector<std::string> cumConfirmed = {};
	nlohmann::json j = nlohmann::json::parse(dataTWtrend);
	for (int i = int(j.size()) - 1; i >= 0; i--) {
		cumConfirmed.push_back(j[i]["a05"]);
	}

	sf::RectangleShape x_axis(sf::Vector2f(cumConfirmed.size()*1.1f + 50.f, 10));
	sf::RectangleShape y_axis(sf::Vector2f(10, cumConfirmed.size()*0.8f + 10.f));
	x_axis.setPosition(windowWidth - x_axis.getLocalBounds().width*1.2f, 
		windowHeight - updatingTime.getLocalBounds().height * 5);
	y_axis.setPosition(x_axis.getPosition().x, x_axis.getPosition().y - y_axis.getLocalBounds().height);
	sf::ConvexShape trendTW(cumConfirmed.size());
	sf::CircleShape trendP(2);
	trendP.setFillColor(sf::Color::Red);
	sf::Text timeText("Time", font);
	sf::Text cumConfirmedText("Cumulative\nconfirmed", font);
	sf::Text trendTWtext("Covid-19 Cumulative\nConfirmed Trending\n(Taiwan, since 2020/1/16)", font, 50);
	timeText.setPosition(x_axis.getPosition().x + x_axis.getLocalBounds().width + 50.f, 
		x_axis.getPosition().y - timeText.getLocalBounds().height);
	cumConfirmedText.setPosition(y_axis.getPosition().x-cumConfirmedText.getLocalBounds().width/2, 20);
	trendTWtext.setPosition(x_axis.getPosition().x +
		(x_axis.getLocalBounds().width - trendTWtext.getLocalBounds().width) / 2, 20);
	bool showPlotTW = false;

	while (window.isOpen()) {
		
		if (clock.getElapsedTime().asSeconds() >= 60) {
			//時間更新
			_time64(&long_time);
			err = _localtime64_s(&newtime, &long_time);
			if (err) {
				std::cout << "Error Code: " << (int)err << std::endl;
				return EXIT_FAILURE;
			}
			err = asctime_s(timebuf, 26, &newtime);
			if (err) {
				std::cout << "Error Code: " << (int)err << std::endl;
				return EXIT_FAILURE;
			}
			strftime(timebuf, 26, "%F %R", &newtime);
			updatingTime.setString("Last Updated: " + std::string(timebuf));

			//全世界數據更新
			data = client.Get("/coronavirus/")->body;
			confirmed = data.substr(confirmedBegin, confirmedEnd - confirmedBegin);
			death = data.substr(deathBegin, deathEnd - deathBegin);
			recovered = data.substr(recoveredBegin, recoveredEnd - recoveredBegin);
			confirmedText.setString(confirmed);
			deathText.setString(death);
			recoveredText.setString(recovered);

			//台灣數據更新
			dataTW = client.Get("/coronavirus/country/taiwan/")->body;
			confirmedTW = dataTW.substr(confirmedBeginTW, confirmedEndTW - confirmedBeginTW);
			deathTW = dataTW.substr(deathBeginTW, deathEndTW - deathBeginTW);
			std::string recoveredTW = dataTW.substr(recoveredBeginTW, recoveredEndTW - recoveredBeginTW);
			confirmedTextTW.setString(confirmedTW);
			deathTextTW.setString(deathTW);
			recoveredTextTW.setString(recoveredTW);

			clock.restart();
		}

		sf::Event evt;
		if (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				window.close();
			}
			if (evt.type == sf::Event::KeyPressed) {
				switch (evt.key.code) {
					case sf::Keyboard::Key::T:
						if (!showTW) {
							showTW = true;
						}
						else {
							showTW = false;
						}
						break;
					case sf::Keyboard::Key::P:
						if (!showPlotTW) {
							showPlotTW = true;
						}
						else {
							showPlotTW = false;
						}
						break;
				}
				
			}
		}

		window.clear();
		//畫全世界統計數據
		window.draw(globalTitle);
		window.draw(confirmedSprite);
		window.draw(deathSprite);
		window.draw(recoveredSprite);
		window.draw(confirmedText);
		window.draw(deathText);
		window.draw(recoveredText);

		//畫台灣統計數據
		if (showTW) {
			window.draw(taiwanTitle);
			window.draw(confirmedSpriteTW);
			window.draw(deathSpriteTW);
			window.draw(recoveredSpriteTW);
			window.draw(confirmedTextTW);
			window.draw(deathTextTW);
			window.draw(recoveredTextTW);
		}

		//畫更新時間
		window.draw(updatingTime);

		//畫趨勢圖
		if (showPlotTW) {
			window.draw(trendTWtext);
			window.draw(x_axis);
			window.draw(y_axis);
			window.draw(timeText);
			window.draw(cumConfirmedText);
			for (int i = 0; i < int(cumConfirmed.size()); i++) {
				trendP.setPosition(x_axis.getPosition().x + y_axis.getLocalBounds().width + i,
					x_axis.getPosition().y - float(std::stoi(cumConfirmed[i])) / 50);
				window.draw(trendP);
			}
		}
		window.display();
	}
	return EXIT_SUCCESS;
}