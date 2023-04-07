#include "DemoApp.h"
#include <random>

#include "imgui.h"
#include "imgui-SFML.h"

DemoApp::DemoApp() :
    window(NULL),
    stepCount(0),
    perlin(false),
    viewSeparate(false)
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
    //System events are run using ImGui
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
    }
}

//Update the application
void DemoApp::update(sf::Time dt)
{
    //ecology.Update(dt.asSeconds());
    stepCount++;

    //Construct diagnostic data
    ImGui::SFML::Update(*window, dt);

    float water = 0;
    float plantMass = 0;
    for(auto cell : ecology.world)
    {
        water += cell->GetWater();
        if(cell->plants.size() > 0)
			plantMass += cell->plants[0].mass;
    }

    //Create ImGui window
    ImGui::Begin("Diagnostics");

    //Data
    ImGui::Text("Step Count:\t\t\t%d", stepCount);
    ImGui::Text("Water Level:\t\t%f", water);
    ImGui::Text("Total Plant Mass:\t%f", plantMass);

    //Triggers
    //Generation
    ImGui::Text("\nGeneration");
    ImGui::Checkbox("Use Perlin Noise?", &perlin);
    if (ImGui::Button("Generate", ImVec2(200, 20)))
    {
        if (!perlin)
            ecology.GenerateWorld();
        else
            ecology.GenerateWorld(EcoResilience::GenerationType::PERLIN);
    }

    //Natural events
    ImGui::Text("\nNatural Events");
    if (ImGui::Button("Rain", ImVec2(200, 20)))
        ecology.Rain();

    //Disturbance events
    ImGui::Text("\nDisturbance Events");
    if (ImGui::Button("Fire", ImVec2(200, 20)))
        ecology.Rain();

    //Viewing
    ImGui::Text("\nViewing");
    ImGui::Checkbox("View Separate layers", &viewSeparate);

    ImGui::End();
}

//Render the application to the screen
void DemoApp::render()
{
    //Clear the display
    window->clear();

    sf::RectangleShape cellRep;

    int sideSize;
    if(viewSeparate)
		sideSize = std::min(window->getSize().x * 0.5f, window->getSize().y * 0.5f) / DEFAULT_SIDE;
    else
        sideSize = std::min(window->getSize().x, window->getSize().y) / DEFAULT_SIDE;

    cellRep.setSize(sf::Vector2f(sideSize, sideSize));
    const sf::Vector2f screenCentre = window->getView().getCenter();

    //Render anything needed
    for (int cellNum = 0; cellNum < ecology.world.size(); cellNum++)
    {
        sf::Vector2f pos;

        const sf::Vector2f cellPos(cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize), cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize));
        const sf::Vector2f worldPos(window->getSize().x * 0.25f, window->getSize().y * 0.25f);

        sf::Color repColor(0, 0, 0);

        if (viewSeparate)
        {
            //Plant Mass
            pos.x = cellPos.x + worldPos.x;
            pos.y = cellPos.y - worldPos.y;
            cellRep.setPosition(screenCentre + pos);

            float mass = 0;
            if (!ecology.world[cellNum]->plants.empty())
                mass = ecology.world[cellNum]->plants[0].mass;

            repColor = sf::Color(86, 125 + mass * 60.f / DEFAULT_MASS, 70);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);

            //Water
            pos.x = cellPos.x - worldPos.x;
            pos.y = cellPos.y + worldPos.y;
            cellRep.setPosition(screenCentre + pos);

            int waterNum = float(ecology.world[cellNum]->GetWater() * 255.f);

            repColor = sf::Color(0, 0, waterNum);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);

            //Animals
            pos.x = cellPos.x + worldPos.x;
            pos.y = cellPos.y + worldPos.y;
            cellRep.setPosition(screenCentre + pos);

            repColor = sf::Color(ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 15, 0, 0);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);
        }
        else
        {
            //Plant Mass
            cellRep.setPosition(screenCentre + cellPos);

            float mass = 0;
            if (ecology.world[cellNum]->plants.size() > 0)
                mass = ecology.world[cellNum]->plants[0].mass;

            //Water
            int waterNum = float(ecology.world[cellNum]->GetWater() * 255.f);

            //Animals
            float red = ecology.world[cellNum]->cellType == EcoResilience::CellType::WATER ? 0 : 209 - (209 - 86) * mass / DEFAULT_MASS; //+ ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 15;
            float green = ecology.world[cellNum]->cellType == EcoResilience::CellType::WATER ? 0 : 189 - (189 - 125) * mass / DEFAULT_MASS;
            float blue = ecology.world[cellNum]->cellType == EcoResilience::CellType::WATER ? waterNum : 100 - (100 - 70) * mass / DEFAULT_MASS;
            
            //Render cell
            repColor = sf::Color(red, green, blue);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);
        }
    }

    ImGui::SFML::Render(*window);
    //Send the drawn objects to the screen
    window->display();
}