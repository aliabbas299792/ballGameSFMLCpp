#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

//enum definitions
enum gameScreens { entrance, game, scoreboard, end }; //make enum for all the screens

#include "utilFunctions.h"
#include "platforms.h"

//static var definitions
//std::vector<platforms*> platforms::platformStuff;

int main() {
	//consts and vars
	const float accelerationX = 0.1;
	const float decelerationX = 0.03;
	const float accelerationY = 6;
	const float gravityOriginal = 0.15;

	int windowWidth = 900;
	int windowHeight = 600;
	bool removeGravity = false;
	bool hitFloor = false;
	float gravity = 0.15;
	float velocityX = 0;
	float velocityY = 0;
	bool justChanged = false;
	int score = 0;
	std::string scoreboard_text;

	gameScreens gameState = entrance; //set initial gamestate to entrance

	//window settings
	sf::ContextSettings settings; //make settings variable
	sf::Image windowIcon; //the title bar/corner icon thing
	settings.antialiasingLevel = 5; //set anti-aliasing level to 5

	if (!windowIcon.loadFromFile("resources/char.png")) { //the character as an icon seems alright, so loads it
		std::cout << "Error: Couldn't load image." << std::endl;
	}
	const sf::Uint8 *iconPixel = windowIcon.getPixelsPtr(); //then gets a pointer to an array of the pixels making up the image
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Ball", sf::Style::Titlebar, settings); 
	//window, titlebar option disables resizing and removes titlebar
	windowWidth = window.getSize().x; //get actual width
	windowHeight = window.getSize().y; //get actual height

	window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, iconPixel);
	//(refering to above) sets the icon thing, takes the original image's width, then height, then the pixel array from earlier
	
	//sfml object definitions
	sf::Texture spriteTex;
	sf::Sprite sprite1;
	sf::RectangleShape scoreboardAndFps(sf::Vector2f(100, 60)); //make the fps box thing
	sf::View windowView(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)); //make the moving view that will be centered on the ball
	sf::RectangleShape backgroundGround(sf::Vector2f(windowWidth, 100));
	sf::RectangleShape buttonStart(sf::Vector2f(200, 75)); //start button
	sf::Texture btnStartTex; //the respective texture
	sf::RectangleShape buttonHighScore(sf::Vector2f(150, 56.25)); //high score button
	sf::Texture btnHighScreTex; //the respective texture
	sf::RectangleShape buttonBack(sf::Vector2f(100, 37.5)); //back button
	sf::Texture btnBackTex; //the respective texture
	sf::RectangleShape buttonExit(sf::Vector2f(100, 37.5)); //back button
	sf::Texture btnExitTex; //the respective texture
	sf::RectangleShape logo(sf::Vector2f(300, 300)); //the logo in start screen
	sf::Texture logoTex; //the respective texture
	sf::RectangleShape scoreboard_board(sf::Vector2f(520, 540)); //scoreboard rect
	sf::Font lato; //lato font
	sf::Text textScoreBoardHead("Scoreboard:", lato); //the scoreboard title
	sf::Text textScoreBoard("", lato); //the scores
	sf::Text textScoreLive("", lato); //the scores

	//loading stuff, or making rectangles/sprites with colors and stuff, or setting position of stuff
	loadTextureSprite(spriteTex, sprite1, "resources/char.png", windowWidth / 2 - 15, 100); //sprite
	loadTexture(btnStartTex, buttonStart, "resources/btnStart.png", windowWidth/2-buttonStart.getSize().x/2, 360); //start button
	loadTexture(btnBackTex, buttonBack, "resources/btnBack.png", windowWidth - buttonBack.getSize().x - 25, 25); //back button
	loadTexture(btnHighScreTex, buttonHighScore, "resources/btnHighScre.png", windowWidth / 2 - buttonHighScore.getSize().x / 2, 460); //high score button
	loadTexture(btnExitTex, buttonExit, "resources/btnExit.png", windowWidth / 2 - buttonExit.getSize().x / 2, 541.25); //high score button
	loadTexture(logoTex, logo, "resources/logo.png", windowWidth / 2 - logo.getSize().x / 2, 30);
	lato.loadFromFile("resources/Lato-Light.ttf"); //the actual font, lato

	sprite1.setPosition(300.0f,100.0f); //set sprite initial position
	sprite1.setTexture(spriteTex); //set the texture
	sprite1.setOrigin(15, 15); //set origin for transformation to center

	scoreboardAndFps.setPosition(0, 0); //set the fps box holder thing to top left
	scoreboardAndFps.setFillColor(sf::Color(30, 30, 30, 150)); //set it's color

	//scoreboard stuff under the same subheading
	scoreboard_board.setFillColor(sf::Color(50, 50, 50, 255)); //set scoreboard's color
	scoreboard_board.setPosition(windowWidth / 2 - scoreboard_board.getSize().x / 2, 30); //set the board to halfway across screen

	textScoreBoardHead.setPosition(sf::Vector2f(windowWidth / 2 - scoreboard_board.getSize().x / 2 + 40, 40)); //sets scoreboard title to halfway across screen
	textScoreBoardHead.setCharacterSize(60);

	//making collision objects
	chunksHolder *liveChunk = new chunksHolder(windowWidth, windowHeight, (0-windowWidth), (0-windowHeight)); //this is the initial one
	//1st/2nd params are obvsly window width/height, and 3rd/4th are the beginning of the chunks
	
	int i = 0; //for the iteration through the objects to check for collisions/draw them
	int j = 0; //to loop through chunk itself
	bool tempTorF = false; //check if there is a collison variable

	//timing and fps
	sf::Clock gameClock; //start the clock
	sf::Time elapsedTime = gameClock.getElapsedTime(); //get elapsed time at this instant
	sf::Time timeSinceCollision;
	sf::Time currentTime; //define the variable to compare (will be current time)

	sf::Text fpsText("0 FPS", lato); //the fps text
	fpsText.setPosition(10, 10);
	fpsText.setCharacterSize(15);
	int fps = 0; //set initial value of fps to 0
	sf::Time elapsedTimeFps = gameClock.getElapsedTime(); //

	while (window.isOpen()) {
		sf::Event events; 

		window.clear(sf::Color(10,10,15,255)); //set default background to black

		currentTime = gameClock.getElapsedTime(); //gets elapsed time since the last time it was measured

		textScoreLive = sf::Text("Score: " + std::to_string(score), lato);
		textScoreLive.setPosition(10, 30); //set the position on the screen
		textScoreLive.setCharacterSize(15); //font size of 15

		if (currentTime.asMilliseconds() >= elapsedTimeFps.asMilliseconds() + 1000) { //if a second has passed...
			fpsText = sf::Text(std::to_string(fps) + " FPS", lato); //update the fps text
			fpsText.setPosition(10, 10); //set the position on the screen
			fpsText.setCharacterSize(15); //font size of 15

			fps = 0; //reset the fps counter (it's per second, so needs to be of course)
			elapsedTimeFps = currentTime; //sets the variable to current time, to measure for passing of 1 second in later loop
		}

		if (currentTime.asMilliseconds() < elapsedTime.asMilliseconds() + 9) { //check if 9ms has passed
			continue; //if it hasn't, skip processing for this loop
		}
		else {
			fps++; //otherwise increment fps
			elapsedTime = currentTime;
		}

		switch (gameState) {
			case entrance:
				//below are just the button stuff, so highlighting, and working, to change game state and move to another screen after clicking
				selectionProcessor(gameState, end, buttonExit, window, justChanged, "scoreboard.txt", score);
				selectionProcessor(gameState, game, buttonStart, window, justChanged, "scoreboard.txt", score);
				selectionProcessor(gameState, scoreboard, buttonHighScore, window, justChanged, "scoreboard.txt", score);

				//and draw the buttons and logo
				window.draw(buttonStart);
				window.draw(buttonExit);
				window.draw(buttonHighScore);
				window.draw(logo);
				break;
			case game:
				if (justChanged == true) { //esentially set it to initial positions and stuff
					score = 0;
					sprite1.setPosition(300.0f, 100.0f); //set sprite initial position
					velocityX = 0;
					velocityY = 0;
					timeSinceCollision = gameClock.getElapsedTime();

					liveChunk = new chunksHolder(windowWidth, windowHeight, (0 - windowWidth), (0 - windowHeight)); //reset chunks

					justChanged = false;
				}

				if (currentTime.asMilliseconds() - timeSinceCollision.asMilliseconds() >= 2000) {
					liveChunk->~chunksHolder();
					onDeath(gameState, "scoreboard.txt", score); //if it's been falling for at least 2 seconds then die
				}

				removeGravity = false; //initialise bool to false
				tempTorF = false; //initialise this bool to false as well

				for (i = 0; i < liveChunk->chunksLoaded.size(); i++) { //iterates through chunk vector
					for (j = 0; j < liveChunk->chunksLoaded[i].platformsInTheChunk.size(); j++) { //iterates through individual chunks
						if (liveChunk->chunksLoaded[i].platformsInTheChunk[j].checkIntersect(sprite1, gravity, velocityX, velocityY, gravityOriginal, hitFloor, removeGravity, score, timeSinceCollision, gameClock) && tempTorF == false) {
							//if the above evaluates to true even once, the variable below is set to true for this entire iteration of the loop
							//the if statement checks for collisions
							tempTorF = true;
						}
					}
				}

				if (tempTorF) //if it's true...
				{
					moveBall(windowView, window, sprite1, velocityX, velocityY, accelerationX, accelerationY, gravity, gravityOriginal, true, hitFloor, decelerationX, removeGravity);
					//then move the ball, while accounting for the collision has taken place
				}
				else {
					//otherwise act as if there was no collision
					moveBall(windowView, window, sprite1, velocityX, velocityY, accelerationX, accelerationY, gravity, gravityOriginal, false, hitFloor, decelerationX, removeGravity);
				}

				window.setView(windowView); //sets the view to the moving view, and draws stuff relative to it

				for (i = 0; i < liveChunk->chunksLoaded.size(); i++) { //iterates through chunk vector
					for (j = 0; j < liveChunk->chunksLoaded[i].platformsInTheChunk.size(); j++) { //iterates through individual chunks
						window.draw(*liveChunk->chunksLoaded[i].platformsInTheChunk[j].shape()); //draws the platforms
					}
				}

				liveChunk->updateChunks(sprite1, windowWidth, windowHeight);

				window.draw(sprite1); //and draws the sprite

				window.setView(window.getDefaultView()); //sets back to the normal default view
				window.draw(buttonBack); //draws the back button
				window.draw(scoreboardAndFps); //draws the fps text and (what will probably be score board) box
				window.draw(fpsText); //draws the fps text
				window.draw(textScoreLive); //draw the score text
				
				selectionProcessor(gameState, entrance, buttonBack, window, justChanged, "scoreboard.txt", score);
				//and adds the respective selection processor for the back button

				break;
			case scoreboard:
				if (justChanged == true) { //on first iteration when the scoreboard has just been clicked on
					scoreboard_init("scoreboard.txt", scoreboard_text); //sort all the scores, and limit the total amount to 9

					textScoreBoard = sf::Text(scoreboard_text, lato, 37); //set the text font and size
					textScoreBoard.setPosition(sf::Vector2f(windowWidth / 2 - scoreboard_board.getSize().x / 2 + 40, 130)); //and centers it on x axis

					justChanged = false; //and set the just clicked on thing to false, for after this processing
				}

				window.draw(scoreboard_board); //draw the scoreboard
				window.draw(textScoreBoardHead); //draw the title
				window.draw(textScoreBoard); //draw the scores
				//
				window.draw(buttonBack); //draw back button
				selectionProcessor(gameState, entrance, buttonBack, window, justChanged, "scoreboard.txt", score); //and the back buttons's selection processor thing here too
				break;
			case end:
				window.close();
				return 0; //and end if exited
				break;
		}
		
		while (window.pollEvent(events))
		{
			//gotta poll events regardless of whether or not we use em, otherwise window will become unresponsive
			if (events.type == sf::Event::Closed)
				window.close();
		}

		window.display(); //display everything drawn
	}
}