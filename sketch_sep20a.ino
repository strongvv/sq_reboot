#include <SPI.h>
#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <SD.h>

#define ON 0
#define OFF 1
#define PRESS_TIME 1
#define ETH_SS 53
#define SD_SS 4


byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Мак адрес
byte ip[] = { 
  192, 168, 1, 11 }; // IP адрес (В броузере вводим 192.168.1.11)
EthernetServer server(80);
EthernetClient client; 
byte num_pins = 8;
byte relay_pin[] = {
  29, 31, 33, 35, 37, 39, 41, 43};
byte led_pin[] = {
  28, 30, 32, 34, 36, 38, 40, 42};
byte relay_pin_state[] = {
  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};  
byte from_client[] = {
  ON, ON, ON, ON, ON, ON, ON, ON};  
byte counter;
int reset_time = 50;
int time_counter[]= {
  0, 0, 0, 0, 0, 0, 0, 0};
long uptime[] = {
  0, 0, 0, 0, 0, 0, 0, 0};
boolean client_get_html;
boolean get;
//SD
Sd2Card card;
SdVolume volume;
SdFile root;
//------------
void setup()
{
  Serial.begin(9600);
  // init sd
  pinMode(ETH_SS, OUTPUT);
  digitalWrite(ETH_SS, HIGH);
  SD.begin(SD_SS);

  digitalWrite(SD_SS, HIGH);
  digitalWrite(ETH_SS, LOW);
  //-----------------------

  // Изначально выключаем все реле и включаем видимость ледов
  for (counter = 0; counter < num_pins; counter++)
  {
    pinMode(relay_pin[counter], OUTPUT);
    pinMode(led_pin[counter], INPUT);
    digitalWrite(relay_pin[counter], relay_pin_state[counter]);
    digitalWrite(led_pin[counter], HIGH);
  }

  Ethernet.begin(mac, ip);
  server.begin();

}

void loop()
{
  client = server.available();
  if (client)
  { 
    while (client.connected())
    {
      int dataCount = client.available();
      if (dataCount > 0)
      {
        for (int i = 0; i < dataCount; i++)
        {
          char ch = client.read();
          Serial.print(ch);
          if ( i == 0 ){
            if (ch == 'G'){
              get = true;
            } 
            else 
              if (ch == 'P'){
              get = false;
            } 
            else break;
          } 
          if ((ch=='o' || ch == 'r' || ch == 'c') && get==false)
          {
            char num = client.read();
            if (num == 'h')
            {
            } 
            else
            {
              if (num-'0' > 0 && num-'0' <= num_pins)
              {
                int current_btn = num-'0';
                if (ch=='o')
                {
                  time_counter[current_btn-1] = PRESS_TIME;
                }
                if (ch=='r')
                {
                  time_counter[current_btn-1] = reset_time;
                }
              }
            }
          }
        }
      }
      if (get){
        client.println("[{\"id\":1,\"status\":1,\"uptime\":1412160294}]");
      } 
      else {
        client.println("ok");
      }
      client.stop();
      //     while (!client_get_html){
      //     client.println("<!DOCTYPE html><html><head lang='en'><meta charset='UTF-8' http-equiv='refresh' content='30'><title>SQ reboot</title> <style>#content{ width: 330px;padding: 10px;margin: auto;} .form{margin: 20px 0;} .form > *{float: left;margin: 0 5px 0 0;} .form > span{ display: block; } .form:after {content:'';display:block;clear: both;} .power, .reset{width: 75px;margin-top: -2px;} .up_time {width: 90px;} #refresh{ width: 155px; margin-left: 31px; } .led { width: 26px; height: 26px; border: 0; line-height: 27px; text-align: center; font-size: 14px; color: white; border-radius: 13px; margin-top: -3px; } .led.on{ background-color: green; } .led.off{ background-color: red; } #title{ margin-left: 30px; } </style></head><body><div id='content'> <h1 id='title'>SQ reboot</h1>");
      //     for (counter = 0; counter < num_pins; counter++)
      //     {
      //       client.print("<form class='form' method='post' action='index.html' name='");
      //       client.print(counter+1);
      //       client.print("'><div class='led ");
      //      if (digitalRead(led_pin[counter])==LOW){
      //         client.print("on");
      //       } 
      //       else {
      //         client.print("off");
      //       }
      //       client.print("'>");
      //       client.print(counter+1);
      //       client.print("</div><button class='power' type='submit' name='o");
      //       client.print(counter+1);
      //       client.print("'>");
      //       if (digitalRead(led_pin[counter])==LOW){
      //         client.print("Pwr off");
      //       } 
      //       else {
      //         client.print("Pwr on");
      //       }
      //       client.print("</button><button class='reset' type='submit' name='r");
      //       client.print(counter+1);
      //       client.print("'>reset</button><span class='up_time'>01:23:45</span></form>");
      //     }
      //     client.print("<form class='form' method='post' action='index.html' name='9'><button id='refresh' type='submit' name='ch'>Refresh all</button></form> </div></body></html>");
      //      delay(100);
      //     client.stop();
      //     client_get_html = true;  
      //      }
    }
  } 
  // включаем или выключаем реле
  for (counter = 0; counter < num_pins; counter++)
  {
    if (time_counter[counter]>0)
    {
      Serial.print(" time to off ");
      Serial.print(time_counter[counter]);
      Serial.print(" pin ");
      Serial.print(counter);
      from_client[counter]=OFF;
      Serial.print(" is off ");
      Serial.print(from_client[counter]);
      Serial.print(" end log ");
    } 
    else 
    {
      from_client[counter]=ON;
    }
    if (time_counter[counter] > 0)
      time_counter[counter] = time_counter[counter] - 1;
    relay_pin_state[counter] = from_client[counter]==ON;
    digitalWrite(relay_pin[counter], relay_pin_state[counter]);
  }
  delay(10);
}


