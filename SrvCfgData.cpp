/* ************************************************************************ */
/*
    SrvCfgData.cpp - Intended to provide external server configuration data,
    such as - address and port. 

    NOTE: If the server is providing some type of service that requries 
    an API key or other service specific data then it would also be kept 
    here. Or if too complex for a shared config file then those service
    specific configs could end up in their own file & object.
*/
#include "SrvCfgData.h"
#include <ArduinoJson.h>

// these are known, but not referenced in the client application
String SrvCfgData::labels[] = {"http","devapi","websock","END"};

//////////////////////////////////////////////////////////////////////////////
/*
*/
SrvCfgData::SrvCfgData(const char *cfgfile, bool muted): ConfigData(cfgfile)
{
    muteDebug = muted;

    for(int ix = 0; ix < MAX_SRVCFG; ix++)
    {
        configs[ix] = NULL;
    } 
}

//////////////////////////////////////////////////////////////////////////////
/*
*/
void SrvCfgData::parseJSON(std::unique_ptr<char[]>& buf)
{
    if(!muteDebug)
    {
        Serial.println();
        Serial.println("SrvCfgData parsing JSON - ");
        Serial.println(buf.get());
    }
 
    // https://bblanchon.github.io/ArduinoJson/assistant/
    const size_t bufferSize = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 120;
    StaticJsonBuffer<bufferSize> jsonBuffer;

    JsonObject& json = jsonBuffer.parseObject(buf.get());

    /*
        This is one of the places where you would customize this class to be 
        used with your application specific configuration data.
        
        Another place is in SrvCfgData.h
    */
    for(int ix = 0; labels[ix] != "END"; ix++)
    {
        configs[ix] = new srvcfg();
        configs[ix]->srvLabel = labels[ix];
        configs[ix]->srvAddr  = String((const char *)json[(char *)labels[ix].c_str()]["addr"]);
        configs[ix]->srvPort  = json[(char *)labels[ix].c_str()]["port"];
    }
}

//////////////////////////////////////////////////////////////////////////////
/*
    This is one of the places where you would customize this class to be 
    used with your application specific configuration data.
    
    Another place is in SrvCfgData.h
*/
bool SrvCfgData::getServerCfg(String label, srvcfg &cfgout)
{
bool bRet = false;

    for(int ix = 0; (ix < MAX_SRVCFG) && !bRet; ix++)
    {
        if(configs[ix]->srvLabel == label)
        {
            cfgout.srvLabel = configs[ix]->srvLabel;
            cfgout.srvAddr  = configs[ix]->srvAddr;
            cfgout.srvPort  = configs[ix]->srvPort;
            bRet = true;
        }
    }
    return bRet;
}


