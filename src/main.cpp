/*
LED_BUILTIN (LOW = ON, HIGH = OFF)
    STATES:
    1- OFF: not connected to internet
    2- ON (STATIC): internet connected and barrier intrusion detecting OFF
    3- BLINK: internet connected and barrier intrusion detecting ON
*/

#include <Arduino.h>
#include <utils.h>
#include <secrets.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <CTBot.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_CLIENT, UTC);
CTBot tBot;

void connect_wifi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("\nConnecting to ");
    Serial.print(WIFI_SSID);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }

    Serial.print("\nConnection established!");
    tBot.sendMessage(TELEGRAM_ID, "Connection established! at " + timeClient.getFormattedTime());
    Serial.print("\nIP address:\t");
    Serial.println(WiFi.localIP());
}

void connect_telegram_bot()
{
    tBot.wifiConnect(WIFI_SSID, WIFI_PASSWORD);
    tBot.setTelegramToken(TELEGRAM_TOKEN);

    while (!tBot.testConnection())
    {
        delay(1000);
        Serial.print('.');
    }

    timeClient.update();
    Serial.print("\tBot connected at " + timeClient.getFormattedTime());
    tBot.sendMessage(TELEGRAM_ID, "Bot connected at " + timeClient.getFormattedTime());
    Serial.print("\n");
}


boolean is_barrier_open()
{
    return digitalRead(BARRIER_PIN) == HIGH;
}

/*If current hour is between ONHOUR and OFFHOUR, checks for intrusion (Intrusion detecting ON)*/
void check_barrier_intrusion()
{
    if (is_between(timeClient.getHours(), ONHOUR, OFFHOUR))
    {
        if (is_barrier_open())
        {
            Serial.println("ALERT INTRUSION at" + timeClient.getFormattedTime());
            tBot.sendMessage(TELEGRAM_ID, "ALERT INTRUSION at" + timeClient.getFormattedTime());
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
    connect_wifi();
    digitalWrite(LED_BUILTIN, LOW);
    connect_telegram_bot();
}

void loop()
{
    timeClient.update();
    check_barrier_intrusion();
}
