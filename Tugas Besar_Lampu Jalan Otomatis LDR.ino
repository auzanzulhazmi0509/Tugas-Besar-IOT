#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int ldr = A0;

const char* ssid = "Fast";
const char* password = "lasagna enak";

const char* mqtt_server = "152.67.110.130";

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }  
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length){
  String string;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
    string+=((char)payload[i]);
  }
  Serial.println(string);

  if((char)payload[0]== '0'){
    Serial.println("LAMPU MATI");
    digitalWrite(D5, LOW);
    
  }

  else if((char)payload[0] == '1'){
    Serial.println("LAMPU NYALA");
    digitalWrite(D5, HIGH);
  }
}

void reconnect(){
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")){
      Serial.println("connected");
      client.subscribe("lampu");
    }else{
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(3000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }

  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 3000){
    lastMeasure = now;
    int nilai = analogRead(ldr);

    static char itensitasCahaya[7];
    dtostrf(nilai,6,2,itensitasCahaya);

    client.publish("cahaya", itensitasCahaya);


      Serial.print("Cahaya = ");
      Serial.print(nilai);
      Serial.println("V");
    }

}