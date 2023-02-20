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
	sf::Text plantDiagnostic;

	void handleInput();
	void update(float deltaTime);
	void render();

	EcoApp ecology;

	int stepCount;
	bool paused;
	bool cont;

public:
	DemoApp();
	~DemoApp() {};

	void init();
	void run();
};