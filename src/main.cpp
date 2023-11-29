/*
LED_BUILTIN (LOW = ON, HIGH = OFF)
    STATES:
    1- OFF: not connected to internet
    2- ON (STATIC): internet connected and barrier intrusion detecting OFF
    3- BLINK: internet connected and barrier intrusion detecting ON
*/

#include <Arduino.h>
#include <secrets.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <CTBot.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ar.pool.ntp.org", UTC);
CTBot tBot;

void waitWifiConnection()
{
    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }
}
boolean connectWifi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("\nConnecting to ");
    Serial.print(WIFI_SSID);
    Serial.println(" ...");
    waitWifiConnection();
    Serial.print("\nConnection established!");
    tBot.sendMessage(TELEGRAM_ID, "Connection established! at " + timeClient.getFormattedTime());
    Serial.print("\nIP address:\t");
    Serial.println(WiFi.localIP());
    return true;
}

void waitBotConnection()
{
    int i = 0;
    while (!tBot.testConnection())
    {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }
}

boolean connectTelegramBot()
{
    tBot.wifiConnect(WIFI_SSID, WIFI_PASSWORD);
    tBot.setTelegramToken(TELEGRAM_TOKEN);
    Serial.print("\nConnecting Bot ...");
    waitBotConnection();
    timeClient.update();
    Serial.print("\tBot connected! at " + timeClient.getFormattedTime());
    tBot.sendMessage(TELEGRAM_ID, "Bot connected! at " + timeClient.getFormattedTime());
    Serial.print("\n");
    return true;
}


boolean isBetween(int currentHour, int startHour, int endHour)
{
    if (endHour > startHour) // Time does not cross midnight
    {
        if (currentHour >= startHour && currentHour <= endHour)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    if (endHour < startHour) // Time cross midnight
    {
        if (currentHour >= startHour && currentHour >= endHour) // time is between before midnight
        {
            return true;
        }
        else if (currentHour <= startHour && currentHour <= endHour) // time is between after midnight
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    // Add a return statement outside of any conditional block
    return false;
}


boolean isOpen(int input)
{
    return digitalRead(input) == HIGH;
}

/*If current hour is between ONHOUR and OFFHOUR, checks for intrusion (Intrusion detecting ON)*/
void checkBarrierIntrusion()
{
    if (isBetween(timeClient.getHours(), ONHOUR, OFFHOUR))
    {
        if (isOpen(BARRIER_PIN))
        {
            Serial.println("Intrusion in BARRIER at " + timeClient.getFormattedTime());
            tBot.sendMessage(TELEGRAM_ID, "Intrusion in BARRIER at " + timeClient.getFormattedTime());
            delay(5000);
        }
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BARRIER_PIN, INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, HIGH);
    connectWifi();
    digitalWrite(LED_BUILTIN, LOW);
    connectTelegramBot();
}

void loop()
{
    timeClient.update();
    checkBarrierIntrusion();
}