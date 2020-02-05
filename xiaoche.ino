/*Code to transmit RF values to Arduino
 *
 * Pin Conections
 *  CE - 7
    MISO - 12
    MOSI - 11
    SCK - 13
    CS - 8
    A0 - JoyX
    A1 - JoyY
*/
#include <SPI.h>  
#include "RF24.h"
RF24 myRadio(7, 8);
struct package
{
    int msg = 0;
};
byte addresses[][6] = { "0" };
typedef struct package Package;
Package data;
void setup()
{
    Serial.begin(9600);
    myRadio.begin();
    myRadio.setChannel(115);  //115 band above WIFI signals
    myRadio.setPALevel(RF24_PA_MAX); //MAX power long rage
    myRadio.setDataRate(RF24_250KBPS);  //Minimum speed
    delay(500);
    Serial.print("Remote Initialized");
}
int forward;
int reverse;
int left;
int right;
void loop()
{
    int xValue = analogRead(A0); //Read JoyX value
    int yValue = analogRead(A1); //Read JoyY Value 
    //Serial.print(xValue); Serial.print(" , "); Serial.println(yValue);
    if (xValue > 560 && xValue < 1000) // Filter JoyX for up 
    {
        forward = map(xValue, 560, 1000, 1, 10); //Convert Joyx-up to 0-10
        //Serial.print("F="); Serial.println(forward);
        data.msg = forward; WriteData(); delay(50);
    }
    if (xValue < 500 && xValue > 10) // Filter JoyX for break 
    {
        reverse = map(xValue, 10, 500, 20, 11); //Convert JoyX-down to 11-20
        //Serial.print("B="); Serial.println(reverse);
        data.msg = reverse; WriteData(); delay(50);
    }
    else if (yValue > 600 && yValue < 1000) // Filter JoyY for right  
    {
        right = map(yValue, 600, 1000, 21, 30); //Convert JoyY-right to 21-30
        //Serial.print("R="); Serial.println(right);
        data.msg = right; WriteData(); delay(50);
    }
    else if (yValue < 450 && yValue > 10) // Filter JoyY for left 
    {
        left = map(yValue, 10, 450, 40, 31); //Convert JoyY-left to 31-40
        //Serial.print("L="); Serial.println(left);
        data.msg = left; WriteData(); delay(50);
    }
    /* else
     {
      Serial.println("Rest");
      data.msg = 0; WriteData(); delay(50);
     }
    */
}
void WriteData()
{
    myRadio.stopListening();  //Stop Receiving and start transminitng 
    myRadio.openWritingPipe(0xF0F0F0F0AA); //Sends data on this 40-bit address
    myRadio.write(&data, sizeof(data));
    //Serial.print("\nSent:");
    //Serial.println(data.msg);
    delay(50);
}
void ReadData()
{
    myRadio.openReadingPipe(1, 0xF0F0F0F066); // Which pipe to read, 40 bit Address
    myRadio.startListening(); //Stop Transminting and start Reveicing 
    if (myRadio.available())
    {
        while (myRadio.available())
        {
            myRadio.read(&data, sizeof(data));
        }
        Serial.print("\nReceived:");
        Serial.println(data.msg);
    }
}