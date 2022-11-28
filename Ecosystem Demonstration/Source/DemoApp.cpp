#include "DemoApp.h"

DemoApp::DemoApp() :
    window(NULL),
    width(DEFAULT_SIDE),
    height(DEFAULT_SIDE),
    world(DEFAULT_SIDE, DEFAULT_SIDE)
{}

void DemoApp::init()
{
    //Initialise window and starting variables
    window = new sf::RenderWindow(sf::VideoMode(1600, 900), "Ecosystem Resilience Demonstration");

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
            
        }
    }

    //Check for persistent inputs
    //For when checking if a key is currently pressed instead of if it has just been pressed or released
}

//Update the application
void DemoApp::update(float dt)
{
    float timeElapsed = clock.getElapsedTime().asMilliseconds();

    world.Update();
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    sf::RectangleShape cellRep;
    cellRep.setSize(sf::Vector2f(20, 20));

    //Render anything needed
    for (int cellNum = 0; cellNum < world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / world.GetHeight() * 20 - (width / 2 * 20);
        pos.y = cellNum % world.GetWidth() * 20 - (height / 2 * 20);
        cellRep.setPosition(window->getView().getCenter() + pos);

        int waterNum = float(world[cellNum]->GetWater() * 255.f) / 1;

        cellRep.setFillColor(sf::Color(0, 0, waterNum, 255));

        window->draw(cellRep);
    }

    //Send the drawn objects to the screen
    window->display();
}