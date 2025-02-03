#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <esp32cam.h>

const char* ssid = "NITRO-MARCIN 0845";
const char* password = "504Ahaslo";

Servo servo1;
Servo servo2;

const int servoPin1 = 14;
const int servoPin2 = 15;

int servo1Angle = 55;
int servo2Angle = 60;

const int servo1Min = 15;
const int servo1Max = 55;
const int servo2Min = 0;
const int servo2Max = 120;

WebServer server(80);

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);

void serveJpg() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo() {
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

void handleJpgHi() {
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleJpgMid() {
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}

void handleServo1() {
  String path = server.uri();
  int pos = path.lastIndexOf('/');
  if (pos != -1) {
    int change = path.substring(pos + 1).toInt();
    servo1Angle = constrain(servo1Angle + change, servo1Min, servo1Max);
    servo1.write(servo1Angle);
    Serial.print("Servo 1 moved to: ");
    Serial.println(servo1Angle);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void handleServo2() {
  String path = server.uri();
  int pos = path.lastIndexOf('/');
  if (pos != -1) {
    int change = path.substring(pos + 1).toInt();
    servo2Angle = constrain(servo2Angle + change, servo2Min, servo2Max);
    servo2.write(servo2Angle);
    Serial.print("Servo 2 moved to: ");
    Serial.println(servo2Angle);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void handleServoCommand() {
  String path = server.uri();
  Serial.print("Requested path: ");
  Serial.println(path);

  if (path.startsWith("/servo1/")) {
    int change = path.substring(8).toInt();
    servo1Angle = constrain(servo1Angle + change, servo1Min, servo1Max);
    servo1.write(servo1Angle);
    Serial.print("Servo 1 moved to: ");
    Serial.println(servo1Angle);
    server.send(200, "text/plain", "OK");
  } 
  else if (path.startsWith("/servo2/")) {
    int change = path.substring(8).toInt();
    servo2Angle = constrain(servo2Angle + change, servo2Min, servo2Max);
    servo2.write(servo2Angle);
    Serial.print("Servo 2 moved to: ");
    Serial.println(servo2Angle);
    server.send(200, "text/plain", "OK");
  } 
  else {
    server.send(404, "text/plain", "Not Found");
  }
}


void setup() {
  Serial.begin(115200);

  using namespace esp32cam;
  Config cfg;
  cfg.setPins(pins::AiThinker);
  cfg.setResolution(hiRes);
  cfg.setBufferCount(2);
  cfg.setJpeg(80);

  bool ok = Camera.begin(cfg);
  Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("http://");
  Serial.println(WiFi.localIP());

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(servo1Angle);
  servo2.write(servo2Angle);

  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid.jpg", handleJpgMid);
  server.on("/servo1/", handleServo1);
  server.on("/servo2/", handleServo2);
  server.onNotFound(handleServoCommand);

  server.begin();
}

void loop() {
  server.handleClient();
}
