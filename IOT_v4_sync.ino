#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

IPAddress local_IP(4,4,4,4);
IPAddress gateway(0,0,0,0);
IPAddress subnet(255,255,255,0);
bool power_failure=false;
bool configured=false;
bool bad_pass=false;
bool bad_auth=false;
int switchD5,switchD6,switchD7,switchD8;
int flag_blynk=0;

char ssid2[100];
char pass2[100];
char ID2[100];

String remflag;
String wifi_user;
String wifi_pass;
String blynk_ID;

char ssid_string2[600];
char ssid_string[600];

ESP8266WebServer server(80);

String main_page11="<!DOCTYPE html><html><body><center><h2>Welcome to IOT</h2><h3>Let's start by connecting to the internet.</h3>";
String main_page13="<br><font size='2'>1. Select your WiFi SSID from the list below.<br> 2. Enter the network password and the Blynk ID recieved in the Email address provided in the app.<br>3. Click 'Submit' to connect the system to the app.</font><br><br><form action='/action_page' method='post'>";
String main_page21="<br>Password:<br><input type='text' name='pass_' placeholder='Enter WiFi Password'><br>Blynk ID:<br><input type='text' name='ID_' placeholder='Enter Blynk ID'><br>";
String main_page22="<br><input type='checkbox' name='rem' value='true'>Remember credentials for faster login?<br><br><input type='submit' value='Submit'><br><br><br>Please note that you are not connected to the internet and your password is safe :)</center></form></body></html>";



//----------------------------------------------------------------------------------------------------------------
//                                            Initialize switch pins
//----------------------------------------------------------------------------------------------------------------
void initialize_switches(){
  switchD8 = digitalRead(D8);
  switchD5 = digitalRead(D5);
  switchD6 = digitalRead(D6);
  switchD7 = digitalRead(D7);
  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                            Declare virtual pins for use as status indicator LEDs in-app
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WidgetLED LED1(V6);
WidgetLED LED2(V7);
WidgetLED LED3(V8);
WidgetLED LED4(V9);
//----------------------------------------------------------------------------------------------------------------
//                                            Server reconnection app sync
//----------------------------------------------------------------------------------------------------------------
void serversync(){
  if (digitalRead(D0)){
    LED1.off();
  }
  else{
    LED1.on();
  }
  if (digitalRead(D1)){
    LED1.off();
    }
  else{
    LED1.on();
  }
  if (digitalRead(D2)){
    LED1.off();
    }
  else{
    LED1.on();
  }
  if (digitalRead(D3)){
    LED1.off();
    }
  else{
    LED1.on();
  }
}

//----------------------------------------------------------------------------------------------------------------
//                                            Change pin state with hardware switch
//----------------------------------------------------------------------------------------------------------------

void setswitchstates(){
  if(digitalRead(D5)!=switchD5){
    digitalWrite(D0,!digitalRead(D0));
    if (Blynk.connected()){
      if (digitalRead(D0)){
        LED1.off();
        }
      else{
        LED1.on();
      }
    }
      switchD5 = !switchD5;
  }
  if(digitalRead(D6)!=switchD6){
    digitalWrite(D1,!digitalRead(D1));
    if (Blynk.connected()){
    if (digitalRead(D1)){
      LED2.off();
    }
    else{
      LED2.on();
    }
    }
    switchD6 = !switchD6;
  }
  if(digitalRead(D7)!=switchD7){
    digitalWrite(D2,!digitalRead(D2));
    if (Blynk.connected()){
    if (digitalRead(D2)){
      LED3.off();
    }
    else{
      LED3.on();
    }
    }
    switchD7 = !switchD7;
  }
  if(digitalRead(D8)!=switchD8){
    digitalWrite(D3,!digitalRead(D3));
    if (Blynk.connected()){
    if (digitalRead(D3)){
      LED4.off();
    }
    else{
      LED4.on();
    }
    }
    switchD8 = !switchD8;
  }
}

//-----------------------------------------------------------------------------------------------------------
//                                            DATA RETRIEVAL UPON POWER FAILURE
//-----------------------------------------------------------------------------------------------------------



BLYNK_WRITE(V10){
  int pinValue = param.asInt();
  digitalWrite(D0,pinValue);
}


BLYNK_WRITE(V11){
  int pinValue = param.asInt();
  digitalWrite(D1,pinValue);
}

BLYNK_WRITE(V12){
  int pinValue = param.asInt();
  digitalWrite(D2,pinValue);
}

BLYNK_WRITE(V13){
  int pinValue = param.asInt();
  digitalWrite(D3,pinValue);
}

void power_sync() {
  Serial.println("POWER SYNC");
  Blynk.syncVirtual(V10,V11,V12,V13);
}
//-----------------------------------------------------------------------------------------------------------
//                                            Virtual pins get data from blynk
//-----------------------------------------------------------------------------------------------------------
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  if (pinValue == 1){
    digitalWrite(D0, !digitalRead(D0));
    if (digitalRead(D0)){
      LED1.off();
      Blynk.virtualWrite(V10,HIGH);
    }
    else{
      LED1.on();
      Blynk.virtualWrite(V10,LOW);
    }
  }
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if (pinValue == 1){
    digitalWrite(D1, !digitalRead(D1));
    if (digitalRead(D1)){
      LED2.off();
      Blynk.virtualWrite(V11,HIGH);
    }
    else{
      LED2.on();
      Blynk.virtualWrite(V11,LOW);
    }
  }
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  if (pinValue == 1){
    digitalWrite(D2, !digitalRead(D2));
    if (digitalRead(D2)){
      LED3.off();
      Blynk.virtualWrite(V12,HIGH);
    }
    else{
      LED3.on();
      Blynk.virtualWrite(V12,LOW);
    }
  }
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  if (pinValue == 1){
    digitalWrite(D3, !digitalRead(D3));
    if (digitalRead(D3)){
      LED4.off();
      Blynk.virtualWrite(V13,HIGH);
    }
    else{
      LED4.on();
      Blynk.virtualWrite(V13,LOW);
    }
  }
}

BLYNK_WRITE(V4)                                             //HARD RESET FROM APP
{
  int pinValue = param.asInt();
  if(pinValue){
    EEPROM.begin(512);
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.end();
    delay(2000);
    ESP.reset();
  }
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); 
  if (pinValue == 1){
    LED1.off();
    LED2.off();
    LED3.off();
    LED4.off();
    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
  }
}

//--------------------------------------------------------------------------------------
//                                  EEPROM Functions
//--------------------------------------------------------------------------------------
void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}


void writeString(char add1,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add1+i,data[i]);
  }
  EEPROM.write(add1+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}

 
String read_String(char add2)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add2);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add2+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}

//----------------------------------------------------------------------------
//                            Server Functions
//----------------------------------------------------------------------------
void handleroot(){
  bad_pass=false;
  bad_auth=false;
  String s;
  s+=main_page11;
  s+=main_page13;
  s+=String (ssid_string2);
  s+=main_page21;
  s+=main_page22;
  configured=true;
  Serial.println("Send string initialized");
  Serial.println(s);
  server.send(200,"text/html",s);
}
void handleform() {
  if(configured)
  {
    server.send(200,"text/html","<center>IoT is now connecting to Blynk servers, if the Access Point doesn't automatically disconnect, in 30 seconds <a href='http://4.4.4.4/status'>click here to check the status.</a></center>");
    int len;

    wifi_user = server.arg("form1");
    delay(10);
    const char *str=wifi_user.c_str();
    int x;
    configured=true;
    sscanf(str,"%d",&x);
    len = WiFi.SSID(x).length() + 1;
    char arr_ssid[len];
    WiFi.SSID(x).toCharArray(arr_ssid,len);  
  
    wifi_pass = server.arg("pass_");
    len = wifi_pass.length() + 1;
    char arr_pass[len];
    wifi_pass.toCharArray(arr_pass,len);
  
    blynk_ID = server.arg("ID_");
    len = blynk_ID.length() + 1;  
    char arr_blynk[len];
    blynk_ID.toCharArray(arr_blynk,len);

    remflag = server.arg("rem");
    len = remflag.length() + 1;
    char arr_rem[len];
    remflag.toCharArray(arr_rem,len);
    
    Serial.println("SSID:");
    Serial.println(arr_ssid);
    Serial.println("Password: ");
    Serial.println(arr_pass);
    Serial.println("Blynk ID:");
    Serial.println(arr_blynk);
    WiFi.begin(arr_ssid, arr_pass);
    int ml=0;
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
      ++ml;
      if(ml==13){
        bad_pass=true;
      }
      if (bad_pass){
        break;
      }
    }
    if (bad_pass){
      return;
    }
    Serial.println("Correct password");
    delay(1000);
    Blynk.connectWiFi(arr_ssid, arr_pass);
    Blynk.config(arr_blynk);
    ml=0;
    while (!Blynk.connected()){
    Blynk.connect();
    ++ml;
    Serial.print(".");
    if(ml==9)
    {
      Serial.println("Bad auth");
      bad_auth=true;
      return;
    }
    }
    Serial.println("Correct auth");
    WiFi.softAPdisconnect();
    configured=false;
    flag_blynk=1;
    delay(1000);
    if (!strcmp(arr_rem,"true")){
      EEPROM.begin(512);
      writeString(0,WiFi.SSID(x));
      writeString(50,wifi_pass);
      writeString(100,blynk_ID);
      eeWriteInt(200,1);
      EEPROM.end();
      Serial.println("Crendentials saved");
    }
  }
  else{
    server.send(404,"text/html","<h2>404: Not found</h2> <br> Looks like you're lost, please return to <a href='http://4.4.4.4/'>Network Selection Homepage</a>");
  }
}

void handlenotfound(){
  Serial.println("not found");
  server.send(404,"text/html","<h2>404: Not found</h2> <br> Looks like you're lost, please return to <a href='http://4.4.4.4/'>Network Selection Homepage</a>");
}

void handlestatus(){
  if (bad_pass){
    server.send(200,"text/html","<center>Wrong WIFI password, please<a href='http://4.4.4.4/'>click here to return to the login page and try again.</a></center>");
  }
  if (bad_auth){
    server.send(200,"text/html","<center>Wrong Blynk Authentication Token or unstable internet connection, please<a href='http://4.4.4.4/'>click here to return to the login page and try again.</a></center>");
  }
  if (!bad_pass && !bad_auth && flag_blynk==1){
    server.send(200,"text/html","<center>Blynk is working as it should. :)");
  }
  if (!bad_pass && !bad_auth && flag_blynk ==0)
  {
    server.send(200,"text/html","<center>Module not configured, please<a href='http://4.4.4.4/'>click here to login</a></center>");
  }
}

void handlereset(){
  server.send(200,"text/html","<center>Server is performing a hard reset. Please wait for 5 seconds then <a href='http://4.4.4.4/'>click here to login</a></center>");
  EEPROM.begin(512);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  delay(2000);
  ESP.reset();
}

//-----------------------------------------------------------------------------------------------------------
//                                            Setup() and Loop()
//-----------------------------------------------------------------------------------------------------------

void setup() {
  delay(1000);
  
  int i=0;
  
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  
  initialize_switches();
  
  Serial.begin(115200);
  Serial.println("");
  
  EEPROM.begin(512);
  flag_blynk=eeGetInt(200);
  EEPROM.end();
  
  if (flag_blynk!=1){
    flag_blynk=0;
  }
  Serial.println("Flag value:");
  Serial.println(flag_blynk);
  
  if (flag_blynk==0){
    Serial.println("Initializing wifi and scanning for networks.");
    
    int n=WiFi.scanNetworks();
    
    if ((n+n)!=(2*n))
    {
      Serial.println("No nearby WiFi networks were found. Make sure your WiFi router is ON.");
      
      while((n+n)!=(2*n)){
        n=WiFi.scanNetworks();
        Serial.print(".");
        delay(3000);
      }
      
      Serial.println("");
    }
    else
    {
      Serial.println("Scan complete");
      Serial.print("Number of networks= ");
      Serial.println(n);
      Serial.println("Available networks:");
      int z=0;
    
      for(int i=0;i<n;i++)
      {
        char ssid_temp2[100];
        sprintf(ssid_temp2,"<option value='%d'>%s</option>",i,WiFi.SSID(i).c_str());
        sprintf(ssid_string,"%s%s",ssid_string,ssid_temp2);  
        
      }
      sprintf(ssid_string2,"<select name='form1'>%s</select>",ssid_string);
      
      Serial.println("SSID Acquire complete");
      Serial.println(ssid_string2);
      Serial.println("Staring access point");
      
      delay(100);
      
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP("IOT");
      Serial.println("Configuring access point");
      
      if (WiFi.softAPConfig(local_IP, gateway, subnet)){
        Serial.println("Configured");
      }
      else{
        Serial.println("Failed to configure");
      }
     Serial.println("Starting Web server on PORT 80");
     
     delay(50);
     
     server.begin();
     
     Serial.println("Server Online at ip ");
     Serial.println(WiFi.softAPIP());
     
     server.on("/", handleroot);      //Which routine to handle at root location
     server.on("/action_page", handleform);        //form action is handled here
     server.on("/status",handlestatus);
     server.on("/reset",handlereset);
     server.onNotFound(handlenotfound);
     }
  }
  //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  //                                                                      Power Failure
  //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  if(flag_blynk==1){
    bad_pass=false;
    bad_auth=false;
    power_failure=true;
    int len;
    
    EEPROM.begin(512);
    wifi_user=read_String(0);
    wifi_pass=read_String(50);
    blynk_ID=read_String(100);
    EEPROM.end();

    Serial.println(wifi_user);
    len = wifi_user.length() + 1;
    char arr_ssid[len];
    wifi_user.toCharArray(arr_ssid,len);
    strncpy(ssid2,arr_ssid,len);
    
    Serial.println(wifi_pass);
    len = wifi_pass.length() + 1;
    char arr_pass[len];
    wifi_pass.toCharArray(arr_pass,len);
    strncpy(pass2,arr_pass,len);
  
    Serial.println(blynk_ID);
    len = blynk_ID.length() + 1;  
    char arr_blynk[len];
    blynk_ID.toCharArray(arr_blynk,len);
    strncpy(ID2,arr_blynk,len);
    
    int ml=0;
    WiFi.begin(arr_ssid, arr_pass);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
      ++ml;
      if(ml==13){
        bad_pass=true;
      }
      if (bad_pass){
        break;
      }
    }
    if (bad_pass){
      EEPROM.begin(512);
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.end();
      delay(2000);
      ESP.reset();
    }
    Serial.println("Correct password");
    delay(1000);
    Blynk.connectWiFi(arr_ssid, arr_pass);
    Blynk.config(arr_blynk);
    ml=0;
    while (!Blynk.connected()){
    Blynk.connect();
    ++ml;
    Serial.print(".");
    if(ml==9)
    {
      Serial.println("Bad auth");
      EEPROM.begin(512);
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.end();
      delay(2000);
      ESP.reset();
    }
    }
    Serial.println("Correct auth");
    power_sync();
  }
}


void loop() {
  if(flag_blynk==0){
    server.handleClient();
  }
  else{
    if(!Blynk.connected()){
      Serial.println("BLYNK DISCONNECTED");
      WiFi.disconnect();
      Serial.println("WIFI BEGIN");
       WiFi.begin(ssid2,pass2);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(100);
        Serial.print(".");
        setswitchstates();
      }
      Serial.println("WiFi connection success");
      
      delay(1000);
      int count=0;
      while (!Blynk.connected()){
        for (count=0;count<150;count++){
          delay(500);
          setswitchstates();
        }
        count=0;
        Blynk.connectWiFi(ssid2, pass2);
        Serial.print("|");
        Blynk.config(ID2);
        Blynk.connect();
        Serial.print(". ");
      }
      serversync();
      Serial.println("Connected to server");
    }
    setswitchstates();
    Blynk.run();
  }
}
