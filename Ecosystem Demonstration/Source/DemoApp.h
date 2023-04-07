#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <World.h>
#include <EcoApp.h>

class DemoApp
{
private:
	sf::RenderWindow* window;

	sf::Clock clock;

	void handleInput();
	void update(sf::Time deltaTime);
	void render();

	EcoApp ecology;

	int stepCount;
	bool perlin;

	bool viewSeparate;

public:
	DemoApp();
	~DemoApp() {};

	void init();
	void run();
};