

#include <Wire.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A0, A2, 5, 4, 3, 2);
unsigned long pcount=0;
int pecount=0;

byte data[32];
  
void setup() {
  //Serial.begin(9600);
  //Serial.println( "Starting wireless..." );
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  
  pinMode(A1,OUTPUT);
  digitalWrite(A1,LOW);
  digitalWrite(13,LOW);
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("NRF24L01 Init..");

  // Setup
  Mirf.spi = &MirfHardwareSpi;
  Mirf.cePin = A3;
  Mirf.csnPin = 10;
  Mirf.init();
  Mirf.setRADDR((byte *)"iotc1");
  Mirf.payload = 32;
  Mirf.config();

  Mirf.configRegister(EN_AA,0);

  /*
  // Read and print RF_SETUP
  byte rf_setup = 0;
  Mirf.readRegister( RF_SETUP, &rf_setup, sizeof(rf_setup) );
  Serial.print( "rf_setup = " );
  Serial.println( rf_setup, BIN );
  Serial.println( "Wireless initialized!" );*/

  /*byte x=0x61;
  Mirf.writeRegister(RX_ADDR_P0, &x, sizeof(x));
  delay(10);
  Mirf.readRegister(RX_ADDR_P0, &x, sizeof(x));
  Serial.println(x);*/
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Ready           ");
}

void loop()
{
  
  if(!Mirf.isSending() && Mirf.dataReady())
  {
    Mirf.getData(data);

    byte device_id = data[0];
    byte packet_id = data[1];

    int battery_value = *(int*)&data[2];

    int rain_value = *(int*)&data[4];

    double dust_volt = *(double*)&data[6];

    unsigned int brightness_val = *(unsigned int*)&data[10];

    double pres = *(double*)&data[12];
    double temp = *(double*)&data[16];

    byte dht11_hu = data[20];


    double batteryVoltage = (double)battery_value*5/1024;

    lcd.setCursor(0, 0);
    lcd.print(packet_id);
    lcd.print(' ');
    lcd.print(batteryVoltage);
    lcd.print('V');
    lcd.print(' ');
    lcd.print(temp);
    lcd.print(' ');
    lcd.print(' ');

    lcd.setCursor(0, 1);
    lcd.print(pres);
    lcd.print(' ');
    lcd.print(dht11_hu);
    lcd.print(' ');
    lcd.print((int)getDustDensity(dust_volt));
    lcd.print(' ');
    lcd.print(brightness_val);
    lcd.print(' ');
    lcd.print(' ');
  }

}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  for(int i=0;i<32;i++)
  {
    Wire.write(data[i]); // respond with message of 6 bytes
  }
  // as expected by master
}

double getDustDensity(double outputV) {
  //输出电压和灰尘密度换算公式: ug/m3 = (V - 0.9) / 5 * 1000
  double ugm3 = (outputV - 0.9) / 5 * 1000;
  //去除检测不到的范围
  if (ugm3 < 0) {
    ugm3 = 0;
  }
  return ugm3;
}

