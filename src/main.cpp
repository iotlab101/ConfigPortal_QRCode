#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

const byte          DNS_PORT = 53;
IPAddress           apIP(192, 168, 1, 1);
DNSServer           dnsServer;
WebServer           webServer(80);
const int           RELAY = 15;

String responseHTML = ""
    "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body><center>"
    "<p>Captive Sample Server App</p><br/><br/>"
    "<button id='button1' style='width:160px;height:60px'><font size='20'>Relay</font></button>"
    "<script>var xhttp=new XMLHttpRequest();"
    "  button1.onclick=function(e) {"
    "    xhttp.open('GET', 'http://192.168.1.1/button', false);"
    "    xhttp.send(''); }"
    "</script><p>This is a captive portal example</p></center></body></html>";

void button(){
    Serial.println("button pressed");
    digitalWrite(RELAY, !digitalRead(RELAY));
    webServer.send(200, "text/plain", "OK");
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY, OUTPUT);
    Serial.println();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("CaptivePortal");

    // if started with "*" for domain name, it will reply with provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);

    webServer.on("/button", button);

    // replay to all requests with same HTML
    webServer.onNotFound([]() {
        webServer.send(200, "text/html", responseHTML);
    });
    webServer.begin();
    Serial.println("Captive Portal Started");
}

void loop() {
    dnsServer.processNextRequest();
    webServer.handleClient();
}