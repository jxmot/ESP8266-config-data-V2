# ESP8266-config-data-V2

* [Overview](#overview)
    * [History](#history)
* [Requirements](#requirements)
    * [Flash Memory Tool](#flash-memory-tool)
        * [Installation](#installation)
    * [ArduinoJSON](#arduinojson)
* [Running the Application](#running-the-application)
    * [Next Steps](#next-steps)
* [Recommended Reading](#recommended-reading)

# Overview

This repository contains code intended for the ESP8266. One of its purposes is to demonstrate how SPIFFS might be used for containing an application's configuration data. And it is based on my [ESP8266-config-data](#https://github.com/jxmot/ESP8266-config-data) repository, but with many extensive improvements and modifications.

The other purpose of this repository is to provide a starting point for future projects that require the ability to obtain external configuration data.

## History

I had continued some work using my [ESP8266-config-data](#https://github.com/jxmot/ESP8266-config-data) repository as a starting point. And my new design had the following requriements - 

* Configuration data is kept in files and formatted as JSON.
* Automatically connect to a WiFi access point that -
    * Would be configured in `/data/wificfg.dat`.
    * Has multiple access point choices that would be provided in the configuration file.
* The potential need for some additional configuration files - 
    * Application Configuration : Application name, and a debug output mute flag. 
    * Remote Server Configuration : remote server addresses and port numbers to be selectable via a textual name or other identification.
    * *TBD*

# Requirements

You will need the following - 

* ESP8266 - I'm using a NodeMCU 1.0, and mine is a 12-E
* Arduino IDE - Installed and made ready for use with the ESP8266
    * In addition to being able to compile code for the ESP8266 you **will** need an IDE plug-in for downloading the file(s) to the ESP8266 *flash memory*.
* The [ArduinoJSON](#http://arduinojson.org/) library is used in this application. 
    
## Flash Memory Tool

I dowloaded the most *recent* version of the plugin by going to <https://github.com/esp8266/arduino-esp8266fs-plugin> and clicking the "releases page" link in the README.

**NOTE :** I am running *Windows 10*. And this file, its instructions and any other information may or may not be influenced by that fact. Where possible I will note any windows specific items.

### Installation

There are some discrepancies in the available on-line sources in regards to *where* to place the plug-in's `jar` file. In *Windows* I've found that the plugin will work as long as it's in one of two places. They are - 

* `C:\Users\SOME_USER\Documents\Arduino` - you might have to create a `tools` folder here, and then copy the contents of the plugin zip file into it.

**OR**

* `C:\Program Files (x86)\Arduino\tools` - copy the contents of the plug-in zip file into this folder.

I've tested the plug-in separately in each of the locations and have had no issues.

## ArduinoJSON

The ArduinoJSON library should be installed via the *library manager* in the Arduino IDE. This application was built and tested with **ArduinoJSON v5.11.0**.

# Running the Application

Start with an unmodified version of the code from this repository. Please use the following steps - 

**1**) Run the *Arduino IDE* and go to the *Tools* menu and find **`Flash Size:`** and choose **'4M (1M SPIFFS)'**.

**2**) Open the **ino** file in the *Arduino IDE* and build & upload it to the ESP8266. **Do not** open the serial monitor.

**3**) Then go to the *Tools* menu and click on **`ESP8266 Sketch Data Upload`**.

Your SPIFFS along with the file it contains will be uploaded to the ESP8266. After that has completed open the *serial monitor* and reset the ESP8266. 

You should see this in the serial monitor - 

```
setup START

AppCfgData parsing JSON - 
{"appname":"My Application","debugmute":false}

getAppName - My Application
getDebugMute - OFF


WifiCfgData parsing JSON - 
{ "apoints":[
{"wifi-ssid":"your ssid-1 here","wifi-pass":"your wifi password-1 here"},
{"wifi-ssid":"your ssid-2 here","wifi-pass":"your wifi password-2 here"}
],"apcount":2}

getAPCount - 2

AP #0
getSSID - your ssid-1 here
getPASS - your wifi password-1 here

AP #1
getSSID - your ssid-2 here
getPASS - your wifi password-2 here

Attempting connection to - 
ssid : your ssid-1 here
pass : your wifi password-1 here
connected? - no

Attempting connection to - 
ssid : your ssid-2 here
pass : your wifi password-2 here
connected? - no

setupWiFi ERROR! - Could not connect to - 
    your ssid-1 here
    your ssid-2 here

setup DONE - Could not connect to - 
    your ssid-1 here
    your ssid-2 here

```

Since all attempts failed the on-board LED will flash with an interval of `ERR_TOGGLE_INTERVAL` (*defined in ESP8266-config-data-V2.ino*).

## Next Steps

Let's edit the configuration data and change at least one of the SSID and password combinations to something *usable*.

**1**) Edit the `wificfg.dat` file, it's found in `/data`

**2**) The contents currently look like this - 

```
{ "apoints":[
{"wifi-ssid":"your ssid-1 here","wifi-pass":"your wifi password-1 here"},
{"wifi-ssid":"your ssid-2 here","wifi-pass":"your wifi password-2 here"}
],"apcount":2}
```

**3**) If you're only going to have one access point in the configuration file, then it should look like this - 

```
{ "apoints":[
{"wifi-ssid":"your ssid-1 here","wifi-pass":"your wifi password-1 here"}
],"apcount":1}
```

**4**) Save and close the file as **`_wificfg.dat`** (*note the underscore at the beginning of the file name, since this file now contains sensitive information it will be ignored via* `.gitignore` *and cannot be committed*)

**5**) Run the *Arduino IDE* and go to the *Tools* menu and find **`Flash Size:`** and choose **'4M (1M SPIFFS)'**.

**6**) Open the **ino** file in the *Arduino IDE* and edit line 29 to read as `if(setupWiFi("/_wificfg.dat"))`, then build & upload it to the ESP8266. **Do not** open the serial monitor.

**7**) Then go to the *Tools* menu and click on **`ESP8266 Sketch Data Upload`**.

After the SPIFFS has uploaded you can open the *serial monitor* and reset the ESP8266. You should now see something *like this* - 

```
setup START

AppCfgData parsing JSON - 
{"appname":"My Application","debugmute":false}

getAppName - My Application
getDebugMute - OFF


WifiCfgData parsing JSON - 
{ "apoints":[
{"wifi-ssid":"usable_SSID_1","wifi-pass":"wifipassw1"},
{"wifi-ssid":"usable_SSID_2","wifi-pass":"wifipassw2"}
],"apcount":2}

getAPCount - 2

AP #0
getSSID - usable_SSID_1
getPASS - wifipassw1

AP #1
getSSID - usable_SSID_2
getPASS - wifipassw2

Attempting connection to - 
ssid : usable_SSID_1
pass : wifipassw1
connected? - no

Attempting connection to - 
ssid : usable_SSID_2
pass : wifipassw2
connected? - yes

ip : 192.168.0.2
mac: 60:01:00:00:00:02
Connection Attempt Stats : 
attempts = 1
time     = 4
rssi     = -45 dBm

SrvCfgData parsing JSON - 
{
    "http":{"addr":"192.168.0.26","port":80},
    "devapi":{"addr":"192.168.0.26","port":4843},
    "websock":{"addr":"192.168.0.26","port":8080}
}

srvLabel = http
srvAddr  = 192.168.0.26
srvPort  = 80

srvLabel = devapi
srvAddr  = 192.168.0.26
srvPort  = 4843

srvLabel = websock
srvAddr  = 192.168.0.26
srvPort  = 8080


setup DONE - No Errors
```

Where `usable_SSID_1`, `usable_SSID_2` and `wifipassw1`, `wifipassw2` are the values you edited in the **`/data/_wificfg.dat`** file.

When the connection is successful the on-board LED will flash with an interval of `TOGGLE_INTERVAL` (*defined in ESP8266-config-data-V2.ino*). However if an error has occurred the LED will flash with an interval of `ERR_TOGGLE_INTERVAL`.

# Recommended Reading

To fully understand **ArduinoJson** and how to properly determine the appropriate size of the buffer needed for your JSON data I recommend that you read the following - 

* **ESP8266 SPIFFS - Flash File System** documentation, read this first - <http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html>
* **ArduinoJson** documentation, start here - <https://bblanchon.github.io/ArduinoJson/>
* **ArduinoJson Avoiding Pitfalls**, this provides a very good explanation of common pitfalls as related to the ArduinoJson memory model - <https://bblanchon.github.io/ArduinoJson/doc/pitfalls/>
* **ArduinoJson Memory Model**, this explains how the memory model is allocated and information regarding buffer sizes and methods of allocation - <https://bblanchon.github.io/ArduinoJson/doc/memory/>
* **ArduinoJson Assistant** is a tool to aid in determining the correct buffer size based on a sample of the JSON you are working with - <https://bblanchon.github.io/ArduinoJson/assistant/>

