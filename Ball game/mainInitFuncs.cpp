#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "utilFunctions.h"

void selectionProcessor(gameScreens &gameState, gameScreens nextScreen, sf::RectangleShape &button, sf::RenderWindow &window, bool &justChanged, std::string filePath, int &score) {
	//get the position of the mouse
	int mouseX = sf::Mouse::getPosition(window).x;
	int mouseY = sf::Mouse::getPosition(window).y;

	//if the mouse is within the boundaries of the button
	if (mouseX <= button.getPosition().x + button.getSize().x && mouseX >= button.getPosition().x && mouseY >= button.getPosition().y && mouseY <= button.getPosition().y + button.getSize().y) {
		button.setFillColor(sf::Color(255, 255, 255, 170)); //reduce the transparency of the button if hovering over it
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { //and if it's clicked on
			if (gameState == game) {
				std::fstream scores;
				scores.open(filePath, std::ios::in);
				std::string temp1 = "";
				std::string temp = "";
				int temp3 = 1;

				while (std::getline(scores, temp1)) { //get all lines
					if (temp3 == 1) {
						temp += temp1;
						temp3++;
						continue;
					}

					temp += '\n' + temp1;
				}

				scores.close();

				scores.open(filePath, std::ios::out);
				temp = temp + '\n' + std::to_string(score);
				scores << temp;
				scores.close();
			}

			justChanged = true; //set the just changed variable to true, for processes that need to run once per screen
			gameState = nextScreen; //set the game state to the provided next screen enum
		}
	}
	else {
		button.setFillColor(sf::Color(255, 255, 255, 255)); //otherwise set the opacity to 100%
	}
}

void loadTexture(sf::Texture &texture, sf::RectangleShape &rect, std::string filePath, float posX, float posY) { //texture loading for rectangles
	if (!texture.loadFromFile(filePath)) {
		std::cout << "Error: Couldn't load image." << std::endl;
	}
	rect.setTexture(&texture);
	rect.setPosition(posX, posY);
}

void loadTextureSprite(sf::Texture &texture, sf::Sprite &sprite, std::string filePath, float posX, float posY) { //texture loading for sprites
	if (!texture.loadFromFile(filePath)) {
		std::cout << "Error: Couldn't load image." << std::endl;
	}
	sprite.setTexture(texture);
	sprite.setPosition(posX, posY);
}