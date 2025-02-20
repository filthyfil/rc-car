#include <WiFi.h>
#include <WebServer.h>
//
unsigned long times;
int balls;

WebServer server(80);

void setMotor(int xspeed, int yspeed) {
    if (xspeed > 0){
      analogWrite(19, xspeed);
    }
    // if (xspeed<0){
    //   xspeed = abs(xspeed);
    //   analogWrite(22,xspeed);
    // }
    // if(xspeed==0){
    //   analogWrite(22,0);
    //   analogWrite(19,0);
    // }

    if (yspeed > 0){
      analogWrite(23, yspeed);
    }
    // if(yspeed < 0){
    //   yspeed = abs(yspeed);
    //   analogWrite(18,yspeed);
    // }
    // if (yspeed ==0){
    //   analogWrite(18,0);
    //   analogWrite(23,0);
    // }

}

void handleJoystick() {

  times = millis();

  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.println(y);

  // int yspeed = map(y, -200, 200, -200, 200);
  // int xspeed = map(x, -200, 200, -200, 200);
 
  setMotor(x, y);

  server.send(200, "text/plain", "Joystick updated");
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <style>
        body { text-align: center; font-family: Arial, sans-serif; background: black; margin: 0; padding: 0; height: 100vh; display: flex; flex-direction: column; justify-content: center; align-items: center; }
        #joystick { width: 600px; height: 600px; background-color: #ddd; border-radius: 50%; position: relative; touch-action: none; margin-top: 100px; }
        #stick { width: 160px; height: 160px; background-color: #666; border-radius: 50%; position: absolute; top: 220px; left: 220px; transition: transform 0.1s ease; }
        #stick.active { background-color: #f00; transform: scale(1.2); }
        h1 { font-size: 48px; color: #fff; margin-top: 20px; }
      </style>
    </head>
    <body>
      <h1></h1>
      <div id="joystick">
        <div id="stick"></div>
      </div>
      <script>
        const joystick = document.getElementById("joystick");
        const stick = document.getElementById("stick");
        const center = 300;

        joystick.addEventListener("touchmove", (e) => {
          const touch = e.touches[0];
          const rect = joystick.getBoundingClientRect();
          let x = touch.clientX - rect.left - center;
          let y = touch.clientY - rect.top - center;
          x = Math.max(-200, Math.min(200, x));
          y = Math.max(-200, Math.min(200, y));
          stick.style.left = `${x + 220}px`;
          stick.style.top = `${y + 220}px`;
          fetch(`/joystick?x=${x}&y=${-y}`);
        });

        joystick.addEventListener("touchend", () => {
          stick.style.left = "220px";
          stick.style.top = "220px";
          fetch("/joystick?x=0&y=0");
        });
      </script>
    </body>
    </html>
  )rawliteral");
}

void setup() {
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(18, OUTPUT);

  WiFi.softAP("ESP32-RC", "12345678");
  server.on("/", handleRoot);
  server.on("/joystick", handleJoystick);
  server.begin();
}

void loop() {

  if (millis()-times>200){
    // digitalWrite(22, 0);
    // digitalWrite(19, 0);
    // digitalWrite(23, 0);
    // digitalWrite(18, 0);
    setMotor(0,0);
    // delay(10000);
  }


  server.handleClient();
}
