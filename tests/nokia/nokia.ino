#include <LCD5110_Graph.h>

// CLK – Pin 3
// Din – Pin 4
// DC – Pin 5
// CE – Pin 6
// RST – Pin 7

LCD5110 myGLCD(8,9,10,12,11); // 84x48 pixels
extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];
extern unsigned char SmallFont[];
extern unsigned char TinyFont[];
int contrast = 55;
int inv = 0;
void setup()
{

Serial.begin(9600);

myGLCD.InitLCD(contrast);
}

void loop()
{

myGLCD.setFont(SmallFont);
myGLCD.print("HetPro",CENTER,5);
myGLCD.print("TUTORIAL",14,20);
myGLCD.print("LCD NOKIA ",10,30);
myGLCD.print("5110 ",CENTER,40);
myGLCD.update();
delay(1000);
myGLCD.clrScr();
delay(1000);

myGLCD.setFont(TinyFont);
myGLCD.print("HETPRO",0,10);
myGLCD.print("hetpro",0,30);
myGLCD.setFont(SmallFont);
myGLCD.print("HETPRO",45,10);
myGLCD.print("hetpro",45,30);
myGLCD.update();
delay(1000);
myGLCD.clrScr();
myGLCD.setFont(BigNumbers);
myGLCD.print("123",0,0);
myGLCD.setFont(MediumNumbers);
myGLCD.print("123",40,30);
myGLCD.update();
delay(1000);
myGLCD.clrScr();
myGLCD.drawRoundRect(0,0,83,47);
myGLCD.drawLine(0,5,83,5);
myGLCD.drawLine(0,20,83,20);
myGLCD.drawLine(0,35,83,35);

myGLCD.update();

delay(1000);
myGLCD.clrScr();
myGLCD.drawCircle(42,24,10);
myGLCD.drawCircle(42,24,20);
myGLCD.update();
delay(1000);
myGLCD.clrScr();
myGLCD.setFont(SmallFont);
myGLCD.print("VISITA",CENTER,10);
myGLCD.setFont(TinyFont);
myGLCD.print("hetpro-store.com",10,30);
myGLCD.update();
delay(2000);
myGLCD.clrScr();

inv=inv+1;
if(inv==1)
{
myGLCD.invert(true);
myGLCD.update();

}
if(inv==2)
{
myGLCD.invert(false);
myGLCD.update();
inv=0;
}
myGLCD.clrScr();

}
