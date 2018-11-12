#include "TM1637.h"
#include <TimeLib.h>

//https://github.com/PaulStoffregen/Time

//pins definition

#define CLK 4
#define DIO 5
#define upside 12
#define buzzer 8

#define loop_for_wifi 100

TM1637 led(4,5);

int reveil_jour;
int reveil_heure;
int reveil_minute;

int alarm_status = 0; //0 not armed, 1 armed, 2 ringing

int cpt = loop_for_wifi;

bool upsidedown;
bool upsidedown_previous;

void setup() {
  Serial.begin(115200); //connect to the ESP via serial port

  pinMode(upside, INPUT_PULLUP); //using internet pullup resistor
  pinMode(buzzer, OUTPUT); //enabling the buzzer
  
  setTime(00,00,00 ,25,01,2015); //by default set time as 00:00:00
  led.set(7);
  led.point(false);
  show(88,88,true);

  //two beeps to say hello
  pinMode(buzzer, OUTPUT);
  tone(buzzer, 440);
  delay(150);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 440);
  delay(150);
  noTone(buzzer);
  pinMode(buzzer, INPUT);
  
}

void loop() {

  //looking if in alarm mode
  if(reveil_jour == day() && reveil_heure == hour() && reveil_minute == minute() && alarm_status == 1)
  {
    alarm_status = 2;
  }

  //reading upside down 
  upsidedown = digitalRead(upside);
  if(upsidedown != upsidedown_previous)
  {
    if(alarm_status == 1) //there is an alarm scheduled
    {
      show(reveil_heure,reveil_minute,upsidedown); //show the alarm for half a second
      delay(500);
    }
    else if(alarm_status == 2)
    {
      alarm_status = 0; //it was ringing now it is over
      led.point(false);

      //two beeps to say alarm is over
      pinMode(buzzer, OUTPUT);
      tone(buzzer, 440);
      delay(150);
      noTone(buzzer);
      delay(100);
      tone(buzzer, 480);
      delay(150);
      noTone(buzzer);
       pinMode(buzzer, INPUT);
    }
  }


  if(alarm_status == 2) //IT'S RINGING MEN !!
  {
    led.set(7); //full brightness
    if(cpt%2 == 0) //even
    {
      pinMode(buzzer, OUTPUT);
      tone(buzzer, 340);
    }
    else //odd
    {
      
      noTone(buzzer);
      pinMode(buzzer, INPUT);
    }
    delay(200);
  }
  else //handling brightness
  {
     if(hour() > 22 || hour() < 11)
     {
      led.set(0);
     }
     else
     {
      led.set(5);
     }
  }

  
  if(cpt >= loop_for_wifi || alarm_status == 2) 
  {
    while(Serial.available()) //empty the buffer
    {
      Serial.read();
    }
    
    Serial.print(1); //send one char to esp
    if(Serial.find("GO"))
    {
                   int heures = Serial.parseInt();
                   int minutes = Serial.parseInt();
                   int secondes = Serial.parseInt();
                   
                   int jour = Serial.parseInt();
                   int mois = Serial.parseInt();
                   int annee = Serial.parseInt();
  
                   reveil_jour = Serial.parseInt();
                   reveil_heure = Serial.parseInt();
                   reveil_minute = Serial.parseInt();
  
  
                   setTime(heures,minutes,secondes,jour,mois,annee);

                   if(reveil_jour == 42)
                   {
                     alarm_status = 0; //if day is 42 no alarm scheduled
                     led.point(false);
                   }
                   else if(reveil_jour == day())
                   {
                        if(reveil_jour == day() && reveil_heure == hour() && reveil_minute == minute() && alarm_status == 1)
                        {
                          alarm_status = 2;
                        } 
                        else if( (reveil_heure*60 + reveil_minute) > (hour()*60 + minute()) )
                        {
                          alarm_status = 1; //it is going to ring later to day so alarm is armed
                          led.point(true);
                        }
                        else if(alarm_status !=2) //if not ringing
                        {
                          alarm_status = 0;
                          led.point(false);
                        }
                   }
                   else if(reveil_jour == day(now() + 86400))
                   {
                      alarm_status = 1;
                      led.point(true);
                   }
                   else if(alarm_status != 2) //if not ringing
                   {
                      alarm_status = 0;
                      led.point(false);
                   }

                   //led.point(true);
    }  
    else
    {
      //if no go message = no Wifi
      //led.point(false);
    }

    if(cpt > loop_for_wifi)
    {
      cpt = 0;//reset CPT only too high
    }
  }


  show(hour(),minute(),upsidedown);

    upsidedown_previous = upsidedown; //updating upsidedown_previous for next loop

    cpt++;
}

void show(int left, int right, bool upsidedown)
{
  if(!upsidedown) //showing normally
  {
    led.display(0, left/10);
    led.display(1, left%10);
    led.display(2, right/10);
    led.display(3, right%10);
  }
  else //showing upside down
  {
    led.display(3, left/10 + 10); //needs modified library to work
    led.display(2, left%10 + 10);
    led.display(1, right/10 + 10);
    led.display(0, right%10 + 10);
  }


  
}


