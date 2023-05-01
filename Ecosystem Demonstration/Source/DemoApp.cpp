#include "DemoApp.h"
#include <random>

#include "imgui.h"
#include "imgui-SFML.h"

DemoApp::DemoApp() :
    window(NULL),
    stepCount(0),
    perlin(false),
    viewSeparate(false),
	paused(false)
{
    srand(time(0));
}

void DemoApp::init()
{
    //Initialise window and starting variables
    window = new sf::RenderWindow(sf::VideoMode(1200, 800), "Ecosystem Resilience Demonstration");

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
    if (!paused)
    {
        ecology.Update(dt.asSeconds());
        stepCount++;
    }

    //Construct diagnostic data
    ImGui::SFML::Update(*window, dt);

    float water = 0;
    float plantMass = 0;
    int prey = 0;
    int predator = 0;
    for(auto cell : ecology.world)
    {
        water += cell->GetWater();
        if(cell->plants)
			plantMass += cell->plants->mass;

        prey += cell->GetPopulation(EcoResilience::PopulationType::PREY);
        predator += cell->GetPopulation(EcoResilience::PopulationType::PREDATOR);
    }

    //Create ImGui window
    ImGui::Begin("Diagnostics");

    //Data
    ImGui::Text("Step Count:\t\t\t %d", stepCount);
    ImGui::Text("Water Level:\t\t    %f", water);
    ImGui::Text("Total Plant Mass:\t   %f", plantMass);
    ImGui::Text("Prey Population:\t    %d", prey);
    ImGui::Text("Predator Population:\t%d", predator);

    //Triggers
    //Generation
    ImGui::Text("\nGeneration");
    ImGui::Checkbox("Use Perlin Noise?", &perlin);
    if (ImGui::Button("Generate", ImVec2(250, 20)))
    {
        if (!perlin)
            ecology.GenerateWorld();
        else
            ecology.GenerateWorld(EcoResilience::GenerationType::PERLIN);
        stepCount = 0;
    }

    //Natural events
    ImGui::Text("\nNatural Events");
    if (!ecology.drought)
    {
        if (ImGui::Button("Rain", ImVec2(250, 20)))
            ecology.Rain();
    }
    else
        ImGui::Button("Rain (Disabled due to drought)", ImVec2(250, 20));

    //Disturbance events
    ImGui::Text("\nDisturbance Events");
    if (!ecology.drought)
    {
        if (ImGui::Button("Flood", ImVec2(250, 20)))
            ecology.Flood();
    }
    else
        ImGui::Button("Rain (Disabled due to drought)", ImVec2(250, 20));

    if (ImGui::Button("Drought", ImVec2(250, 20)))
        ecology.Drought();

    if (ImGui::Button("Urban Development", ImVec2(250, 20)))
        ecology.Urbanise();
    
    if (ImGui::Button("Plague", ImVec2(250, 20)))
        ecology.Plague();

    if (ImGui::Button("Wildfire", ImVec2(250, 20)))
        ecology.Fire();

    //Viewing
    ImGui::Text("\nViewing");
    ImGui::Checkbox("View Separate layers", &viewSeparate);

    if (paused)
    {
        if (ImGui::Button("Play", ImVec2(250, 20)))
            paused = false;
    }
    else
        if (ImGui::Button("Pause", ImVec2(250, 20)))
            paused = true;

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

        const sf::Vector2f cellPos(cellNum % ecology.world.GetWidth() * sideSize - (ecology.height / 2 * sideSize), cellNum / ecology.world.GetHeight() * sideSize - (ecology.width / 2 * sideSize));
        const sf::Vector2f worldPos(window->getSize().x * 0.25f, window->getSize().y * 0.25f);

        sf::Color repColor(0, 0, 0);

        if (viewSeparate)
        {
            //Disaster Map
            pos.x = cellPos.x - worldPos.x;
            pos.y = cellPos.y - worldPos.y;
            cellRep.setPosition(screenCentre + pos);

            //Plague
            if (ecology.world[cellNum]->animal)
            {
                if (ecology.world[cellNum]->animal->infected)
                {
                    repColor.r = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 128 : 238;
                    repColor.g = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 0 : 130;
                    repColor.b = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 128 : 238;
                }
            }

            //Urban Development
            if (ecology.world[cellNum]->cellType == EcoResilience::CellType::URBANISED)
                repColor = sf::Color(100, 100, 100);

            //Fire
            if (ecology.world[cellNum]->plants)
                if (ecology.world[cellNum]->plants->fire)
                    repColor = sf::Color(204, 85, 0);

            cellRep.setFillColor(repColor);
            window->draw(cellRep);

            //Plant Mass
            pos.x = cellPos.x + worldPos.x;
            pos.y = cellPos.y - worldPos.y;
            cellRep.setPosition(screenCentre + pos);

            float mass = 0;
            if (ecology.world[cellNum]->plants)
                mass = ecology.world[cellNum]->plants->mass;

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

            int red = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) * 255;
            int green = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREDATOR) * 255;
            int blue = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREDATOR) * 255;

            repColor = sf::Color(red, green, blue);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);
        }
        else
        {
            //Plant Mass
            cellRep.setPosition(screenCentre + cellPos);

            float mass = 0;
            if (ecology.world[cellNum]->plants)
                mass = ecology.world[cellNum]->plants->mass;

            sf::Color baseColour(209 - (209 - 86) * mass / DEFAULT_MASS, 189 - (189 - 125) * mass / DEFAULT_MASS, 100 - (100 - 70) * mass / DEFAULT_MASS);

            //Water
            int waterNum = float(ecology.world[cellNum]->GetWater() * 255.f);
            sf::Color waterColour(0, 0, waterNum);

            //Animals
            sf::Color animalColour(
                ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) == 1 ? 255 : 0,
                ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREDATOR) == 1 ? 255 : 0,
                ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREDATOR) == 1 ? 255 : 0
            );

            if (ecology.world[cellNum]->animal)
            {
                if (ecology.world[cellNum]->animal->infected)
                {
                    animalColour.r = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 128 : 238;
                    animalColour.g = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 0 : 130;
                    animalColour.b = ecology.world[cellNum]->GetPopulation(EcoResilience::PopulationType::PREY) ? 128 : 238;
                }
            }

            if (ecology.world[cellNum]->animal)
                baseColour = animalColour;

            //Cell type
            repColor = ecology.world[cellNum]->cellType == EcoResilience::CellType::WATER ? waterColour : baseColour;
            repColor = ecology.world[cellNum]->cellType == EcoResilience::CellType::URBANISED ? sf::Color(100, 100, 100) : repColor;

            if (ecology.world[cellNum]->plants)
                if (ecology.world[cellNum]->plants->fire)
                    repColor = sf::Color(204, 85, 0);

            cellRep.setFillColor(repColor);

            window->draw(cellRep);
        }
    }

    ImGui::SFML::Render(*window);
    //Send the drawn objects to the screen
    window->display();
}