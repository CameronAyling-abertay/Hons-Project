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

    if (!font.loadFromFile("media/arial.ttf"))
    {
        exit(0);
    }

    waterDiagnostic.setFont(font);
    waterDiagnostic.setFillColor(sf::Color::White);
    waterDiagnostic.setPosition(50, 50);
    waterDiagnostic.setCharacterSize(24);
    waterDiagnostic.setStyle(sf::Text::Regular);
    plantDiagnostic.setFont(font);
    plantDiagnostic.setFillColor(sf::Color::White);
    plantDiagnostic.setPosition(50, 100);
    plantDiagnostic.setCharacterSize(24);
    plantDiagnostic.setStyle(sf::Text::Regular);
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
        float prevTime = currentTime.asSeconds();
        currentTime = clock.getElapsedTime();
        update(currentTime.asSeconds() - prevTime);

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

            if (event.key.code == sf::Keyboard::R)
                ecology.Rain();
        }
    }

    //Check for persistent inputs
    //For when checking if a key is currently pressed instead of if it has just been pressed or released
}

//Update the application
void DemoApp::update(float dt)
{
    ecology.Update(dt);
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    sf::RectangleShape cellRep;

    int sideSize = std::min(window->getSize().x, window->getSize().y) / DEFAULT_SIDE;

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

        if (ecology.world[cellNum]->cellType == EcoResilience::CellType::WATER)
            repColor.b = waterNum;
        else
        {
            float mass = 0;
            for (auto plant : ecology.world[cellNum]->plants)
                mass += plant.mass;

            repColor = sf::Color(86, 125 + int(mass * 60.f / DEFAULT_MASS), 70);
            //repColor.r += ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 15;
        }

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    float water = 0;
    for (auto cell : ecology.world)
    {
        water += cell->GetWater();
    }

    waterDiagnostic.setString("Global Water: " + std::to_string(water));
    window->draw(waterDiagnostic);

    int numOfPlants = 0;
    for(auto cell : ecology.world)
    {
        numOfPlants += cell->plants.size();
    }

    plantDiagnostic.setString("Global Plants: " + std::to_string(numOfPlants));
    window->draw(plantDiagnostic);

    //Send the drawn objects to the screen
    window->display();
}