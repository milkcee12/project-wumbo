#include <WiFi.h>
#include <PubSubClient.h>
#include <DacESP32.h>

// WiFi
const char *ssid = "Galaxy S20 FE 5G5A0C"; // Enter your Wi-Fi name
const char *password = "emog4158";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// Joystick stuff
const int joystickVRX = 34;  
const int joystickVRY = 35; 
const char *joystickTopic = "wumbo/joystick"; 
int xValue = 0;
int joystickState = 0, joystickStateOld = 0;

// Button stuff
const int button1 = 14, button2 = 27, button3 = 33, button4 = 25;
const int NUM_BUTTONS = 4;
int buttonValue[NUM_BUTTONS];
int buttonFreq[NUM_BUTTONS+1];
const char *buttonTopic = "wumbo/buttons";
std::string buttonStateOld = "", buttonState = "";

// Speaker stuff
const int speaker = 32;
int speakerOutput = 0;
hw_timer_t *My_timer = NULL;
int timerCount = 0;



WiFiClient espClient;
PubSubClient client(espClient);


void IRAM_ATTR onTimer();

void setup() {

    buttonFreq[0] = 440;
    buttonFreq[1] = 349;
    buttonFreq[2] = 392;
    buttonFreq[3] = 440;
    buttonFreq[4] = 440;

    // I/O
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(speaker, OUTPUT);

    // Interrupts
    My_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(My_timer, &onTimer, true);

    // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    client.publish(topic, "Hi, I'm ESP32 ^^");
    client.subscribe(topic);
    client.subscribe(joystickTopic);
    client.subscribe(buttonTopic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    xValue = analogRead(joystickVRX);
    if(xValue < 1000){
        joystickState = -1;
    } else if(xValue > 4000){
        joystickState = 1;
    } else {
        joystickState = 0;
    }

    buttonValue[0] = digitalRead(button1);
    buttonValue[1] = digitalRead(button2);
    buttonValue[2] = digitalRead(button3);
    buttonValue[3] = digitalRead(button4);
    buttonState = ""; //"std::to_string(button1Value) + std::to_string(button2Value)";
    for(int i = 0; i < 4; i++){
        buttonState += std::to_string(buttonValue[i]);
    }
    if(buttonState != buttonStateOld){
        // TODO: change pitch if new button pressed while playing
        buttonStateOld = buttonState;
        unsigned int len = buttonState.length();
        client.publish(buttonTopic, buttonState.c_str(), len);
    }

    if(joystickStateOld == 0 && joystickState != 0){
        client.publish(joystickTopic, "Hi, string was plucked ^^");
        Serial.println("String plucked");
        int count = 0;
        if(buttonValue[3] == 0){
            count = 500000/buttonFreq[3]; 
            timerCount = buttonFreq[3];
        } else if(buttonValue[2] == 0){
            count = 500000/buttonFreq[2]; 
            timerCount = buttonFreq[2];
        } else if(buttonValue[1] == 0){
            count = 500000/buttonFreq[1]; 
            timerCount = buttonFreq[1];
        } else if(buttonValue[0] == 0){
            count = 500000/buttonFreq[0]; 
            timerCount = buttonFreq[0];
        } else {
            count = 500000/buttonFreq[4]; 
            timerCount = buttonFreq[4];
        }
        timerCount /= 5;
        timerAlarmWrite(My_timer, count, true);
        timerAlarmEnable(My_timer);
        timerStart(My_timer);
    }

    joystickStateOld = joystickState;

    client.loop();
}

void IRAM_ATTR onTimer(){
    // Play note
    speakerOutput = 1 - speakerOutput;
    digitalWrite(speaker, speakerOutput);
    timerCount--;
    if(timerCount == 0){
      timerStop(My_timer);
    }
}