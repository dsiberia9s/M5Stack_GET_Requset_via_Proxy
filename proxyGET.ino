#include <M5Stack.h>
#include <WiFi.h>

const char* ssid = "";
const char* password = "";

IPAddress proxyIP(0, 0, 0, 0);
int proxyPort = 1234;

String url = ""; 

String proxyGET(IPAddress ip, int port, String url, bool header = false, unsigned long timeout = 10000) {
  WiFiClient client;
  String result = "";
  if (WiFi.status() != WL_CONNECTED) {
    client.stop();
    return "ERR:WIFI";
  }
  if (!client.connect(ip, port)) {
    client.stop();
    return "ERR:PROXY";
  }
  
  String host = "";
  int rsc = 0;
  for (int i = 0; i < url.length(); i++) {
    if (url[i] == '/') rsc++;
    if ((rsc == 2) && (url[i] != '/')) host += url[i];
    if (rsc == 3) break;
  }

  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + host);
  client.println("Connection: close");
  client.println();

  unsigned long t = millis();
  while (client.available() == 0) {
    if (millis() - t > timeout) {
      client.stop();
      return "ERR:TIMEOUT";
    }
  }

  while (client.available()) {
    result += (char)client.read();
  }
  
  if (!header) {
    char * eoh = strstr(result.c_str(), "\r\n\r\n");
    result = eoh + 4;
  }
  
  client.stop();
  return result;
}

void setup() {
  M5.begin();

  M5.Lcd.print("Attempting to connect to SSID: ");
  M5.Lcd.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(1000);
  }

  M5.Lcd.print("Connected to ");
  M5.Lcd.println(ssid);

  String res = proxyGET(proxyIP, proxyPort, url, 1);
  M5.Lcd.print(res);
}

void loop() {

}
