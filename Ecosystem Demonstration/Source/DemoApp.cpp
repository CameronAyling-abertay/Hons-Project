#include "DemoApp.h"

void DemoApp::init()
{
    //Initialise window and starting variables
    window = new sf::RenderWindow(sf::VideoMode(1600, 900), "Ecosystem Resilience Demonstration");

    rect.setFillColor(sf::Color::Green);
    speed = 0.001f;
    scale = 100.f;

    sf::Time currentTime = clock.getElapsedTime();
}

//Run the app until the window is closed
void DemoApp::run()
{
    //While the window is open
    while (window->isOpen())
    {
        //Handle Input
        handleUpdate();

        //Update Variables
        float prevTime = currentTime.asMilliseconds();
        currentTime = clock.getElapsedTime();
        update(currentTime.asMilliseconds() - prevTime);

        //Render to Screen
        render();
    }
}

//Handle the user's inputs
void DemoApp::handleUpdate()
{
    //Create an event to determine whether the user has done somthing noteworthy
    //It will be used as a container to run through every event the user has triggered
    sf::Event event;
    while (window->pollEvent(event))
    {
        //The user has closed the app
        if (event.type == sf::Event::Closed)
            window->close();

        //The user has resized the window
        if (event.type == sf::Event::Resized)
        {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window->setView(sf::View(visibleArea));
        }

        //The user has pressed a key
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::A)
                scale *= 2.f;

            if (event.key.code == sf::Keyboard::S)
                scale *= 0.5f;
        }
    }

    //Check for persistent inputs
    //For when checking if a key is currently pressed instead of if it has just been pressed or released
}

//Update the application
void DemoApp::update(float dt)
{
    float timeElapsed = clock.getElapsedTime().asMilliseconds();

    float x = scale + 5.f + scale * cos(timeElapsed * speed);
    float y = scale + 5.f + scale * sin(2 * timeElapsed * speed);

    rect.setSize(sf::Vector2f(x, y));
    rect.setPosition(sf::Vector2f(window->getSize().x / 2 - rect.getSize().x / 2, window->getSize().y / 2 - rect.getSize().y / 2));
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    //Render anything needed
    window->draw(rect);

    //Send the drawn objects to the screen
    window->display();
}