/* ************************************************************************ */
/*
    connectWiFi.h - Assists in making a connection to an access point. If a 
    problem occurs during the connection attempt this class will retry for 
    no more than MAX_ATTEMPTS times.
*/
#include <ESP8266WiFi.h>

#include "connectWiFi.h"

/*
    Construct the object and connect to the access point. Optionally return
    the current connection information.
*/
ConnectWiFi::ConnectWiFi(const char *ssid, const char *passw, conninfo *info)
{
    // connect to the access point
    if(connectToAP(ssid, passw))
    {
        // success, is caller requesting the connection info?
        if(info != NULL)
        {
            // retrieve the connection info
            GetConnInfo(info);
        }
    }
}

/*
    Retrieve the current connection information
*/
bool ConnectWiFi::GetConnInfo(conninfo *info)
{
bool bRet = false;

    // check the arg, and if we're connected...
    if((info != NULL) && IsConnected())
    {
        // copy the connection info to the caller's
        // provided memory
        info->ssidString    = currwifi.ssidString;
        info->passString    = currwifi.passString;
        info->ipAddrString  = currwifi.ipAddrString;
        info->macAddrString = currwifi.macAddrString;
        for(int ix = 0; ix < MAC_SIZE; ix++) info->mac[ix] = currwifi.mac[ix];
        info->timeToConnect = currwifi.timeToConnect;
        info->attempts      = currwifi.attempts;
        info->isConnected   = IsConnected();
        info->rssi          = currwifi.rssi;
        bRet = true;
    }
    return(bRet);
}

/*
    Check and see if we're connected
*/
bool ConnectWiFi::IsConnected()
{
    currwifi.isConnected = (WiFi.status() == WL_CONNECTED) ? true : false;
    return(currwifi.isConnected);
}

/*
    Connect to the access point using the SSID and password
*/
bool ConnectWiFi::connectToAP(const char *ssid, const char *passw) 
{
char macStr[MACSTR_SIZE];

    // initialize/clear the connection info object
    initCurrWiFi(ssid, passw);

    // clear out the MAC
    for(int ix = 0; ix < MACSTR_SIZE; ix++) macStr[ix] = 0;

    // Keep trying to connect until either we're successful or
    // we've run out of attempts
    while(true) 
    {
        // get ready to connect...
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passw);

        // only wait for the connection to occur for MAX_WAITCONNECTED
        // times with a delay of WAITCONNECTED_DELAY before each check
        // of our connection status...
        for(int x = 0; x < MAX_WAITCONNECTED; x++) 
        {
            delay(WAITCONNECTED_DELAY);
            if(WiFi.status() == WL_CONNECTED) 
            {
                // connected, gather connection info and return
                // success (true)
                currwifi.ipAddrString  = WiFi.localIP().toString();
                currwifi.timeToConnect = x;
                WiFi.macAddress(currwifi.mac);
                sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
                        currwifi.mac[0], currwifi.mac[1], currwifi.mac[2], 
                        currwifi.mac[3], currwifi.mac[4], currwifi.mac[5]);
                currwifi.macAddrString = String(macStr);
                currwifi.isConnected   = true;
                currwifi.rssi          = WiFi.RSSI();
                return(true);
            }
        }
        // only attempt to make the connection MAX_ATTEMPTS times with a
        // delay of ATTEMPT_DELAY between each connection attempt
        if(currwifi.attempts < MAX_ATTEMPTS) 
        {
            currwifi.attempts += 1;
            delay(ATTEMPT_DELAY);
        } else {
            // failed to connect
            return(false);
        }
    }
}

/*
    Initialize/clear the connection info object
*/
void ConnectWiFi::initCurrWiFi(const char *ssid, const char *passw) 
{
    currwifi.ssidString    = String(ssid);
    currwifi.passString    = String(passw);
    currwifi.ipAddrString  = "";
    currwifi.macAddrString = "";
    for(int ix = 0; ix < MAC_SIZE; ix++) currwifi.mac[ix] = 0;
    currwifi.timeToConnect = 0;
    currwifi.attempts      = 1;
    currwifi.isConnected   = false;
    currwifi.rssi          = 0;
}
