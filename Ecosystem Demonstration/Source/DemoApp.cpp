#include "DemoApp.h"
#include <random>

#include "imgui.h"
#include "imgui-SFML.h"

DemoApp::DemoApp() :
    window(NULL)
{
    srand(time(0));
}

void DemoApp::init()
{
    //Initialise window and starting variables
    window = new sf::RenderWindow(sf::VideoMode(900, 700), "Ecosystem Resilience Demonstration");

    ImGui::SFML::Init(*window);

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
        update(clock.restart());

        //Render to Screen
        render();
    }
}

//Handle the user's inputs
void DemoApp::handleInput()
{
    //Create an event to determine whether the user has done something noteworthy
    //It will be used as a container to run through every event the user has triggered
    sf::Event event;
    while (window->pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(*window, event);

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

            if (event.key.code == sf::Keyboard::W)
            {
                cont = true;
                paused = false;
            }
        }
    }

    //Check for persistent inputs
    //For when checking if a key is currently pressed instead of if it has just been pressed or released
}

//Update the application
void DemoApp::update(sf::Time dt)
{
    ecology.Update(dt.asSeconds());

    ImGui::SFML::Update(*window, dt);
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    sf::RectangleShape cellRep;

    int sideSize = std::min(window->getSize().x * 0.5f, window->getSize().y * 0.5f) / DEFAULT_SIDE;

    cellRep.setSize(sf::Vector2f(sideSize, sideSize));

    //Render anything needed
    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize) - 225;
        pos.y = cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize) - 175;
        cellRep.setPosition(window->getView().getCenter() + pos);

        sf::Color repColor(0, 0, 0);

        repColor = sf::Color(86, 125 + ecology.world[cellNum]->plants.size() * 5, 70);

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize) + 225;
        pos.y = cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize) - 175;
        cellRep.setPosition(window->getView().getCenter() + pos);

        sf::Color repColor(0, 0, 0);

        float mass = 0;
        for (auto plant : ecology.world[cellNum]->plants)
            mass += plant.mass;

        repColor = sf::Color(86, 125 + mass * 60.f / DEFAULT_MASS, 70);

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize) - 225;
        pos.y = cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize) + 175;
        cellRep.setPosition(window->getView().getCenter() + pos);

        sf::Color repColor(0, 0, 0);

        int waterNum = float(ecology.world[cellNum]->GetWater() * 255.f);

        repColor = sf::Color(0, 0, waterNum);

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;
        pos.x = cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize) + 225;
        pos.y = cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize) + 175;
        cellRep.setPosition(window->getView().getCenter() + pos);

        sf::Color repColor(0, 0, 0);

        repColor = sf::Color(ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 15, 0, 0);

        cellRep.setFillColor(repColor);

        window->draw(cellRep);
    }

    float water = 0;
    for (auto cell : ecology.world)
    {
        water += cell->GetWater();
    }

    int numOfPlants = 0;
    for(auto cell : ecology.world)
    {
        numOfPlants += cell->plants.size();
    }

    ImGui::Begin("Diagnostics");
    ImGui::Text("Water Level:\t%f", water);
    ImGui::Text("Plant Population:\t%d", numOfPlants);
    ImGui::End();

    ImGui::SFML::Render(*window);
    //Send the drawn objects to the screen
    window->display();
}