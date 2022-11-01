#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class DemoApp
{
private:
	sf::RenderWindow* window;

	sf::RectangleShape rect;
	float speed;
	float scale;

	sf::Clock clock;
	sf::Time currentTime;

	void handleUpdate();
	void update(float deltaTime);
	void render();

public:
	void init();
	void run();
};