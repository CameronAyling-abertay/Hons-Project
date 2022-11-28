#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <World.h>

#define DEFAULT_SIDE 40

class DemoApp
{
private:
	sf::RenderWindow* window;

	sf::Clock clock;
	sf::Time currentTime;

	int width;
	int height;
	EcoResilience::World world;

	void handleUpdate();
	void update(float deltaTime);
	void render();

public:
	DemoApp();
	~DemoApp() {};

	void init();
	void run();
};