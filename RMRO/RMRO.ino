#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Motor.h>
#include <Servo.h>

#define FIREBASE_HOST "https://rmro-5d8ff-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "9pnprE3COTZSqFNvSLRcR0IohdAUoUmGg2AAnEbP"
FirebaseData firebaseData;

#define WIFI_SSID "Jorge's Room" // input your home or public wifi name
#define WIFI_PASSWORD "Tostatronic 1$" //password of wifi ssid
// #define WIFI_SSID "DESKTOP-RCPTMBJ 0128"
// #define WIFI_PASSWORD "!d49K968" //password of wifi ssid
// #define WIFI_SSID "Totalplay-2.4G-4588"
// #define WIFI_PASSWORD "Wv82VejXELbVSRUu" //password of wifi ssid

// Parameters (pin, pin, enable, speed)
Motor motorA_1 = Motor(0, 2, 14, 10); // Left motor
Motor motorB_1 = Motor(4, 5, 15, 10); // Right motor

Servo servo_1; // Bottom servomotor
Servo servo_2; // Top servomotor

const int pinMagnet = 13; // Electromagnet

const String path = "/actions/Mi primer robot";
bool allowMove = false;
bool allowArm = false;
short countVerification = 0;
short servoPosition = 45; // Degrees for servomotors

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

  servo_1.attach(12); // Assign Pin for servo
  servo_2.attach(14); // Assign Pin for servo

  pinMode(pinMagnet, OUTPUT); // Enable pin

  sentSignal(); // Robot online

  initArm(); // Init position for arm
}

void loop()
{
  // Senial para indicar que el robot esta en linea
  if (countVerification == 0) {
    sentSignal();
  }

  if (countVerification <= 0) {
    checkStatus();
  } else if (countVerification >= 5) {
    countVerification = -1;
  }

  if (allowMove) {
    move();
  } else if (allowArm) {
    moveArm();
    magnet();
  }

  countVerification++;
}

void Delante() {
  motorA_1.MoveFoward();
  motorB_1.MoveFoward();
}

void Atras() {
  motorA_1.MoveBackward();
  motorB_1.MoveBackward();
}

void Derecha() {
  motorA_1.MoveFoward();
  motorB_1.MoveBackward();
}

void Izquierda() {
  motorA_1.MoveBackward();
  motorB_1.MoveFoward();
}

void Stop() {
  motorA_1.Stop();
  motorB_1.Stop();
}

// Indicar que el robot esta en linea
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

// Verificar que el brazo esta en activado
bool activedArm() {
  Firebase.getBool(firebaseData, path + "/arm");
  bool arm = firebaseData.boolData();

  return arm;
}

// Verificar que el control y robot estan en linea;
// Verificar si el brazo esta activado
void checkStatus() {
  if (controlOnline() && robotOnline()) {
    // Control y robot en linea
    if (activedArm()) {
      // Activar brazo
      // Desactivar movimiento
      allowArm = true;
      allowMove = false;
    } else {
      // Desactivar brazo
      // Activar movimiento
      allowArm = false;
      allowMove = true;
    }
  } else {
    // Desactivar brazo
    // Desactivar movimiento
    allowMove = false;
    allowArm = false;
  }
}

void move() {
  Firebase.getString(firebaseData, path + "/direction");

  String direction = firebaseData.stringData();
  Serial.println(direction);
  if (direction == "\"C\"") {
    Serial.println("Centro");
    //adjustSpeed((uint8_t) 0);
    Stop();
  } else if (direction == "\"N\"") {
    Serial.println("Adelante");
    Stop();
    delay(10);
    //adjustSpeed(getYSpeed());
    Delante();
  } else if (direction == "\"S\"") {
    Serial.println("Atras");
    Stop();
    delay(10);
    //adjustSpeed(getYSpeed());
    Atras();
  } else if (direction == "\"E\"") {
    Serial.println("Derecha");
    Stop();
    delay(10);
    //adjustSpeed(getXSpeed());
    Derecha();
  } else if (direction == "\"W\"") {
    Serial.println("Izquierda");
    Stop();
    delay(10);
    //adjustSpeed(getXSpeed());
    Izquierda();
  } else {
    Serial.println("Otro");
    //adjustSpeed((uint8_t) 0);
    Stop();
  }
}

uint8_t getXSpeed() {
  Firebase.getInt(firebaseData, path + "/vX");
  return (uint8_t) firebaseData.intData();
}

uint8_t getYSpeed() {
  Firebase.getInt(firebaseData, path + "/vY");
  return (uint8_t) firebaseData.intData();
}

void adjustSpeed(uint8_t speed) {
  // Validar si el valor es negativo para convertir a positivo
  if (speed < 0) {
    speed = -speed;
  }
  Serial.println(speed);
  motorA_1.SetMotorSpeed(speed);
  motorB_1.SetMotorSpeed(speed);
}

void initArm() {
  // Ajustar en su posicion inicial el robot (dejar objeto)
  for(servoPosition; servoPosition <= 180; servoPosition++) {
    servo_1.write(servoPosition);
    servo_2.write(servoPosition);
    delay(10);
  }
}

void moveArm() {
  Firebase.getBool(firebaseData, path + "/activatedArm");
  // Validar que posicion de tomar el brazo
  if (firebaseData.boolData()) { // Brazo en posicion para tomar objeto
    Serial.println("Externo");
    for(servoPosition; servoPosition > 0; servoPosition--) {
      servo_1.write(servoPosition);
      servo_2.write(servoPosition); 
      delay(10);
    }
  } else { // Brazo en posicion para dejar objeto
    Serial.println("Interno");
    for(servoPosition; servoPosition <= 180; servoPosition++) {
      servo_1.write(servoPosition);
      servo_2.write(servoPosition);
      delay(10);
    }
  }
}

// Control electroiman
void magnet() {
  Firebase.getBool(firebaseData, path + "/magnet");
  // Validar estado del iman
  if (firebaseData.boolData()) { // Activar iman
    digitalWrite(pinMagnet, HIGH);
  } else {  // Desactivar iman
    digitalWrite(pinMagnet, LOW);
    delay(200);
  }
}
