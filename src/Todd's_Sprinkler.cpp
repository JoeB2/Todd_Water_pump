/*
  Water Pump Controller

  Purpose:  Regulate a SSR intended to power a 2HP water pump for lawn care at the Wheatley's Whitman property
  
  What this does: Using 2.4ghz connect to WiFi & allow user to Turn On/Off an SSR relay.
                  Additionally the user can add 15 minutes at a time to an "ON" timer.
                  - When timer counts down to zero the relay is turned off.
                  
  Function: There are three hosted web pages:
            1. Main page http://ip,port showing status (On/Off Remaining On Minutes), Relay ON Relay OFF,
               and Add 15 ON minutes
            3. A WiFi settings page http://ip/wifi where local WiFi information can be Entered or Updated.
            4. An over the air OTA firmware update page http://ip/update

            Schedules are stored in non-volatile memory.  Up to 32 schedules can be created and stored

*/

//#define dbg

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <string>

// OTA
#include <AsyncElegantOTA.h>

// Web Server
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <index.h>

// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>


// bit macros
#define SET_BIT(x, pos) (x |= (1U << pos))
#define CLEAR_BIT(x, pos) (x &= (~(1U<< pos)))
#define CHECK_BIT(x, pos) (x & (1UL << pos) )

#define READINGS_DELAY 30000  //  30 seconds
#define ONE_WIRE_BUS   0      // 4.7k pullup: GPIO0 pullup during boot
#define SSR_PIN        3      // Relay / SSR pin

char AP_SSID[] = "Todd ap:10.0.1.23 ip:"; // max 32 chars

uint32_t startMillis=millis();
uint32_t ms = 0;
float    t  = 0; // DS18B20 temp

AsyncWebServer webServer(80);
AsyncWebSocket webSock("/");

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

IPAddress  // soft AP IP info
          ip_AP(10,0,1,23)              // default IP
        , ip_GW(10,0,0,125)             // default GW
        , ip_AP_GW(10,0,1,23)           // Self AP - GW
        , ip_subNet(255,255,255,128);   // subnet  10000000, i.e. 127 possible IPs

  struct {
      std::string ssid = "myWiFi";     // max 342 characters
      std::string pwd  = "secretPWD";  // max 64 characters
      bool isDHCP = false;
      std::string ip   = "198.162.123.117";
      std::string gw   = "198.162.123.117";
      std::string mask = "255.255.255.128";
      bool updated = 0;

      std::string toStr(){
        char s[150];
        size_t n;
        n = sprintf(s, "{\"src\":\"mcuNetwork\",\"ssid\":\"%s\",\"pwd\":\"%s\",\"isDHCP\":%hhu,\"ip\":\"%s\",\"gw\":\"%s\",\"mask\":\"%s\",\"updated\":%hhu}"
        , ssid.c_str(), pwd.c_str(), isDHCP?1:0, ip.c_str(), gw.c_str(), mask.c_str(), updated);
        return(std::string(s, n));
      };
  } network;

  /******  PROTOTYPES  *******/
  // web
  void notFound(AsyncWebServerRequest *request);
  void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

  bool initWiFi(); // load struct creds from LittleFS
  bool saveWiFi(); // save struct creds JSON string in LittleFS 
  void initWiFi(const std::string &s); // load struct creds from WebSock string
  bool wifiConnect(WiFiMode m);
  IPAddress str2IP(const std::string &s);
  std::string mac2string(const uint8_t *mac);
  std::string toStr();
  std::string valFromJson(const std::string &json, const std::string &element);// Parse JSON string

  void setup(){
    delay(500);
#ifdef dbg
    Serial.begin(115200);
    Serial.printf("\n%i\n", __LINE__);
#endif
    ds18b20.begin();

    LittleFS.begin(); // store WiFi Creds
    initWiFi();       // get WiFi credentials from LittleFS

    wifi_set_phy_mode(PHY_MODE_11B); // high power wifi

    if(!wifiConnect(WIFI_STA))
      wifiConnect(WIFI_AP);

#ifndef dbg
		pinMode(SSR_PIN, OUTPUT);
    digitalWrite(SSR_PIN, 0);
#endif

		// init Websock
		webSock.onEvent(onWsEvent);
		webServer.addHandler(&webSock);

		// Web Pages
		webServer.onNotFound(notFound);

    if(WiFi.softAPIP().toString() == "(IP unset)"){
      webServer.on("/"         , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", MAIN);});
      webServer.on("/wifi"     , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", NETWORK);});
    }
    else {webServer.on("/"     , HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", NETWORK);});}

		AsyncElegantOTA.begin(&webServer);    // Start ElegantOTA
		webServer.begin();
		startMillis=millis()-READINGS_DELAY; // loop timer; set so as to execute upon entry of loop()
  }

  void loop() {
     if(millis()-startMillis > READINGS_DELAY){
        if((int)ms-READINGS_DELAY<0)ms=0;else ms -=READINGS_DELAY;
        digitalWrite(SSR_PIN, (bool)ms);
        ds18b20.requestTemperatures();
        t = ds18b20.getTempFByIndex(0);

        webSock.textAll(toStr().c_str());
        startMillis=millis();
     }
  }// END loop()
 
  void notFound(AsyncWebServerRequest *request){request->send_P(200, "text/html", MAIN);}
  //**********************************************************************
  void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){}
    if(type == WS_EVT_DISCONNECT){return;}
//**************
    if(type == WS_EVT_DATA){
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      if(info->final && !info->index && info->len == len){
        if(info->opcode == WS_TEXT){
          std::string s((char*)data, len);
#ifdef dbg
    Serial.printf("%i ws_in: %s\n", __LINE__, s.c_str());
#endif
          if(valFromJson(s, "src")=="network"){
            if(::atoi(valFromJson(s, "updated").c_str())){
Serial.printf("%i updated: %s : %i\n",__LINE__, valFromJson(s, "updated").c_str(), ::atoi(valFromJson(s, "updated").c_str()));
              if(::atoi(valFromJson(s, "reboot").c_str())){ESP.restart();}
              initWiFi(s); // <-- updated, but no reboot yet
              network.updated = saveWiFi();
            }
            client->text(network.toStr().c_str()); // include "updated" value
#ifdef dbg
    Serial.printf("%i ws_out: %s\n", __LINE__, network.toStr().c_str());
#endif
            return;
          }
          if(valFromJson(s, "src")=="main"){ // main == index.html web page
            if(::atoi(valFromJson(s, "ms").c_str())>=0){ms = ::atoi(valFromJson(s, "ms").c_str());}
            digitalWrite(SSR_PIN, (bool)ms);
            webSock.textAll(toStr().c_str());
          }
        }
      }
    }
  }
  std::string toStr(){
    char buff[45];
    sprintf(buff, "{\"src\":\"mcu\",\"ms\":%u,\"t\":%.1f}", ms, t);
    return std::string(buff);
  }
  bool initWiFi(){
        File f = LittleFS.open("/network", "r");
        if(f){
               const std::string s = f.readString().c_str();
                f.close();
                initWiFi(s);
//                std::string s1 = valFromJson(s, "ip");
//                strcat(AP_SSID, s1.c_str());
                return(true);
        }
        return(false);
  }
  void initWiFi(const std::string &s){
    network.ssid   = valFromJson(s, "ssid");
    network.ip     = valFromJson(s, "ip");
    network.pwd    = valFromJson(s, "pwd");
    network.isDHCP = valFromJson(s, "isDHCP")=="1";
    network.gw     = valFromJson(s, "gw");
    network.mask   = valFromJson(s, "mask");
  }
  // saveWiFi() resets "updated" flag before the save
  bool saveWiFi(){
    network.updated=0; // we never save "updated's" state
    File f = LittleFS.open(F("/network"), "w");
    if(f){
      f.print(network.toStr().c_str());
      f.close();
      return 1;
    }
    else
      return 0;
  }
  bool wifiConnect(WiFiMode m){
    WiFi.disconnect();
    WiFi.softAPdisconnect();

    WiFi.mode(m);  // WIFI_AP_STA,WIFI_AP; WIFI_STA;

    switch(m){
        case WIFI_STA:
          WiFi.hostname("Attic Fan Controller");
          if(network.isDHCP==false){
            if(WiFi.config(str2IP(network.ip), str2IP(network.gw), str2IP(network.mask)))
              WiFi.begin(network.ssid.c_str(), network.pwd.c_str());
            else
              return(false);
          }
          break;
        case WIFI_AP:
          WiFi.softAPConfig(ip_AP, ip_AP_GW, ip_subNet);
          WiFi.softAP(AP_SSID);//, "", 7, false, 4);
          WiFi.begin();
        break;
        case WIFI_AP_STA:
          delay(25);
          WiFi.hostname("Attic Fan Controller");
          if(!network.isDHCP)
            WiFi.config(str2IP(network.ip), str2IP(network.gw), str2IP(network.mask));
          WiFi.softAPConfig(ip_AP, ip_AP_GW, ip_subNet);
          WiFi.softAP(AP_SSID, "", 3, false, 4);
          WiFi.begin(network.ssid.c_str(), network.pwd.c_str(), 1);//, 7, (const uint8_t *)__null, true);
        break;
        case WIFI_OFF: return(true);
    }
    unsigned int startup = millis();
#ifdef dbg
            Serial.println("\n");
#endif
    while(WiFi.status() != WL_CONNECTED){
          delay(750);
#ifdef dbg
            Serial.print(".");
#endif
          if(millis() - startup >= 5000) break;
    }
#ifdef dbg
      Serial.print("\n\n");
#endif


#ifdef dbg
    Serial.printf("mode: %i, ip: %s, gw: %s, mask: %s\n", m, WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());Serial.flush();
    Serial.printf("%i AP_ssid: \"%s\", AP_ip: %s\n", __LINE__, WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str());Serial.flush();
    Serial.printf("\n%i MAC: %s, RSSI: %i\n", __LINE__, WiFi.macAddress().c_str(), WiFi.RSSI());Serial.flush();
    delay(100);
#endif
    return((WiFi.status()== WL_CONNECTED)?true:false);
  }
	IPAddress str2IP(const std::string &s){
    unsigned int parts[4] ={0,0,0,0};
    int part=0;

    for( std::size_t i=0 ; i< s.length() ; i++){
    if(s[i] == '.')part++;
      else{
          parts[part]*=10;
          parts[part]+= (unsigned char)s[i]-'0';
      }
    }
    return (IPAddress(parts[0], parts[1], parts[2], parts[3]));
	}
  std::string valFromJson(const std::string &json, const std::string &element){
    uint32_t start, end;
    start = json.find(element);
    if(start == std::string::npos)return "";
    start = json.find(":", start)+1;
    if(json.substr(start,1) =="\"")start++;
    end  = json.find_first_of(",]}\"", start);
    if(start>json.length() || end-start<0)return "";
    return(json.substr(start, end-start));
  }
  std::string mac2string(const uint8_t *mac){
    char macStr[18];
    snprintf(macStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return(macStr);
  }
