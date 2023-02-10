#include "DemoApp.h"
#include <random>

DemoApp::DemoApp() :
    window(NULL)
{
    srand(time(0));
}

void DemoApp::init()
{
    //Initialise window and starting variables
    window = new sf::RenderWindow(sf::VideoMode(1600, 900), "Ecosystem Resilience Demonstration");

    sf::Time currentTime = clock.getElapsedTime();

    ecology.GenerateWorld();
}

//Run the app until the window is closed
void DemoApp::run()
{
    //While the window is open
    while (window->isOpen())
    {
        //Handle Input
        handleInput();

        //Update Variables
        float prevTime = currentTime.asMilliseconds();
        currentTime = clock.getElapsedTime();
        update(currentTime.asMilliseconds() - prevTime);

        //Render to Screen
        render();
    }
}

//Handle the user's inputs
void DemoApp::handleInput()
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
            if(event.key.code == sf::Keyboard::A)
                ecology.GenerateWorld();

            if (event.key.code == sf::Keyboard::S)
                ecology.GenerateWorld(EcoResilience::GenerationType::PERLIN);
        }
    }

    //Check for persistent inputs
    //For when checking if a key is currently pressed instead of if it has just been pressed or released
}

//Update the application
void DemoApp::update(float dt)
{
    float timeElapsed = clock.getElapsedTime().asMilliseconds();
    
    ecology.Update(dt);
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    sf::RectangleShape cellRep;

    int sideSize = 800 / DEFAULT_SIDE;

    cellRep.setSize(sf::Vector2f(sideSize, sideSize));

    //Render anything needed
    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize);
        pos.y = cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize);
        cellRep.setPosition(window->getView().getCenter() + pos);

        sf::Color repColor(0, 0, 0);

        int waterNum = float(ecology.world[cellNum]->GetWater() * 255.f);

        if (waterNum > 150)
            repColor.b = waterNum;
        else
        {
            repColor = sf::Color(86, 125 + ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PLANT) * 5, 70);
            repColor.r += ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 15;
        }

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    //Send the drawn objects to the screen
    window->display();
}