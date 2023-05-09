#include "DemoApp.h"

//This is a demonstrative application developed by Cameron Ayling in order to showcase the EcoResilience Library
//The EcoResilience library is a data-only library that generates and simulates ecosystems that can respond to
//external disturbance events and can recover from them.

int main()
{
    //Create the application
    DemoApp app;
    app.init();

    //Set the application running
    app.run();

    //End the application's running
    return 0;
}