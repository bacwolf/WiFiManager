/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials and
   store them in EEPROM.
   inspired by http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>


const char HTTP_200[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM = "<style>div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}</style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM = "</head><body><div style='text-align: left; display: inline-block;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form>";
const char HTTP_ITEM[] PROGMEM = "<div><a href='#' onclick='c(this)'>{v}</a> {r}% {i}</div>";
const char HTTP_ITEM_PADLOCK[] PROGMEM = "<img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAgAAAAKCAMAAAC+Ge+yAAAACVBMVEUAAAC7u7v///9etNz6AAAAI0lEQVQI133GsQ3AMAAEoXv2H9qNJacKFbEaWTQJ+uQB/nMdEaYAXeJkZ9AAAAAASUVORK5CYII='/>";
const char HTTP_FORM_START[] PROGMEM = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}'>";
const char HTTP_FORM_END[] PROGMEM = "<br/><button type='submit'>save</button></form>";

const char HTTP_SAVED[] PROGMEM = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM = "</div></body></html>";

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter {
  public:
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);

    const char* getID();
    const char* getValue();
    const char* getPlaceholder();
    int getValueLength();
  private:
    const char* _id;
    const char* _placeholder;
    char* _value;
    int _length;

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    String        getSSID();
    String        getPassword();

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();
    //for convenience
    String        urldecode(const char*);

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //usefully for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds
    void          setTimeout(unsigned long seconds);
    void          setDebugOutput(boolean debug);

    //sets a custom ip /gateway /subnet configuration
    void          setAPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(void) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );

    //adds a custom parameter
    void          addParameter(WiFiManagerParameter *p);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    const int     WM_DONE = 0;
    const int     WM_WAIT = 10;

    const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();

    int           _eepromStart;
    const char*   _apName = "no-net";
    const char*   _apPassword = NULL;
    String        _ssid = "";
    String        _pass = "";
    unsigned long timeout = 0;
    unsigned long start = 0;
    IPAddress     _ip;
    IPAddress     _gw;
    IPAddress     _sn;
    int           _paramsCount = 0;

    String        getEEPROMString(int start, int len);
    void          setEEPROMString(int start, int len, String string);

//    boolean keepLooping = true;
    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(void) = NULL;
    void (*_savecallback)(void) = NULL;

    WiFiManagerParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    void          DEBUG_WM(Generic text);
};



#endif
