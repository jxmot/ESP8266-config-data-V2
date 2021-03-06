/* ************************************************************************ */
/*
    esp8266-ino.cpp - support functions for the associated ino file.
*/
#include "AppCfgData.h"
#include "WifiCfgData.h"
#include "SrvCfgData.h"
#include "connectWiFi.h"

// if the application configuration is present, and if the debug mute flag is 
// true then mute debug output
#define DEBUG_MUTE ((a_cfgdat != NULL) && a_cfgdat->getDebugMute() ? true : false)

// default serial baud rate, modify as needed
const int DEFAULT_SERIAL_BAUD = 115200;

// functions that are only called from within this module
void printAppCfg();
void printWiFiCfg();
bool connectWiFi(String ssid, String pass);
void printSrvCfg();

// pointers to configuration data objects
AppCfgData *a_cfgdat = NULL;
WifiCfgData *w_cfgdat = NULL;
SrvCfgData *s_cfgdat = NULL;

// error message string
String errMsg;

// pointer to the WiFi connection object
ConnectWiFi *connWiFi = NULL;
//////////////////////////////////////////////////////////////////////////////
/*
    Print a start up message to the serial port
*/
void setupStart()
{
    Serial.begin(DEFAULT_SERIAL_BAUD);
    Serial.println();
    Serial.println();
    Serial.println("setup START");
    Serial.flush();
}

/*
    Print a start up done message to the serial port
*/
void setupDone()
{
    Serial.flush();
    Serial.println();
    Serial.println("setup DONE - " + (errMsg == "" ? "No Errors" : errMsg));
    Serial.println();
    Serial.flush();

    // let's blink the LED from within loop()
    pinMode(LED_BUILTIN, OUTPUT);
}

/*
    Toggle the on-board LED
*/
void toggleLED()
{
static bool ledTogg = false;

    if(ledTogg) digitalWrite(LED_BUILTIN, LOW);
    else digitalWrite(LED_BUILTIN, HIGH);

    ledTogg = !ledTogg;
}

/*
    Print an error message, and change the baud rate if
    necessary.
*/
void printError(String func, String _errMsg)
{
    if(Serial.baudRate() != DEFAULT_SERIAL_BAUD)
    {
        Serial.end();
        Serial.begin(DEFAULT_SERIAL_BAUD);
    }
    Serial.println(func + " ERROR! - " + _errMsg);
}

/*
    Read and parse the application configuration settings, 
    and report any errors.
*/
bool setupApp(const String appCfgFile)
{
String func = String(__func__);
bool bRet = false;

    a_cfgdat = new AppCfgData((const char *)appCfgFile.c_str());
    if(!a_cfgdat->getError(errMsg)) 
    {
        a_cfgdat->parseFile();

        if(a_cfgdat->getError(errMsg)) printError(func, errMsg);
        else {
            printAppCfg();
            bRet = true;
        }
    } else printError(func, errMsg);
    return bRet;
}

/*
    Print(to the serial port) the application setting values
*/
void printAppCfg()
{
    if(a_cfgdat != NULL)
    {
        Serial.println("getAppName - " + a_cfgdat->getAppName());
        Serial.println("getDebugMute - " + String(a_cfgdat->getDebugMute() ? "ON" : "OFF"));
        Serial.println();
    }
}

/*
    Read and parse the WiFi configuration settings, and make a
    connection to one of the configured access points. If successful
    then return true, otherwise return false.
*/
bool setupWiFi(const String wifiCfgFile)
{
String func = String(__func__);
bool isconnected = false;

    w_cfgdat = new WifiCfgData((const char *)wifiCfgFile.c_str(), DEBUG_MUTE);

    if(!w_cfgdat->getError(errMsg)) 
    {
        w_cfgdat->parseFile();
        if(w_cfgdat->getError(errMsg)) printError(func, errMsg);
        else {
            printWiFiCfg();
            int ix = 0;
            for(ix = 0; ix < w_cfgdat->getAPCount() && isconnected == false; ix++)
            {
                isconnected = connectWiFi(w_cfgdat->getSSIDString(ix), w_cfgdat->getPASSString(ix));
            }
            if(!isconnected) 
            {
                errMsg = "Could not connect to - ";
                for(ix = 0; ix < w_cfgdat->getAPCount(); ix++) errMsg += String("\n    ") + w_cfgdat->getSSIDString(ix);
                printError(func, errMsg);
            } else errMsg = "";
        }
    } else printError(func, errMsg);
    return isconnected;
}

/*
    Print the WiFi configuration values.
*/
void printWiFiCfg()
{
    if((a_cfgdat != NULL) && !a_cfgdat->getDebugMute())
    {
        Serial.println("getAPCount - " + String(w_cfgdat->getAPCount()));
        Serial.println();

        for(int ix = 0; ix < w_cfgdat->getAPCount(); ix++)
        {
            Serial.println("AP #" + String(ix));
            Serial.println("getSSID - " + w_cfgdat->getSSIDString(ix));
            Serial.println("getPASS - " + w_cfgdat->getPASSString(ix));
            Serial.println();
        }
    }
}

/*
    Get the server address and port configuration values. These
    are typically servers that this application will communicate
    with.
*/
bool setupServers(const String srvCfgFile)
{
String func = String(__func__);
bool bRet = false;

    // NOTE: The total quantity of server configs is limited,
    // see the SrvCfgData class for details.
    s_cfgdat = new SrvCfgData((const char *)srvCfgFile.c_str(), DEBUG_MUTE);

    if(!s_cfgdat->getError(errMsg)) 
    {
        s_cfgdat->parseFile();
        if(s_cfgdat->getError(errMsg)) printError(func, errMsg);
        else {
            printSrvCfg();
            bRet = true;
        }
    } else printError(func, errMsg);
    return bRet;
}

/*
    Print the server configuration values
*/
void printSrvCfg()
{
srvcfg cfg;
// NOTE: These strings are used in the configuration file
// to separate the various servers that the application 
// might use. Retrieving the information using a string
// eliminates the need to keep indices in sync between the
// application and the config data.
const String labels[] = {"http","devapi","websock","END"};

    if((a_cfgdat != NULL) && !a_cfgdat->getDebugMute())
    {
        for(int ix = 0; labels[ix] != "END"; ix++)
        {
            if(s_cfgdat->getServerCfg(labels[ix], cfg))
            {
                Serial.println("label = " + cfg.label);
                Serial.println("addr  = " + cfg.addr);
                Serial.println("port  = " + String(cfg.port));
                Serial.println();
            } else Serial.println("not found in cfg - " + labels[ix]);
        }
    }
}

/*
    Connect to an access point with its SSID and password,
    this function will retry for 'N' times if unsuccessful.
    Returns true if connected, otherwise returns false.
*/
bool connectWiFi(String ssid, String pass)
{
conninfo conn;

    if((a_cfgdat != NULL) && !a_cfgdat->getDebugMute())
    {
        Serial.println("Attempting connection to - ");
        Serial.println("ssid : " + ssid);
        Serial.println("pass : " + pass);
    }

    connWiFi = new ConnectWiFi(ssid.c_str(), pass.c_str());

    if((a_cfgdat != NULL) && !a_cfgdat->getDebugMute())
    {
        Serial.println("connected? - " + String(connWiFi->IsConnected()? "yes" : "no"));
        Serial.println();

        if(connWiFi->GetConnInfo(&conn)) 
        {
            Serial.println("ip  : " + conn.localIP);
            Serial.println("mac : " + conn.macAddress);
            Serial.println("host: " + conn.hostname);
            Serial.println("Connection Attempt Stats : ");
            Serial.println("attempts = " + String(conn.attempts));
            Serial.println("time     = " + String(conn.timeToConnect));
            Serial.println("rssi     = " + String(conn.rssi) + " dBm");
        }
    }
    return connWiFi->IsConnected();
}
