/* ************************************************************************ */
/*
    Skeletal Application for ESP8266 - 

        1) Reads, and parses configuration files formatted as JSON.
        2) Can be configured for multiple access points, will iterate
           through the configured AP list until it finds a successful
           connection.
        3) The configuration files and objects can be modified as 
           needed. Be sure to read the comments and/or the project 
           README for information and precautions.

    There are a total of 3 configuration objects implemented here. The
    intent is to keep them relatively small and easy to manage. The
    amount of memory that they use must be in constant consideration 
    when modifying or adding configuration objects.
*/
#include "esp8266-ino.h"

/*
    Application Set Up
*/
void setup() 
{
    setupStart();

    if(setupApp("/appcfg.dat"))
        if(setupWiFi("/_wificfg.dat"))
            setupServers("/servercfg.dat");

    setupDone();
}

/*
    Application Execution Loop
*/
void loop() 
{
    yield();
    delay(1000);

    toggleLED();
}

