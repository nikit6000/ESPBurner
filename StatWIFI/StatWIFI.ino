#include <ArduinoJson.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SSD1306.h>
#include <OLEDDisplayUi.h>
#include "images.h"
#include "basicPixel.h"
#include <EEPROM.h>

//#define USE_DEBUG_OUTPUT

#define PAYLOAD_PER_PAGE 3
#define PAYLOAD_OFFSET 36
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define GPU_JSON_BUFFER_SIZE 1162
#define GLOBAL_JSON_BUFFER_SIZE 1722

void updateData();
bool loadData(char* host, int port, String req, String &result);
void processSerial();
void processCommand(char ** command, int len);
void readConfig();
void writeConfig();
bool wifiConnect();
void displayError(String error);
bool isNumber(char * value, int &out);
void drawCPUNode(String json);
void drawGPUNode(String json);
void drawPayload(JsonArray & payload);
void drawNodeName();
void drawWifiQuality();

char* ssid = NULL;
char* password = NULL;
char* host = NULL;  // IP serveur - Server IP
int port = 8085;            // Port serveur - Server Port
const int   watchdog = 5000;        // Fréquence du watchdog - Watchdog frequency
unsigned long previousMillis = millis(); 
unsigned long updatePrevMillis  = millis();
int payloadPage = 0;

String currentNode = "global";
bool reqFullNode = true;
bool hasError = false;

SSD1306  display(0x3c, D3, D5);

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.setTimeout(0);

  readConfig();
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  wifiConnect();

  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.clear();
  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();
}
 
void loop() {

  processSerial();
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - updatePrevMillis > 1000) {
      updatePrevMillis = millis();
      updateData();
    }
  }
}

void updateData(){
  String req =  "/getStat?node=" + currentNode + "&full=";
  if (reqFullNode) {
    req += "1";
  } else {
    req += "0";
  }
  String result;
  if ( loadData(host, port, req, result) ){
      hasError = false;
      if (currentNode == "global"){
          drawCPUNode(result);
      } else if (currentNode.startsWith("gpu")) {
          drawGPUNode(result);          
      }
      drawNodeName();
      drawWifiQuality();
  } else {
      displayError(result);
  }
  display.display();
  //Serial.println(result);
}

bool loadData(char* host, int port, String req, String &result) { 

  if (host == NULL){
      result = "No host";
      return false;
  }
  HTTPClient http;

  #ifdef USE_DEBUG_OUTPUT 
  Serial.print("[HTTP] begin...\n");
  Serial.println("http://"+host+":"+port+""+req);
  #endif


  
  http.begin("http://"+String(host)+":"+port+""+req); //HTTP

  #ifdef USE_DEBUG_OUTPUT 
  Serial.print("[HTTP] GET...\n");
  #endif
  
  int httpCode = http.GET();
  
  if(httpCode > 0) {
      #ifdef USE_DEBUG_OUTPUT 
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      #endif
      
      if(httpCode == HTTP_CODE_OK) {
          result = http.getString();
          #ifdef USE_DEBUG_OUTPUT 
          Serial.println(result);
          #endif
      } else  {
          DynamicJsonBuffer jsonBuffer(40);
          JsonObject& root = jsonBuffer.parseObject(http.getString());
          String error = root["error"];
          result = error;
          http.end();
          return false;
      }
  } else {
      #ifdef USE_DEBUG_OUTPUT 
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif
      result = http.errorToString(httpCode);
      http.end();
      return false;
  }

  http.end();
  return true;
}

void processSerial (){
    int bufferLen = 0;
    int commandLen = 0;
    char ** command = NULL;
    char * buffer = NULL;
    
    while(Serial.available()){   
        char chr = Serial.read();
        if (chr == ' ' || chr == '\r') {
            if (buffer != NULL) {
                buffer = (char*)realloc(buffer, sizeof(char)*++bufferLen);
                command = (char**)realloc(command, sizeof(char*)*++commandLen);
                if (command == NULL || buffer == NULL)
                    return;
                buffer[bufferLen - 1] = '\0';
                command[commandLen - 1] = buffer;
                buffer = NULL;
                bufferLen = 0;
            }
            if (chr == ' ')
              continue;
            else
              break;
        }
        buffer = (char*)realloc(buffer, sizeof(char)*++bufferLen);
        if (buffer == NULL)
            return;
        buffer[bufferLen - 1] =  chr;
    }
    if (buffer != NULL)
        delete [] buffer;
    processCommand(command,commandLen);
    if (command != NULL) {
        for (int i = 0; i < commandLen; i++){
            delete [] command[i];
        }
        delete [] command;
    }
}

void processCommand(char ** command, int len) {
    if (command == NULL)
        return;
        
    if (len > 0) {
        if (strcmp(command[0], "connect") == 0){
            if (len < 3){
                Serial.println("Not enough arguments. \nUse: connect <ssid> <pass>");
                return;
            }
            if (strlen(command[1]) > 128) {
                Serial.println("SSID length must be less than 128!");
                return;
            }
            if (strlen(command[2]) > 128){
                Serial.println("PASSWORD length must be less than 128!");
                return;
            }
            WiFi.disconnect();
            if (ssid != NULL)
                delete [] ssid;
            if (password != NULL)
                delete [] password;
            ssid = (char*)malloc(sizeof(char) * (strlen(command[1]) + 1));
            password = (char*)malloc(sizeof(char) * (strlen(command[2]) + 1));
            memcpy(ssid, command[1], (strlen(command[1]) + 1));
            memcpy(password, command[2], (strlen(command[2]) + 1));
           
            if ( wifiConnect() ){
                writeConfig();
            }
            else 
                Serial.println("Connection failed");
        } else if (strcmp(command[0], "scan") == 0) {
            Serial.println("~~~ Scanning for networks ~~~");
            int n = WiFi.scanNetworks();
            Serial.println("~~~~~~~~~ Scan done ~~~~~~~~~");
            if (n == 0)
              Serial.println("No networks found");
            else
            {
              Serial.print(n);
              Serial.println(" networks found");
              for (int i = 0; i < n; ++i)
              {
                // Print SSID and RSSI for each network found
                Serial.printf("%3d: %20s | (%4d) | %5s\n", i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "OPEN":"PASS");
                delay(10);
              }
            }
        } else if (strcmp(command[0], "status") == 0) {
            
        } else if (strcmp(command[0], "sethost") == 0) {
            if (len < 3){
                Serial.println("Not enough arguments. \nUse: sethost <adress> <port>\nDo not add 'http://' as prefix!");
                return;
            }
            if (strlen(command[1]) > 128) {
                Serial.println("ADRESS length must be less than 128!");
                return;
            }
            if (host != NULL)
                delete [] host;
            host = (char*)malloc(sizeof(char)*(strlen(command[1]) + 1));
            memcpy(host, command[1], (strlen(command[1]) + 1));
            if (strlen(command[2]) > 5) {
                Serial.println("Port value too long, must be less than 6");
                return;
            }
            int value;
            if (!isNumber(command[2], value)){
                Serial.println("Port value must be 16-bit integer");
                return; 
            }
            port = value;
            if (WiFi.status() == WL_CONNECTED) {
                writeConfig();
                Serial.println("OK");
            } else {
                Serial.println("Connet to WiFi first!");
            }
        } else if (strcmp(command[0], "gethost") == 0) {
            if (host == NULL) {
                Serial.println("Host is not set!\nUse: sethost <adress> <port>\nDo not add 'http://' as prefix!");
                return;
            }
            Serial.print("host: http://");
            Serial.print(host);
            Serial.print(":");
            Serial.println(port);
        } else if (strcmp(command[0], "setpage") == 0) {
            if (len < 2) {
                Serial.println("Not enough arguments. \nUse: setpage <page>");
                return;
            }
            if (!isNumber(command[1], payloadPage)){
                Serial.println("Page value must be integer");
                return; 
            }
            Serial.println("OK");
        } else if (strcmp(command[0], "setnode") == 0) {
             if (len < 2) {
                Serial.println("Not enough arguments. \nUse: setnode <name>");
                return;
            }
            currentNode = String(command[1]);
            Serial.println("OK");
        } else {
            Serial.println("Unknown command!");
        }
    }
}

void readConfig() {
    int currentAdress = 0;

    /* reading ssid */
    
    byte ssidLen = EEPROM.read(currentAdress++);

    if (ssidLen == 0) {
        return;
    }
    
    if (ssid != NULL) {
        delete [] ssid;
        ssid = NULL;
    }
    ssid = (char*)malloc(sizeof(char) * (ssidLen + 1));
    ssid[ssidLen] = '\0';
    for (int i = 0; i < ssidLen; i++) {
        ssid[i] = EEPROM.read(currentAdress++);
    }

    /* reading pass */
    
    byte passLen = EEPROM.read(currentAdress++);
    if (password != NULL) {
        delete [] password;
        password = NULL;
    }
    password = (char*)malloc(sizeof(char) * (passLen + 1));
    password[passLen] = '\0';
    for (int i = 0; i < passLen; i++) {
        password[i] = EEPROM.read(currentAdress++);
    }

    /* reading host */
    byte hostLen = EEPROM.read(currentAdress++);
    if (hostLen != 0){
        if (host != NULL) {
            delete [] host;
            host = NULL;
        }
        host = (char*)malloc(sizeof(char) * (hostLen + 1));
        host[hostLen] = '\0';
        for (int i = 0; i < hostLen; i++) {
            host[i] = EEPROM.read(currentAdress++);
        }
    }

    /* reading port */
    byte low = EEPROM.read(currentAdress++);
    byte high = EEPROM.read(currentAdress++);

    port = ((uint16_t)high << 8) | low;
    
}

void writeConfig(){
    if (ssid == NULL || password == NULL) 
        return;
    
    int currentAdress = 0;
    EEPROM.write(currentAdress++, strlen(ssid));
    for (int i = 0; i < strlen(ssid); i++) {
        EEPROM.write(currentAdress++, ssid[i]);
    }
    EEPROM.write(currentAdress++, strlen(password));
    for (int i = 0; i < strlen(password); i++) {
        EEPROM.write(currentAdress++, password[i]);
    }
    if (host != NULL) {
        EEPROM.write(currentAdress++, strlen(host));
        for (int i = 0; i < strlen(host); i++) {
            EEPROM.write(currentAdress++, host[i]);
        }
    } else {
        EEPROM.write(currentAdress++, 0);
    }
    EEPROM.write(currentAdress++, (byte)(port & 0xFF));
    EEPROM.write(currentAdress++, (byte)((port >> 8) & 0xFF));
    EEPROM.commit();
}

bool wifiConnect() {

    if (ssid == NULL)
        return false;
    
    unsigned long start = millis();
    WiFi.begin(ssid, password);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - start > 30000){
            Serial.print("\n");
            return false;
        }
    }
    Serial.print("\nConnected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void displayError(String error) {
    hasError = true;
    display.setColor(BLACK);
    display.fillRect(0,0,128,14);
    display.setColor(WHITE);
    display.fillRect(10,0,108,14);
    display.setColor(BLACK);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0, error);
}

bool isNumber(char * value, int &out) {
    for (byte i = 0; i < strlen(value); i++){
        if (value[i] >= '0' && value[i] <= '9') {
            continue;
        } else {
            return false;
        }
    }
    out = atoi(value);
    return true;
}

void drawCPUNode(String json) {
    DynamicJsonBuffer jsonBuffer(GLOBAL_JSON_BUFFER_SIZE);
    JsonObject& root = jsonBuffer.parseObject(json);
    int framerate = root["framerate"];
    String FPS = "FPS: " + String(framerate);
    display.clear();
    display.setColor(WHITE);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(pixel_20);
    display.drawString(0, 12, FPS);
    drawPayload(root["payload"]);
}

void drawGPUNode(String json) {
    DynamicJsonBuffer jsonBuffer(GPU_JSON_BUFFER_SIZE);
    JsonObject& root = jsonBuffer.parseObject(json);

    String device = root["device"];
    String driver = root["driver"];
    
    display.clear();
    display.setColor(WHITE);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 13, device);
    display.drawString(0, 23, driver);
    drawPayload(root["payload"]);
}

void drawPayload(JsonArray & payload) {
    if (payloadPage < payload.size() / PAYLOAD_PER_PAGE ){
        display.setColor(BLACK);
        display.fillRect(0,PAYLOAD_OFFSET, DISPLAY_WIDTH, DISPLAY_HEIGHT - PAYLOAD_OFFSET);
        for (byte i = 0; i < PAYLOAD_PER_PAGE; i++) {
            int index = payloadPage * PAYLOAD_PER_PAGE + i;
            if (index < payload.size()) {
                int value = payload[index]["value"];
                String name = payload[index]["name"];
                String unit = payload[index]["unit"];
                display.setColor(WHITE);
                display.setTextAlignment(TEXT_ALIGN_LEFT);
                display.setFont(ArialMT_Plain_10);
                display.drawString(0, PAYLOAD_OFFSET + i * 9, name);
                display.setTextAlignment(TEXT_ALIGN_RIGHT);
                display.drawString(DISPLAY_WIDTH, PAYLOAD_OFFSET + i * 9, String(value) + " " + unit);
            }
        }
    }
}

void drawNodeName() {
    if (hasError)
        return;
    int width = currentNode.length() * 7 + 4;
    display.setColor(WHITE);
    display.fillRect(0,0, width,14);
    display.setColor(BLACK);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(width / 2, 0, currentNode);
}

void drawWifiQuality() {
    if (hasError)
        return;
    float quality = 0;
    long rssi = WiFi.RSSI();
    if(rssi <= -100)
        quality = 0;
    else if(rssi >= -50)
        quality = 100;
    else
        quality = 2 * (rssi + 100);

    //qlt3_ico_bits
    display.setColor(WHITE);
    if (quality == 0) {
        display.drawXbm(DISPLAY_WIDTH - 11, 1, ico_width, ico_height, qlt0_ico_bits);
    } else if (quality > 0 && quality <= 33) {
        display.drawXbm(DISPLAY_WIDTH - 11, 1, ico_width, ico_height, qlt1_ico_bits);
    } else if (quality > 33 && quality <= 66) {
        display.drawXbm(DISPLAY_WIDTH - 11, 1, ico_width, ico_height, qlt2_ico_bits);
    } else if (quality > 66) {
        display.drawXbm(DISPLAY_WIDTH - 11, 1, ico_width, ico_height, qlt3_ico_bits);
    }
    display.drawXbm(DISPLAY_WIDTH - 24, 1, ico_width, ico_height, signal_ico_bits);
}

