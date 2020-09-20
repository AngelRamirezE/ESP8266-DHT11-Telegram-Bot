#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "";     // your network SSID (name)
char password[] = ""; // your network key

// Initialize Telegram BOT
#define BOTtoken ""  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

const int ledPin = 15; //D8
int ledStatus = 0;

int pinDHT11 = 4;
SimpleDHT11 dht11(pinDHT11);
//HC-SR04 definitions 
// defines pins numbers
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
long duration;
int distance;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 0;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff") {
      ledStatus = 1;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/distance") {
      
      bot.sendMessage(chat_id, getDistance(), "");
    }
    
    if (text == "/clima") {
      
      bot.sendMessage(chat_id, getClima(), "");
    }
    
    if (text == "/status") {
      if(ledStatus){
        bot.sendMessage(chat_id, "Led is OFF", "");
      } else {
        bot.sendMessage(chat_id, "Led is ON", "");
      }

      
    }

    if (text == "/start") {
      String welcome = "Welcome to this Arduino Telegram Bot made with love by: " + from_name + ".\n\n";
      welcome += "This are the available commands:\n";
      welcome += "/distance : to measure current distance between the robot and AngelPk\n";
      welcome += "/clima : to measure current temp and humidity of Angel's Room\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      welcome += "\n\n Beware the lag";
      
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off
}

void loop() {
  
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}

String getDistance(){
// Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);

      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin,HIGH,50000);

      // Calculating the distance
      distance= duration*0.034/2;

      String message ="";
      message += "Distancia: " + String(distance)+ " cm";
      return message;
}

String getClima(){
  byte temperature = 0;
  byte humidity = 0;
  int statusdht = dht11.read(&temperature, &humidity, NULL);

  if (statusdht == SimpleDHTErrSuccess){
    String message = "";
    message += "Temperatura: " + String(temperature)+ " °C \n";
    message += "Humedad: " + String(humidity) + "%";
    return message;
  }
  return "Error leyendo sensor";
}

