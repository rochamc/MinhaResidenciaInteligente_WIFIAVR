#include <SoftwareSerial.h>
#include <WiFiUdp.h>

#define DEBUG true

char inData[200];
char inChar=-1;
byte index = 0;
int porta_rele1 = 7;
bool releligado = false;

void inicializaSerial()
{
    Serial.begin(115200);
    Serial1.begin(115200);
}
 
void setup()
{
  inicializaSerial();
 
  sendData("AT+RST\r\n", 2000, DEBUG); // rst
  // Configura IP Estatico
  sendData("AT+CIPSTA=\"192.168.0.35\",\"192.168.0.1\",\"255.255.255.0\"\r\n", 2000, DEBUG);
  delay(3000);
  // Conecta a rede wireless
  sendData("AT+CWJAP=\"ssid\",\"psswd\"\r\n", 2000, DEBUG);
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra o endereco IP
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Configura para multiplas conexoes
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Inicia o web server
  sendData("AT+CIPSERVER=1,6789\r\n", 1000, DEBUG);

  pinMode(porta_rele1, OUTPUT); 
}
 
void loop()
{
  // Verifica se o ESP8266 esta enviando dados
  if (Serial1.available())
  { 
    delay(300);
    ReadSerial();
    String string = String(inData);
    if(DEBUG)
    {
      Serial.print("String que foi lido no serial:\n");
      Serial.print(inData);
      Serial.print(string);
      Serial.print("\n**Fim da string lida**\n");
    }
    if(string.indexOf(":desligar") > 0 || string.indexOf(":ligar") > 0)
    {
      if(releligado)
      {
        digitalWrite(porta_rele1, LOW);
        releligado = false;
      }
      else 
      {
        digitalWrite(porta_rele1, HIGH);
        releligado = true;
      }
    }
    else
    {
      delay(300);
      int connectionId = Serial1.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Teste";
      int a = digitalRead(8);
      webpage += a;
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;
      closeCommand += "\r\n";
 
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}
 
String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  Serial1.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read(); // ler o próximo char
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}

char ReadSerial(){
 index = 0;
 for(int i=0;i<200;i++){
   inData[i]='\0';
 }
 while(Serial1.available() > 0)
 {
   if(index < 199)
   {
     inChar = Serial1.read();
     inData[index] = inChar;
     index++;
   }
 }
 inData[index] = '\0'
}
