

#include "LowPower.h"


#define PIN_BatteryVoltage A0


#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;


#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 9


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


// GY-30
// BH1750FVI
// in ADDR 'L' mode 7bit addr
#define ADDR 0b0100011
// addr 'H' mode
// #define ADDR 0b1011100


#define PIN_RAIN_SENSOR A3


#define PIN_DATA_OUT A1 //连接空气质量传感器模拟量输出的IO口
#define PIN_LED_VCC 5 //空气质量传感器中为内部Led供电的引脚
const int DELAY_BEFORE_SAMPLING = 280; //采样前等待时间
const int DELAY_AFTER_SAMPLING = 40; //采样后等待时间
const int DELAY_LED_OFF = 9680; //间隔时间


//-------------------------------------------------------------------------------------------------------------------

float getSharpDustSensorVoltage()
{
  digitalWrite(PIN_LED_VCC, LOW);
  delayMicroseconds(DELAY_BEFORE_SAMPLING);
  float analogOutput = analogRead(PIN_DATA_OUT);
  delayMicroseconds(DELAY_AFTER_SAMPLING);
  digitalWrite(PIN_LED_VCC, HIGH);
  delayMicroseconds(DELAY_LED_OFF);
  //Arduino模拟量读取值的范围为0~1023,以下换算为0~5v
  float outputV = analogOutput / 1024 * 5;
  return outputV;
}

float getDS18B20Temp()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);  
}

unsigned long getGY30Reading()
{
  unsigned long val = 0;
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000111);
  Wire.endTransmission();
  delay(100);
    
  Wire.beginTransmission(ADDR);
  Wire.write(0b00100000);
  Wire.endTransmission();
  // typical read delay 120ms
  delay(120);

  Wire.requestFrom(ADDR, 2); // 2byte every time

  for (val=0; Wire.available()>=1; ) {
    char c = Wire.read();
    //Serial3.println(c, HEX);
    val = (val << 8) + (c & 0xFF);
  }   
  val = val / 1.2;
  return val;
}

byte getDHT11Humidity()
{
  //int chk = DHT11.read(DHT11PIN);
  DHT11.read(DHT11PIN);
  return (byte)DHT11.humidity;
}

bool getBMP180Readings(float* p, float* temp)
{
  char stat;
  double T,P;
  stat = pressure.startTemperature();
  if (stat != 0)
  {
    delay(stat);
    stat = pressure.getTemperature(T);
    if (stat != 0)
    {
      //Serial.print("temperature: ");
      //Serial.print(T,2);
      //Serial.print(" deg C, ");
      stat = pressure.startPressure(3);
      if (stat != 0)
      {
        delay(stat);
        stat = pressure.getPressure(P,T);
        /*if (stat != 0)
        {
          p0=1000;
          a = pressure.altitude(P,p0);
        }*/
        *p = (float)P;
        *temp = (float)T;
        return true;
      }
    }
    else return false;
  }else return false;
  return false;
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  
  pinMode(PIN_DATA_OUT, INPUT); //定义为输入(ADC读取模拟量)
  pinMode(PIN_LED_VCC, OUTPUT); //定义为输出
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.cePin = 8;
  Mirf.csnPin = 7;
  Mirf.init();
  Mirf.setRADDR((byte *)"ws_01");
  Mirf.payload = 32;
  Mirf.config();
  Mirf.configRegister(EN_AA,0);
  delay(100);
  
  pressure.begin();
  delay(100);

  sensors.begin();
  delay(100);
  
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000001);
  Wire.endTransmission();
  delay(100);
}

const byte device_id = 1;
byte packet_id = 0;

void loop() {
  // put your main code here, to run repeatedly:
  char data[Mirf.payload];

  data[0] = device_id;
  data[1] = packet_id;
  packet_id++;

  int battery_value = analogRead(PIN_BatteryVoltage);
  data[2] = *(((byte *)&battery_value)+0);
  data[3] = *(((byte *)&battery_value)+1);

  int rain_value = analogRead(PIN_RAIN_SENSOR);
  data[4] = *(((byte *)&rain_value)+0);
  data[5] = *(((byte *)&rain_value)+1);

  float dust_volt =  getSharpDustSensorVoltage();
  data[6] = *(((byte *)&dust_volt)+0);
  data[7] = *(((byte *)&dust_volt)+1);
  data[8] = *(((byte *)&dust_volt)+2);
  data[9] = *(((byte *)&dust_volt)+3);

  unsigned int brightness_val = (unsigned int)getGY30Reading();
  data[10] = *(((byte *)&brightness_val)+0);
  data[11] = *(((byte *)&brightness_val)+1);

  float pres,temp;
  if(!getBMP180Readings(&pres, &temp))
  {
    pres = 0;
    temp = getDS18B20Temp();
  }
  data[12] = *(((byte *)&pres)+0);
  data[13] = *(((byte *)&pres)+1);
  data[14] = *(((byte *)&pres)+2);
  data[15] = *(((byte *)&pres)+3);
  data[16] = *(((byte *)&temp)+0);
  data[17] = *(((byte *)&temp)+1);
  data[18] = *(((byte *)&temp)+2);
  data[19] = *(((byte *)&temp)+3);

  data[20] = getDHT11Humidity();

  for(int i=21;i<32;i++)
  {
    data[i] = 0;
  }
  
  Mirf.setTADDR((byte *)"iotc1");
  Mirf.send(data);
  while(Mirf.isSending());
  Mirf.powerDown();
  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
