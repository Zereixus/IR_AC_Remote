//MASTER Sketch

// Load Wi-Fi library
#include <WiFi.h>

// Load Master-Slave library
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Include the contents of the User Interface Web page, stored in the same folder as the .ino file
#include "PageIndex.h" 

// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2  

//============================================================
// Wifi network credenials
const char *wifi_ssid = "Zereixus"; //--> wifi name
const char *wifi_password = "z4x2ds2z"; //--> wifi password

// Access Point Declaration and Configuration.
const char* soft_ap_ssid = "ESP32_WS";  //--> access point name
const char* soft_ap_password = "helloesp32WS"; //--> access point password

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
//============================================================

// ======================================== REPLACE WITH THE MAC ADDRESS OF YOUR SLAVES / RECEIVERS / ESP32 RECEIVERS.
uint8_t broadcastAddressSlave1[] = {0x78, 0x21, 0x84, 0x87, 0xAF, 0x88}; // Slave 1 MAC Address 78:21:84:87:AF:88
uint8_t broadcastAddressSlave2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Slave 2 MAC Address ---

//Master 78:21:84:79:EC:A4
//Slave 1 78:21:84:87:AF:88
// ========================================

//WEB PARAM
// ======================================== The variables used to check the parameters passed in the URL.
// Look in the "PageIndex.h" file.
// "set_LED?board="+board+"&gpio_output="+gpio+"&val="+value
// For example :
// set_LED?board=ESP32Slave1&gpio_output=13&val=1
// PARAM_INPUT_1 = ESP32Slave1
// PARAM_INPUT_2 = 13
// PARAM_INPUT_3 = 1
const char* PARAM_INPUT_1 = "board";
const char* PARAM_INPUT_2 = "gpio_output";
const char* PARAM_INPUT_3 = "val";
// ======================================== 


// ======================================== Structure to send data
// Send Data by struct
typedef struct struct_message_send {
  int send_GPIO_num;
  int send_Val;
} struct_message_send;

struct_message_send send_Data; //--> Create a struct_message to send data.
// ======================================== 

// Create a variable of type "esp_now_peer_info_t" to store information about the peer.
esp_now_peer_info_t peerInfo;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


//==================================== Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  digitalWrite(ON_Board_LED, HIGH); //--> Turn on ON_Board_LED.
  char macStr[18];
  Serial.print("\r\nPacket to:");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  digitalWrite(ON_Board_LED, LOW); //--> Turn off ON_Board_LED.
}
//=====================================

// ____________________________________ Subroutine to prepare data and send it to the Slaves.
void send_data_to_slave(int slave_number, int gpio_number, int value) {
  Serial.println();
  Serial.print(">>>>> ");
  Serial.println("Send data");

  send_Data.send_GPIO_num = gpio_number;
  send_Data.send_Val = value;

  esp_err_t result;

  // ::::::::::::::::: Sending data to All Slave
  if (slave_number == 0) {
    result = esp_now_send(0, (uint8_t *) &send_Data, sizeof(send_Data));
  }
  // :::::::::::::::::

  // ::::::::::::::::: Sending data to Slave 1
  if (slave_number == 1) {
    result = esp_now_send(broadcastAddressSlave1, (uint8_t *) &send_Data, sizeof(send_Data));
  }
  // :::::::::::::::::

  // ::::::::::::::::: Sending data to Slave 2
  // if (slave_number == 2) {
  //   result = esp_now_send(broadcastAddressSlave2, (uint8_t *) &send_Data, sizeof(send_Data));
  // }
  // :::::::::::::::::
  
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
// _____________________________________



//_____________________________ VOID SETUP()
void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led On Board


  // ---------------------------------------- Set Wifi to AP+STA mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : AP + STA");
  WiFi.mode(WIFI_AP_STA);
  Serial.println("-------------");
  // ----------------------------------------

  // ---------------------------------------- Access Point Settings.
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI AP");
  Serial.println("Setting up ESP32 to be an Access Point.");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  delay(1000);
  Serial.println("Setting up ESP32 softAPConfig.");
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("-------------");
  // ----------------------------------------

  //WIFI Connect
  connectToWiFi();
  // ----------------------------------------

  // ---------------------------------------- Init ESP-NOW
  Serial.println();
  Serial.println("-------------");
  Serial.println("Start initializing ESP-NOW...");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    Serial.println("Restart ESP32...");
    Serial.println("-------------");
    delay(1000);
    ESP.restart();
  }
  Serial.println("Initializing ESP-NOW was successful.");
  Serial.println("-------------");
  // ----------------------------------------

  // ---------------------------------------- Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  Serial.println();
  Serial.println("get the status of Trasnmitted packet");
  esp_now_register_send_cb(OnDataSent);
  // ---------------------------------------- 

  // ---------------------------------------- Register Peer
  registerPeer();
  // ---------------------------------------- 



  // ---------------------------------------- Handle Web Server
  Serial.println();
  Serial.println("Setting Up the Main Page on the Server.");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });
  // ----------------------------------------

  // ---------------------------------------- Send a GET request to <ESP_IP>/set_LED?board=<inputMessage1>&gpio_output=<inputMessage2>&val=<inputMessage3>
  server.on("/set_LED", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // :::::::::::::::::
    // GET input value on <ESP_IP>/set_LED?board=<inputMessage1>&gpio_output=<inputMessage2>&val=<inputMessage3>
    // PARAM_INPUT_1 = inputMessage1
    // PARAM_INPUT_2 = inputMessage2
    // PARAM_INPUT_3 = inputMessage3
    // Board = PARAM_INPUT_1
    // LED_gpio_num = PARAM_INPUT_2
    // LED_val = PARAM_INPUT_3
    // :::::::::::::::::

    String Board;
    String LED_gpio_num;
    String LED_val;
    
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3)) {
      Board = request->getParam(PARAM_INPUT_1)->value();
      LED_gpio_num = request->getParam(PARAM_INPUT_2)->value();
      LED_val = request->getParam(PARAM_INPUT_3)->value();

      String Rslt = "Board : " + Board + " || GPIO : " + LED_gpio_num + " || Set to :" + LED_val;
      Serial.println();
      Serial.println(Rslt);
      // Conditions for sending data to All Slave.
      if (Board == "ESP32AllSlave") send_data_to_slave(0, LED_gpio_num.toInt(), LED_val.toInt());
      // Conditions for sending data to Slave 1.
      if (Board == "ESP32Slave1") send_data_to_slave(1, LED_gpio_num.toInt(), LED_val.toInt());
      // Conditions for sending data to Slave 2.
      if (Board == "ESP32Slave2") send_data_to_slave(2, LED_gpio_num.toInt(), LED_val.toInt());
    }
    else {
      Board = "No message sent";
      LED_gpio_num = "No message sent";
      LED_val = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });
  // ---------------------------------------- 

  Serial.println();
  Serial.println("Starting the Server.");
  server.begin();

  Serial.println();
  Serial.println("------------");
  Serial.print("ESP32 IP address as soft AP : ");
  Serial.println(WiFi.softAPIP());
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("ESP32 IP address on the WiFi network (STA) : ");
    Serial.println(WiFi.localIP());
    Serial.print("Wi-Fi Channel : ");
    Serial.println(WiFi.channel());
  }
  Serial.println();
  // If your computer or mobile is connected to the access point of the ESP32 Master, then use the "soft AP" IP Address.
  // If your computer or phone is connected to a wifi router/hotspot, then use the "WiFi network (STA)" IP Address.
  Serial.println("Visit the IP Address above in your browser to open the main page.");
  Serial.println("You can choose and use one of the IPs above.");
  Serial.println("------------");
  Serial.println();
}

void connectToWiFi()
{
  Serial.println("------------");
  Serial.println("WIFI STA");
  Serial.print("Connecting to : ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  
  // ::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  
  int connecting_process_timed_out = 20; // 20 seconds
  bool wifiConnected = false;

  while (!wifiConnected && connecting_process_timed_out > 0) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
    } else {
      Serial.print(".");
      digitalWrite(ON_Board_LED, HIGH);
      delay(250);
      digitalWrite(ON_Board_LED, LOW);
      delay(250);
      connecting_process_timed_out--;
    }
  }

  if (wifiConnected) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(ON_Board_LED, LOW);
  } else {
    Serial.println("\nFailed to connect to Wi-Fi. Starting Access Point mode.");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    digitalWrite(ON_Board_LED, HIGH); // Turn on LED to indicate Access Point mode
  }

  Serial.println("------------");
  // :::::::::::::::::
}

void registerPeer()
{
  Serial.println();
  Serial.println("-------------");
  Serial.println("Register peer");
  peerInfo.encrypt = false;
  // ::::::::::::::::: register first peer
  Serial.println("Register first peer (ESP32 Slave 1)");
  memcpy(peerInfo.peer_addr, broadcastAddressSlave1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add first peer");
    return;
  }
  // :::::::::::::::::
  // ::::::::::::::::: register second peer
  Serial.println("Register second peer (ESP32 Slave 2)");
  memcpy(peerInfo.peer_addr, broadcastAddressSlave2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add second peer");
    return;
  }
  // :::::::::::::::::
  Serial.println("-------------");
}


void loop() {

}