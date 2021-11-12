/*
***************************************************************************  
**  Program  : networkStuff.h, part of DSMRloggerAPI
**  Version  : v4.0.0
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#include <WiFi.h>        // Core WiFi Library         
#include <ESPmDNS.h>        // part of Core https://github.com/esp8266/Arduino

#include <WiFiUdp.h>            // part of ESP8266 Core https://github.com/esp8266/Arduino
#ifdef USE_UPDATE_SERVER
//  #include "ModUpdateServer.h"  // https://github.com/mrWheel/ModUpdateServer
  #include <Update.h>
  #include "UpdateServerHtml.h"
#endif
//#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager
#include <WiFiManager.h>        // version 0.16.0 - https://github.com/tzapu/WiFiManager

WebServer        httpServer (80);

bool        FSmounted = false; 
bool        isConnected = false;

void LogFile(const char*, bool);
void P1Reboot();

//gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (WiFiManager *myWiFiManager) 
{
  DebugTln(F("Entered config mode\r"));
  DebugTln(WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  DebugTln(myWiFiManager->getConfigPortalSSID());
} // configModeCallback()


//===========================================================================================
void startWiFi(const char* hostname, int timeOut) 
{
  WiFiManager manageWiFi;
//  ESP_WiFiManager manageWiFi("p1-dongle");
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress();

//  DebugTln("start ...");
  LogFile("Wifi Starting",true);

  manageWiFi.setDebugOutput(false);
  
  //--- set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //--- sets timeout until configuration portal gets turned off
  //--- useful to make it all retry or go to sleep in seconds
  //manageWiFi.setTimeout(240);  // 4 minuten
  manageWiFi.setTimeout(timeOut);  // in seconden ...

  //--- fetches ssid and pass and tries to connect
  //--- if it does not connect it starts an access point with the specified name
  //--- here  "DSMR-WS-<MAC>"
  //--- and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect("P1-Dongle"))
  {
    LogFile("Wifi failed to connect and hit timeout",true);
//    DebugTln(F("Wifi failed to connect and hit timeout"));
    DebugTf(" took [%d] seconds ==> ERROR!\r\n", (millis() - lTime) / 1000);
    P1Reboot();
    return;
  }
  LogFile("Wifi Connected",true);
//  DebugTf("Connected with IP-address [%s]\n", WiFi.localIP().toString().c_str());
  DebugTf(" took [%d] seconds => OK!\n", (millis() - lTime) / 1000);
//  Debug (F("\nConnected to " )); Debugln (WiFi.SSID());
//  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
//  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
//  Debugln();
  
} // startWiFi()

//===========================================================================================
void startTelnet() 
{
  TelnetStream.begin();
  DebugTln(F("Telnet server started .."));
  TelnetStream.flush();
} // startTelnet()


//=======================================================================
void startMDNS(const char *Hostname) 
{
  DebugTf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", Hostname);    
  } 
  else 
  {
    DebugTln(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);
  
} // startMDNS()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
