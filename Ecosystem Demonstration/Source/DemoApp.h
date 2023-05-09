#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <EcoApp.h>

class DemoApp
{
	//General running
	sf::RenderWindow* window;

	bool paused;
	sf::Clock clock;

	void handleInput();
	void update(sf::Time deltaTime);
	void render();

	//EcoResilience
	EcoApp ecology;

	float timebank;
	float speed;
	int stepCount;
	bool perlin;
	bool viewSeparate;

public:
	DemoApp();
	~DemoApp() = default;

	void init();
	void run();
};