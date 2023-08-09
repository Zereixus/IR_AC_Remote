// Slave Sketch

// IR Remote library
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>

// IR Protocol library
#include <IRac.h>

// Load Wi-Fi library
#include <WiFi.h>

// Load Master-Slave library
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

//I2C LCD
#include <LiquidCrystal_I2C.h>
int totalColumns = 16;
int totalRows = 2;
LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);  

// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2

// Defines GPIO 4 as LED_1.
#define LED_1 4
//const int LED_1 = 4;

//IR Led position GPIO 15
const uint16_t kIrLed = 15;


// Protocol of IR Transmitter
IRac ac(kIrLed);

// Insert your SSID (The SSID of the "ESP32 Master" access point.).
constexpr char WIFI_SSID[] = "ESP32_WS";

// ======================================== Variables for PWM settings.
const int PWM_freq = 5000;
const int PWM_Channel = 0;
const int PWM_resolution = 8;
// ========================================

//========================================= Variables for AC Default Setup
float degrees = 16;

// ======================================== Variable for millis / timer.
unsigned long previousMillisScan = 0;

unsigned long intervalScanMinutes = 60;  // Interval pemindaian dalam menit
const unsigned long intervalScanMillis = intervalScanMinutes * 60 * 1000;  // Konversi menit ke milidetik
// ======================================== 

// ======================================== Indicator Value
String powerState = "OFF";
String tempState = String(degrees, 1) + "\u00B0";
String timerState = "Timer not Set";

//=========================================


// ======================================== Structure example to receive data.
// Must match the sender structure
typedef struct struct_message_receive {
  int receive_GPIO_num;
  int receive_Val;
} struct_message_receive;

// Create a struct_message to receive data.
struct_message_receive receive_Data;
// ========================================

// ________________________________________________________________________________ Callback when data is received.
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  digitalWrite(ON_Board_LED, HIGH); //--> Turn on ON_Board_LED.

  // ---------------------------------------- Get the MAC ADDRESS of the sender / slave.
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // ---------------------------------------- 

  memcpy(&receive_Data, incomingData, sizeof(receive_Data)); //--> Copy the information in the "incomingData" variable into the "receive_Data" structure variable.

  // ---------------------------------------- Prints the MAC ADDRESS of sender / Master and Bytes received.
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Packet received from: ");
  Serial.println(macStr);
  Serial.print("Bytes received: ");
  Serial.println(len);
  // ---------------------------------------- 
  
  
  // ---------------------------------------- Conditions for controlling the IR LEDs.
  int indicator = receive_Data.receive_Val;  
  if (receive_Data.receive_GPIO_num == 1) {
    if (indicator != 0){
      digitalWrite(LED_1, HIGH);
      ac.next.power = true;
      powerState = "ON";

    }else{
      digitalWrite(LED_1, LOW);
      ac.next.power = false;
      powerState = "OFF";
    }
    lcd.clear();
    lcd.print("Turning On/Off");
    
    ac_command();
  }

  if (receive_Data.receive_GPIO_num == 2) {
    if (indicator == 1 && degrees<32){
      degrees++;
      ac.next.degrees = degrees;
    }else if (indicator == 0 && degrees>16){
      degrees--;
      ac.next.degrees = degrees;
    }else if (indicator > 1){
      degrees = indicator;
      ac.next.degrees = degrees;
    }
    tempState = String(degrees, 1) + "\u00B0";
    lcd.clear();
    lcd.print("Setting Temp");
    ac_command();
  }

  if (receive_Data.receive_GPIO_num == 3) { //not all supported
    if (indicator == 2){
      ac.next.fanspeed = stdAc::fanspeed_t::kLow;
    }
    else if (indicator == 3){
      ac.next.fanspeed = stdAc::fanspeed_t::kMedium;
    }
    else if (indicator == 4){
      ac.next.fanspeed = stdAc::fanspeed_t::kHigh;
    }
    else{
      ac.next.fanspeed = stdAc::fanspeed_t::kAuto;
    }
    lcd.clear();
    lcd.print("Setting FanMode");
    ac_command();
  }

  if (receive_Data.receive_GPIO_num == 15) {
    // Not impemented Timer
  }

  // ---------------------------------------- 

  // ---------------------------------------- Prints all data received from the sender / Master.
  Serial.println("Receive Data: ");
  Serial.print("Code Activation: ");
  Serial.println(receive_Data.receive_GPIO_num);
  Serial.print("Code Value: ");
  Serial.println(receive_Data.receive_Val);
  Serial.println("<<<<<");
  // ---------------------------------------- 
  digitalWrite(ON_Board_LED, LOW); //--> Turn off ON_Board_LED.
}

// ________________________________________________________________________________ Function to scan your network and get the channel.
// - In this project, the "ESP32 Master" and the "ESP32 Slaves" must use the same Wi-Fi channel.
// - When the "ESP32 Master" is connected to the Wi-Fi router, the ESP32 Master's Wi-Fi channel is assigned automatically by the Wi-Fi router.
// - Wi-Fi routers can change channels due to certain conditions,
//   one of which is if there are many Wi-Fi routers or access points that are close together in one area and use the same channel.
// - Therefore, this function is used so that the "ESP32 Slaves" can check the channel that the "ESP32 Master" uses, 
//   so that the "ESP32 Slaves" and the "ESP32 Master" use the same Wi-Fi channel.
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}
// ________________________________________________________________________________

// ________________________________________________________________________________ scan_and_set_Wifi_Channel()
void scan_and_set_Wifi_Channel() {
  // "ESP32 Master" and "ESP32 Slaves" must use the same Wi-Fi channel.

  Serial.println();
  Serial.println("-------------");
  Serial.print("Scanning SSID : ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");
  
  // Get the Wi-Fi channel "ESP32 Master".
  int32_t channel = getWiFiChannel(WIFI_SSID);

  // Get the Wi-Fi channel on this device (ESP32 Slave).
  int cur_WIFIchannel = WiFi.channel();

  if (channel == 0) {
    Serial.print("SSID : ");
    Serial.print(WIFI_SSID);
    Serial.println(" not found !");
    Serial.println();
  } else {
    Serial.print("SSID : ");
    Serial.print(WIFI_SSID);
    Serial.print(" found. (Channel : ");
    Serial.print(channel);
    Serial.println(")");
    
    // If the "ESP32 Slave" Wi-Fi channel is different from the "ESP32 Master" Wi-Fi channel, 
    // then the "ESP32 Slave" Wi-Fi channel is set to use the same channel as the "ESP32 Master" Wi-Fi channel.
    if (cur_WIFIchannel != channel) {
      Serial.println("Set Wi-Fi channel...");
      Serial.println();
      esp_wifi_set_promiscuous(true);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      esp_wifi_set_promiscuous(false);
    }
  }

  Serial.print("Wi-Fi channel : ");
  Serial.println(WiFi.channel());
  Serial.println("-------------");
}
// ________________________________________________________________________________


// ________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();

  // Initialize all AC objects
  ac_setup();


  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led On Board

  
  pinMode(LED_1,OUTPUT); //--> LED_1 port Direction output

  //pinMode(LED_2,OUTPUT); //--> LED_2 port Direction output
  
  
  // ---------------------------------------- Set Wi-Fi as Wifi Station Mode.
  WiFi.mode(WIFI_STA);
  // ----------------------------------------

  // ---------------------------------------- Scan & Set the Wi-Fi channel.
  scan_and_set_Wifi_Channel();
  // ---------------------------------------- 

  // ---------------------------------------- Init ESP-NOW
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

  // ---------------------------------------- Register for a callback function that will be called when data is received
  Serial.println();
  Serial.println("Register for a callback function that will be called when data is received");
  esp_now_register_recv_cb(OnDataRecv);
  // ----------------------------------------

  // initialize LCD
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.print("Wait Callback");
  delay(1000);

  lcd.clear();
  lcd.print(powerState);
  lcd.setCursor(12,0);
  lcd.print(tempState);
  lcd.setCursor(0,1);
  lcd.print(timerState);
}
// ________________________________________________________________________________

// ________________________________________________________________________________ VOID LOOP()
void loop() {
  // ---------------------------------------- Timer or millis() to scan access point "ESP32 Master" every 60 seconds (see "intervalScan" variable).
  // This process is to recheck the "ESP32 Master" Wi-Fi channel.
  // So if the Wi-Fi channel "ESP32 Master" changes, then this process will reset the Wi-Fi channel "ESP32 Slave" to use the same channel as the Wi-Fi channel "ESP32 Master".
  
  unsigned long currentMillisScan = millis();
  if (currentMillisScan - previousMillisScan >= intervalScanMillis) {
    previousMillisScan = currentMillisScan;

    scan_and_set_Wifi_Channel();
  }
  // ---------------------------------------- 
}
// ________________________________________________________________________________

void ac_setup(){
  // Set up what we want to send.
  // See state_t, opmode_t, fanspeed_t, swingv_t, & swingh_t in IRsend.h for
  // all the various options.
  ac.next.protocol = decode_type_t::SHARP_AC;  // Set a protocol to use.
  ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
  ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
  ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = degrees;  // 16 degrees.
  ac.next.fanspeed = stdAc::fanspeed_t::kHigh;  // Start the fan at High.
  ac.next.swingv = stdAc::swingv_t::kOff;  // Don't swing the fan up or down.
  ac.next.swingh = stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
  ac.next.light = false;  // Turn off any LED/Lights/Display that we can.
  ac.next.beep = false;  // Turn off any beep from the A/C if we can.
  ac.next.econo = false;  // Turn off any economy modes if we can.
  ac.next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
  ac.next.turbo = false;  // Don't use any turbo/powerful/etc modes.
  ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
  ac.next.sleep = -1;  // Don't set any sleep time or modes.
  ac.next.clean = false;  // Turn off any Cleaning options if we can.
  ac.next.clock = -1;  // Don't set any current time if we can avoid it.
  ac.next.power = false;  // Initially start with the unit off.
}


void ac_command() {
  // For every protocol the library has ...
  Serial.println("Sending a message to the A/C unit.");
  ac.sendAc();  // Have the IRac class create and send a message.
  delay(100);  // Wait 0.1 seconds.

  // for (int i = 1; i < kLastDecodeType; i++) {
  //   decode_type_t protocol = (decode_type_t)i;
  //   // If the protocol is supported by the IRac class ...
  //   if (ac.isProtocolSupported(protocol)) {
  //     Serial.println("Protocol " + String(protocol) + " / " +
  //                    typeToString(protocol) + " is supported.");
  //     ac.next.protocol = protocol;  // Change the protocol used.
  //     Serial.println("Sending a message to the A/C unit.");
  //     lcd.clear();
  //     lcd.print("Sending");
  //     lcd.setCursor(0,1);
  //     lcd.print(typeToString(protocol));
  //     ac.sendAc();  // Have the IRac class create and send a message.
  //     delay(1000);  // Wait 1 seconds.
  //   }
  // }
  

  lcd.clear();
  lcd.print(powerState);
  lcd.setCursor(12,0);
  lcd.print(tempState);
  lcd.setCursor(0,1);
  lcd.print(timerState);

  
  //Serial.println("Already Checking all Protocol");
}
