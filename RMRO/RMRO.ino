#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Motor.h>

#define FIREBASE_HOST "https://rmro-5d8ff-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "9pnprE3COTZSqFNvSLRcR0IohdAUoUmGg2AAnEbP"
FirebaseData firebaseData;

// #define WIFI_SSID "Jorge's Room" // input your home or public wifi name
// #define WIFI_PASSWORD "Tostatronic 1$" //password of wifi ssid
#define WIFI_SSID "DESKTOP-RCPTMBJ 0128"
#define WIFI_PASSWORD "!d49K968" //password of wifi ssid

Motor motorA_1 = Motor(0, 2);
Motor motorB_1 = Motor(4, 5);

//Motor motorA_2 = Motor(12, 13);
//Motor motorB_2 = Motor(14, 15);

String path = "/actions/Mi primer robot";
// String path = "/actions/Mi%primer%robot";
bool allowMove = false;
short countVerification = 0;

void setup() {
  Serial.begin(9600);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP()); //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to firebase

  sentSignal();
}

void loop()
{
  if (countVerification <= 0) {
    allowMove = checkStatus();
  } else if(countVerification >= 5) {
    countVerification = -1;
  }

  if (allowMove) {
    move();
  }

  countVerification++;
}

void Delante() {
  motorA_1.MoveFoward();
  motorB_1.MoveFoward();

  //motorA_2.MoveFoward();
  //motorB_2.MoveFoward();
}

void Atras() {
  motorA_1.MoveBackward();
  motorB_1.MoveBackward();

  //motorA_2.MoveBackward();
  //motorB_2.MoveBackward();
}

void Derecha() {
  motorA_1.MoveFoward();
  motorB_1.MoveBackward();

  //motorA_2.MoveFoward();
  //motorB_2.MoveBackward();
}

void Izquierda() {
  motorA_1.MoveBackward();
  motorB_1.MoveFoward();

  //motorA_2.MoveBackward();
  //motorB_2.MoveFoward();
}

void Stop() {
  motorA_1.Stop();
  motorB_1.Stop();

  //motorA_2.Stop();
  //motorB_2.Stop();
}

// Indicar que el robot estya en linea
void sentSignal() {
  Firebase.setBool(firebaseData, path + "/robot", true);
}

// Verificar que el control esta en linea
bool controlOnline() {
  Firebase.getBool(firebaseData, path + "/control");
  bool control = firebaseData.boolData();

  return control;
}

// Verificar que el robot esta en linea
bool robotOnline() {
  Firebase.getBool(firebaseData, path + "/robot");
  bool robot = firebaseData.boolData();

  return robot;
}

// Verificar que el control y robot estan en linea
bool checkStatus() {
  return controlOnline() && robotOnline();
}

void move() {
  Firebase.getString(firebaseData, path + "/action");

  Serial.println(firebaseData.stringData());
  if (firebaseData.stringData() == "C") {
    Serial.println("Centro");
    Stop();
  } else if (firebaseData.stringData() == "\"N\"") {
    Serial.println("Adelante");
    Stop();
    Delante();
  } else if (firebaseData.stringData() == "\"S\"") {
    Serial.println("Atras");
    Stop();
    Atras();
  } else if (firebaseData.stringData() == "\"E\"") {
    Serial.println("Derecha");
    Stop();
    Derecha();
  } else if (firebaseData.stringData() == "\"W\"") {
    Serial.println("Izquierda");
    Stop();
    Izquierda();
  } else {
    Serial.println("Otro");
    Stop();
  }
}