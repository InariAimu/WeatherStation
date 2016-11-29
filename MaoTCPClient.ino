

#include <ESP8266WiFi.h>
#include <Wire.h>

const char* ssid     = "yourssid";
const char* password = "yourpassword";

const char* device_id = "yourdeviceid";
const char* api_id = "yourapiid";
const char* host = "www.bigiot.net";
//const char* host = "192.168.1.100";

WiFiClient client;
int value = 0;

void WiFiEvent(WiFiEvent_t event) {
    //Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("WiFi lost connection");
            delay(10000);
            initWifi();
            break;
    }
}

void initWifi()
{
  // delete old config
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(WiFiEvent);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  /*Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/
}

bool initBigiot()
{
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  const int httpPort = 8181;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout");
      client.stop();
      return false;
    }
  }
  while(client.available()){
    String line = client.readStringUntil('\n');
    Serial.print(line);
  }
  Serial.println("connected");

  delay(1000*2);
  
  Serial.println("logging");
  // This will send the request to the server
  client.println(String("{\"M\":\"checkin\",\"ID\":\"")+device_id+"\","+
               "\"K\":\""+api_id+"\"}");
  client.flush();
  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout or already logged!!");
      client.stop();
      delay(1000*30);
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\n');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("check in OK");
  delay(5000);
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  Wire.begin();        // join i2c bus (address optional for master)

  initWifi();
  
  int retry=0;
  while(!initBigiot())
  {
    delay(10000);
    Serial.print("retry times:");
    Serial.println(++retry);
  }
}

byte      ws_device_id;
byte      packet_id;
int16_t   battery_value;
int16_t   rain_value;
float     dust_volt;
uint16_t  brightness_val;
float     pressure;
float     temp;
byte      dht11_hu;

int16_t   last_battery_value;
int16_t   last_rain_value;
float     last_dust_volt;
uint16_t  last_brightness_val;
float     last_pressure;
float     last_temp;
byte      last_dht11_hu;

bool getRawdataFromI2C()
{
  byte data[32];

  //get raw data via I2C
  Wire.requestFrom(8, 32); 
  int cs=0;
  while (Wire.available()) {
    data[cs] = Wire.read();
    cs++;
  }
  
  ws_device_id = data[0];
  packet_id = data[1];

  battery_value = *(int16_t*)&data[2];
  if(battery_value>=1024)return false;

  rain_value = *(int16_t*)&data[4];
  if(rain_value>=1024)return false;

  dust_volt = *(float*)&data[6];
  if(dust_volt>5)return false;

  brightness_val = *(uint16_t*)&data[10];
  if(brightness_val>=65535)return false;
  
  pressure = *(float*)&data[12];
  temp = *(float*)&data[16];

  dht11_hu = data[20];
  if(dht11_hu>100)return false;

  last_battery_value = battery_value;
  last_rain_value = rain_value;
  last_dust_volt = dust_volt;
  last_brightness_val = brightness_val;
  last_pressure = pressure;
  last_temp = temp;
  last_dht11_hu = dht11_hu;
  return true;
}

void loop() 
{
  bool validData = getRawdataFromI2C();

  if(validData == true)
  {
    
  }
  else
  {
    battery_value = last_battery_value;
    rain_value = last_rain_value;
    dust_volt = last_dust_volt;
    brightness_val = last_brightness_val;
    pressure = last_pressure;
    temp = last_temp;
    dht11_hu = last_dht11_hu;
  }

  //deal with raw data
  float batteryVoltage = (float)battery_value * 5 / 1024;
  int16_t rain_index = 1023 - rain_value;
  
  delay(100);

  if(!client.connected())
  {
    Serial.println("lost connection, reconnecting...");
    int retry=0;
    while(!initBigiot())
    {
      delay(10000);
      Serial.print("retry times:");
      Serial.println(++retry);
    }
  }
  
  client.println(String("{\"M\":\"update\",\"ID\":\"")+device_id+"\","+
               "\"V\":{"+
               "\"1017\":\""+String(pressure,2)+"\","+
               "\"904\":\""+String(temp,2)+"\","+
               "\"1018\":\""+brightness_val+"\","+
               "\"1019\":\""+dht11_hu+"\","+
               "\"1022\":\""+rain_index+"\","+
               "\"1024\":\""+getDustDensity(dust_volt)+"\","+
               "\"1025\":\""+batteryVoltage+"\""+
               "}}");
  client.flush();
  delay(10);
  ++value;
  Serial.print(value);
  Serial.println(" data sent");

  unsigned long startTime = millis();
  while(millis() - startTime < 40000){
    String line;
    if(client.available())
    {
      while(client.available()){
        line = client.readStringUntil('\n');
        Serial.print(line);
      }
      Serial.println();
    }
  }
  
}

float getDustDensity(float outputV) {
  //输出电压和灰尘密度换算公式: ug/m3 = (V - 0.9) / 5 * 1000
  float ugm3 = (outputV - 0.9) / 5 * 1000;
  //去除检测不到的范围
  if (ugm3 < 0) {
    ugm3 = 0;
  }
  return ugm3;
}
