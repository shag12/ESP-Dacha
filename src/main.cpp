#include <M5ez.h>

#include <ezTime.h>

#include "images.h"

#include <SPIFFS.h>

#include <HTTPClient.h>

#include <ArduinoJson.h>


#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

//#define MQTT_HOST IPAddress(178, 213, 114, 222)
#define MQTT_HOST IPAddress(192, 168, 1, 3)
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;

const char* mqtt_user = "shag";
const char* mqtt_pass = "Assasin12";
//const char* mqtt_topic = "shag/ESP-HOME/#";
const char* mqtt_topic = "shag/#";


struct sensors
  {
    String name;
    String Sens;
    int pos; 
    String data;
  };
  

sensors sens[] = {
  {"Themp: ","ESP-HOME/dsw1",60,"0"},
  {"Hum: ","ESP-HOME/dhth1",120,"0"},
  {"Pre: ","ESP-HOME/bmpp",180,"0"},
  {"Tp_T: ","ESP-Tpol/dsw4",120,"0"},
  {"Tp_S: ","ESP-Tpol/dsw3",120,"0"},
  {"Bass: ","ESP-BASS/dsw1",120,"0"}
};
  





#define USE_SERIAL Serial


#define MAIN_DECLARED

 const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIF6TCCA9GgAwIBAgIQBeTcO5Q4qzuFl8umoZhQ4zANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQw\n" \
"OTEyMDAwMDAwWhcNMjQwOTExMjM1OTU5WjBfMQswCQYDVQQGEwJGUjEOMAwGA1UE\n" \
"CBMFUGFyaXMxDjAMBgNVBAcTBVBhcmlzMQ4wDAYDVQQKEwVHYW5kaTEgMB4GA1UE\n" \
"AxMXR2FuZGkgU3RhbmRhcmQgU1NMIENBIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQCUBC2meZV0/9UAPPWu2JSxKXzAjwsLibmCg5duNyj1ohrP0pIL\n" \
"m6jTh5RzhBCf3DXLwi2SrCG5yzv8QMHBgyHwv/j2nPqcghDA0I5O5Q1MsJFckLSk\n" \
"QFEW2uSEEi0FXKEfFxkkUap66uEHG4aNAXLy59SDIzme4OFMH2sio7QQZrDtgpbX\n" \
"bmq08j+1QvzdirWrui0dOnWbMdw+naxb00ENbLAb9Tr1eeohovj0M1JLJC0epJmx\n" \
"bUi8uBL+cnB89/sCdfSN3tbawKAyGlLfOGsuRTg/PwSWAP2h9KK71RfWJ3wbWFmV\n" \
"XooS/ZyrgT5SKEhRhWvzkbKGPym1bgNi7tYFAgMBAAGjggF1MIIBcTAfBgNVHSME\n" \
"GDAWgBRTeb9aqitKz1SA4dibwJ3ysgNmyzAdBgNVHQ4EFgQUs5Cn2MmvTs1hPJ98\n" \
"rV1/Qf1pMOowDgYDVR0PAQH/BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYD\n" \
"VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMCIGA1UdIAQbMBkwDQYLKwYBBAGy\n" \
"MQECAhowCAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNl\n" \
"cnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNy\n" \
"bDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRy\n" \
"dXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZ\n" \
"aHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAWGf9\n" \
"crJq13xhlhl+2UNG0SZ9yFP6ZrBrLafTqlb3OojQO3LJUP33WbKqaPWMcwO7lWUX\n" \
"zi8c3ZgTopHJ7qFAbjyY1lzzsiI8Le4bpOHeICQW8owRc5E69vrOJAKHypPstLbI\n" \
"FhfFcvwnQPYT/pOmnVHvPCvYd1ebjGU6NSU2t7WKY28HJ5OxYI2A25bUeo8tqxyI\n" \
"yW5+1mUfr13KFj8oRtygNeX56eXVlogMT8a3d2dIhCe2H7Bo26y/d7CQuKLJHDJd\n" \
"ArolQ4FCR7vY4Y8MDEZf7kYzawMUgtN+zY+vkNaOJH1AQrRqahfGlZfh8jjNp+20\n" \
"J0CT33KpuMZmYzc4ZCIwojvxuch7yPspOqsactIGEk72gtQjbz7Dk+XYtsDe3CMW\n" \
"1hMwt6CaDixVBgBwAc/qOR2A24j3pSC4W/0xJmmPLQphgzpHphNULB7j7UTKvGof\n" \
"KA5R2d4On3XNDgOVyvnFqSot/kGkoUeuDcL5OWYzSlvhhChZbH2UF3bkRYKtcCD9\n" \
"0m9jqNf6oDP6N8v3smWe2lBvP+Sn845dWDKXcCMu5/3EFZucJ48y7RetWIExKREa\n" \
"m9T8bJUox04FB6b9HbwZ4ui3uRGKLXASUoWNjDNKD/yZkuBjcNqllEdjB+dYxzFf\n" \
"BT02Vf6Dsuimrdfp5gJ0iHRc2jTbkNJtUQoj1iM=\n" \
"-----END CERTIFICATE-----\n";


const char* serverName = "https://zont-online.ru/api/devices";



void mainmenu_image();
void powerOff();
void aboutM5ez();
void sysInfo();
void sysInfoPage1();
void sysInfoPage2();
void dacha();
void dacha_a();
void h_get();
void h_get2();


//MQTT

void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);

//MQTT END












void setup() {
  #include <themes/default.h>
  #include <themes/dark.h>
  ezt::setDebug(DEBUG);
  ez.begin();
  ez.clock.tz = "Europe/Moscow";
  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();
  for(uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(mqtt_user,mqtt_pass);

  
}

void loop() {
  ezMenu mainmenu("DACHA info");
  mainmenu.txtSmall();
 // mainmenu.addItem("Image menus", mainmenu_image);
  mainmenu.addItem("Built-in wifi & other settings", ez.settings.menu);
  mainmenu.addItem("DACHA", dacha);
  mainmenu.addItem("DACHA ALL", dacha_a);
  mainmenu.addItem("Power OFF", powerOff);
  //mainmenu.addItem("Bass");
  mainmenu.upOnFirst("last|up");
  mainmenu.downOnLast("first|down");
  mainmenu.run();
  
}


//void mainmenu_image() {
//  ezMenu images;
//  images.imgBackground(TFT_BLACK);
//  images.imgFromTop(40);
//  images.imgCaptionColor(TFT_WHITE);
//  images.addItem(sysinfo_jpg, "System Information", sysInfo);
//  images.addItem(wifi_jpg, "WiFi Settings", ez.wifi.menu);
//  images.addItem(about_jpg, "About M5ez", aboutM5ez);
//  images.addItem(sleep_jpg, "Power Off", powerOff);
//  images.addItem(return_jpg, "Back");
//  images.run();
//}


void powerOff() { m5.powerOFF(); }

void aboutM5ez() {
  ez.msgBox("About M5ez", "M5ez was written by | Rop Gonggrijp | | https://github.com/ropg/M5ez");
}


String exit_button = "Exit";

void sysInfo() {
  sysInfoPage1();
  while(true) {
    String btn = ez.buttons.poll();
    if (btn == "up") sysInfoPage1();
    if (btn == "down") sysInfoPage2();
    if (btn == "Exit") break;
  }
}


void sysInfoPage1() {
  const byte tab = 120;
  ez.screen.clear();
  ez.header.show("System Info  (1/2)");
  ez.buttons.show("#" + exit_button + "#down");
  ez.canvas.font(&FreeSans9pt7b);
  ez.canvas.lmargin(10);
  ez.canvas.println("");
  ez.canvas.print("CPU freq:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getCpuFreqMHz()) + " MHz");
  ez.canvas.print("CPU cores:");  ez.canvas.x(tab); ez.canvas.println("2");    //   :)
  ez.canvas.print("Chip rev.:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getChipRevision()));
  ez.canvas.print("Flash speed:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
  ez.canvas.print("Flash size:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getFlashChipSize() / 1000000) + " MB");
  ez.canvas.print("ESP SDK:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getSdkVersion()));
  ez.canvas.print("M5ez:");  ez.canvas.x(tab); ez.canvas.println(String(ez.version()));
}

void sysInfoPage2() {
  const String SD_Type[5] = {"NONE", "MMC", "SD", "SDHC", "UNKNOWN"};
  const byte tab = 140;
  ez.screen.clear();
  ez.header.show("System Info  (2/2)");
  ez.buttons.show("up#" + exit_button + "#");
  ez.canvas.font(&FreeSans9pt7b);
  ez.canvas.lmargin(10);
  ez.canvas.println("");
  ez.canvas.print("Free RAM:");  ez.canvas.x(tab);  ez.canvas.println(String((long)ESP.getFreeHeap()) + " bytes");
  ez.canvas.print("Min. free seen:");  ez.canvas.x(tab); ez.canvas.println(String((long)esp_get_minimum_free_heap_size()) + " bytes");
  const int sd_type = SD.cardType();
  
  SPIFFS.begin();
  ez.canvas.print("SPIFFS size:"); ez.canvas.x(tab); ez.canvas.println(String((long)SPIFFS.totalBytes()) + " bytes");
  ez.canvas.print("SPIFFS used:"); ez.canvas.x(tab); ez.canvas.println(String((long)SPIFFS.usedBytes()) + " bytes");
  ez.canvas.print("SD type:"); ez.canvas.x(tab); ez.canvas.println(SD_Type[sd_type]);
  if (sd_type != 0) {
    ez.canvas.print("SD size:"); ez.canvas.x(tab); ez.canvas.println(String((long)SD.cardSize()  / 1000000) + " MB");
    ez.canvas.print("SD used:"); ez.canvas.x(tab); ez.canvas.println(String((long)SD.usedBytes()  / 1000000) + " MB");
  }

}
void dacha() {
  int i=1;
   ez.screen.clear();
   ez.header.show("Dacha");
   ez.buttons.show("#" + exit_button + "#");


  while(true) {

    connectToMqtt();
 
    h_get2();

    for(i=0;i<2000000;i++) {
      String btn = ez.buttons.poll();
      //if (btn == "http") h_get();
      if (btn == "Exit") return;

    }
  }
}

void dacha_a() {
  int i=1;
   ez.screen.clear();
   ez.header.show("Dacha ALL");
   ez.buttons.show("#" + exit_button + "#");
  while(true) {
    h_get();

    for(i=0;i<2000000;i++) {
      String btn = ez.buttons.poll();
      //if (btn == "http") h_get();
      if (btn == "Exit") return;
    }

  }
}



void h_get() {


  // the following variables are unsigned longs because the time, measured in
  // milliseconds, will quickly become a bigger number than can be stored in an int.
  // unsigned long lastTime = 0;
  // Timer set to 10 minutes (600000)
  // unsigned long timerDelay = 600000;
  // Set timer to 5 seconds (5000)
  // unsigned long timerDelay = 5000; 

  DynamicJsonDocument doc(24576);
  
  
  HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(serverName,rootCACertificate);
        //http.setAuthorization("z19015", "Assasin12");
        http.addHeader("Authorization", "Basic ejE5MDE1OlNlbTFyYXNhMTI=");
        http.addHeader("X-ZONT-Client", "shag12@mail.ru");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int httpCode = http.POST("load_io=true");
        
        USE_SERIAL.print("[HTTPS] POST...\n");
       
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                //USE_SERIAL.println(payload);
                long sss = payload.length();
               
                USE_SERIAL.println(String(sss));

                // Deserialize the JSON document
                DeserializationError error = deserializeJson(doc, payload);

                // Test if parsing succeeds.
                if (error) {
                  USE_SERIAL.print(F("deserializeJson() failed: "));
                  USE_SERIAL.println(error.f_str());
                  return;
                }
                ez.canvas.clear();
                ez.canvas.font(&FreeSansBold12pt7b);
                ez.canvas.lmargin(10);
                ez.canvas.println(""); 
                for(int tt=4096;tt<4110;tt=tt+2) {
                  
                  String term_d = String(tt);
                  JsonObject devices_0_z3k = doc["devices"][0]["io"]["z3k-state"][term_d];
                  float term = devices_0_z3k["curr_temp"];
                  ez.canvas.color(TFT_BLACK);
                  ez.canvas.print(term_d +": ");
                  ez.canvas.color(TFT_RED);
                  ez.canvas.print(String(term)+"    ");
                  //USE_SERIAL.println(term_d);
                  //USE_SERIAL.println(String(term));
                  term_d = String(tt+1);
                  devices_0_z3k = doc["devices"][0]["io"]["z3k-state"][term_d];
                  term = devices_0_z3k["curr_temp"];
                  ez.canvas.color(TFT_BLACK);
                  ez.canvas.print(term_d +": ");
                  ez.canvas.color(TFT_RED);
                  ez.canvas.println(String(term));


                }


            }
        } else {
            USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
}




void h_get2() {

 
  // the following variables are unsigned longs because the time, measured in
  // milliseconds, will quickly become a bigger number than can be stored in an int.
  // unsigned long lastTime = 0;
  // Timer set to 10 minutes (600000)
  // unsigned long timerDelay = 600000;
  // Set timer to 5 seconds (5000)
  // unsigned long timerDelay = 5000; 

  DynamicJsonDocument doc(25576);
  
  HTTPClient http;
  float term;

  int Pos1 = 105;
  int Pos2 = 240;

       
  USE_SERIAL.print("[HTTP] begin...\n");
  http.begin(serverName,rootCACertificate);
  http.addHeader("Authorization", "Basic ejE5MDE1OlNlbTFyYXNhMTI=");
  http.addHeader("X-ZONT-Client", "shag12@mail.ru");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("load_io=true");
  
  USE_SERIAL.print("[HTTPS] POST...\n");
 
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          long sss = payload.length();
         
          USE_SERIAL.println(String(sss));

          // Deserialize the JSON document
          DeserializationError error = deserializeJson(doc, payload);

          // Test if parsing succeeds.
          if (error) {
            USE_SERIAL.print(F("deserializeJson() failed: "));
            USE_SERIAL.println(error.f_str());
            return;
          }
          ez.canvas.clear();
          ez.canvas.font(&FreeSansBold12pt7b);
          ez.canvas.lmargin(10);
          ez.canvas.println(""); 
           ez.canvas.font(&FreeSansBold18pt7b);
          JsonObject devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4097"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Out: ");
          ez.canvas.color(TFT_BLUE);
          //ez.canvas.print("D:");
          //ez.canvas.x(90);
          //ez.canvas.print(String(term));
     

          //ez.canvas.color(TFT_GREEN);
          //ez.canvas.print(" M:");
          ez.canvas.x(210);
          //ez.canvas.println(sens[0].data);
          ez.canvas.println(String(term));
       

          
          ez.canvas.font(&FreeSansBold12pt7b);
          devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4096"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Umyv:");
          ez.canvas.color(TFT_RED);
          ez.canvas.x(Pos1);
          ez.canvas.print(String(term));

          ez.canvas.color(TFT_BLACK);
          ez.canvas.x(Pos2-65);
          ez.canvas.print(sens[1].name);
          ez.canvas.color(TFT_BLUE);
          ez.canvas.x(Pos2);
          ez.canvas.println(sens[1].data);

          
          devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4099"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Andrey:");
          ez.canvas.color(TFT_RED);
          ez.canvas.x(Pos1);
          ez.canvas.print(String(term));

          ez.canvas.color(TFT_BLACK);
          ez.canvas.x(Pos2-60);
          ez.canvas.print(sens[2].name);
          ez.canvas.color(TFT_BLUE);
          ez.canvas.x(Pos2);
          ez.canvas.println(sens[2].data);


          devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4100"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Kuhnya: ");
          ez.canvas.color(TFT_RED);
          ez.canvas.x(Pos1);
          ez.canvas.print(String(term));

          ez.canvas.color(TFT_BLACK);
          ez.canvas.x(Pos2-60);
          ez.canvas.print(sens[3].name);
          ez.canvas.color(TFT_BLUE);
          ez.canvas.x(Pos2+15);
          ez.canvas.println(sens[3].data);


          devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4108"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Vanya: ");
          ez.canvas.color(TFT_RED);
          ez.canvas.x(Pos1);
          ez.canvas.print(String(term));

          ez.canvas.color(TFT_BLACK);
          ez.canvas.x(Pos2-60);
          ez.canvas.print(sens[4].name);
          ez.canvas.color(TFT_BLUE);
          ez.canvas.x(Pos2+15);
          ez.canvas.println(sens[4].data);

        
          devices_0_z3k = doc["devices"][0]["io"]["z3k-state"]["4109"];
          term = devices_0_z3k["curr_temp"];
          ez.canvas.color(TFT_BLACK);
          ez.canvas.print("Mama:      ");
          ez.canvas.color(TFT_RED);
          ez.canvas.x(Pos1);
          ez.canvas.print(String(term));

          ez.canvas.color(TFT_BLACK);
          ez.canvas.x(Pos2-60);
          ez.canvas.print(sens[5].name);
          ez.canvas.color(TFT_BLUE);
          ez.canvas.x(Pos2+15);
          ez.canvas.println(sens[5].data);
          
        
          
          

      }
  } else {
      USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


// MQTT!!!

void connectToMqtt() {
  USE_SERIAL.println("Connecting to MQTT...");
  mqttClient.connect();
}


void onMqttConnect(bool sessionPresent) {
  USE_SERIAL.println("Connected to MQTT.");
  USE_SERIAL.print("Session present: ");
  USE_SERIAL.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(mqtt_topic, 2);
  USE_SERIAL.print("Subscribing at QoS 2, packetId: ");
  USE_SERIAL.println(packetIdSub);
  //mqttClient.publish("test/lol", 0, true, "test 1");
  //USE_SERIAL.println("Publishing at QoS 0");
  //uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  //USE_SERIAL.print("Publishing at QoS 1, packetId: ");
  //USE_SERIAL.println(packetIdPub1);
  //uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  //USE_SERIAL.print("Publishing at QoS 2, packetId: ");
  //USE_SERIAL.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  USE_SERIAL.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  USE_SERIAL.println("Subscribe acknowledged.");
  USE_SERIAL.print("  packetId: ");
  USE_SERIAL.println(packetId);
  USE_SERIAL.print("  qos: ");
  USE_SERIAL.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  USE_SERIAL.println("Unsubscribe acknowledged.");
  USE_SERIAL.print("  packetId: ");
  USE_SERIAL.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  
  String data;
  String s(topic); 
  //USE_SERIAL.println(s);

  for(int t=0; t <6 ;t++) {
    if(s.indexOf(sens[t].Sens)>0) {

      for (int i = 0; i < len; i++) {
        data.concat((char)payload[i]);
      }
      s=sens[t].name + data;
      sens[t].data = data;
      USE_SERIAL.println(s);
    }
  }
}

void onMqttPublish(uint16_t packetId) {
  USE_SERIAL.println("Publish acknowledged.");
  USE_SERIAL.print("  packetId: ");
  USE_SERIAL.println(packetId);
}


