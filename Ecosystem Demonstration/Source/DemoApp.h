#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <World.h>
#include <EcoApp.h>

class DemoApp
{
private:
	sf::RenderWindow* window;

	sf::Clock clock;
	sf::Time currentTime;
	sf::Font font;
	sf::Text waterDiagnostic;

	void handleInput();
	void update(float deltaTime);
	void render();

	EcoApp ecology;

public:
	DemoApp();
	~DemoApp() {};

	void init();
	void run();
};