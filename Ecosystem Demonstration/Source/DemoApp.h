#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <EcoApp.h>

class DemoApp
{
	//General application
	sf::RenderWindow* window;

	void handleInput();
	void update(sf::Time deltaTime);
	void render();

	//EcoResilience
	EcoApp ecology;

	EcoResilience::EcoSize size;
	EcoResilience::EcoParameters parameters;

	bool paused;
	sf::Clock clock;
	float timebank;
	float speed;

	//Diagnostic data
	int stepCount;
	bool perlin;
	bool viewSeparate;

public:
	DemoApp();
	~DemoApp() = default;

	//Starting and running the application
	void init();
	void run();
};