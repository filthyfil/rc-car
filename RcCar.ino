#include <WiFi.h>
#include <WebServer.h>

unsigned long times;
WebServer server(80);

#define MOTOR1_FORWARD 19
#define MOTOR1_BACKWARD 22
#define MOTOR2_FORWARD 23
#define MOTOR2_BACKWARD 18

const int pwmFreq = 5000;     // 5 kHz PWM frequency
const int pwmResolution = 8;  // 8-bit resolution (0–255)


void setMotor(int xspeed, int yspeed) {
    if (xspeed > 20) {
        ledcWrite(MOTOR1_FORWARD, xspeed);
        ledcWrite(MOTOR1_BACKWARD, 0);
    } else if (xspeed < -20) {
        
        ledcWrite(MOTOR1_FORWARD, 0);
        ledcWrite(MOTOR1_BACKWARD, abs(xspeed));
    } else {
        ledcWrite(MOTOR1_FORWARD, 0);
        ledcWrite(MOTOR1_BACKWARD, 0);
    }

    if (yspeed > 0) {
        ledcWrite(MOTOR2_FORWARD, yspeed);
        ledcWrite(MOTOR2_BACKWARD, 0);
    } else if (yspeed < 0) {
        ledcWrite(MOTOR2_FORWARD, 0);
        ledcWrite(MOTOR2_BACKWARD, abs(yspeed));
    } else {
        ledcWrite(MOTOR2_FORWARD, 0);
        ledcWrite(MOTOR2_BACKWARD, 0);
    }
}

void handleJoystick() {
    times = millis();
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();

    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.println(y);

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
    h1 { font-size: 48px; color: #fff; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>ESP32 RC Car</h1>
  <div id="joystick">
    <div id="stick"></div>
  </div>
  <script>
    const joystick = document.getElementById("joystick");
    const stick = document.getElementById("stick");
    const center = 300;
    let lastX = 0, lastY = 0;

    // Function to send joystick data
    function sendJoystickData(x, y) {
      // Only send if the position changes significantly (threshold of 10)
      if (Math.abs(x - lastX) > 10 || Math.abs(y - lastY) > 10) {
        fetch(`/joystick?x=${x}&y=${-y}`);
        lastX = x;
        lastY = y;
      }
    }

    joystick.addEventListener("touchmove", (e) => {
      e.preventDefault();
      const touch = e.touches[0];
      const rect = joystick.getBoundingClientRect();
      let x = touch.clientX - rect.left - center;
      let y = touch.clientY - rect.top - center;

      // Limit movement range
      x = Math.max(-200, Math.min(200, x));
      y = Math.max(-200, Math.min(200, y));

      // Move the joystick visually
      stick.style.left = `${x + 220}px`;
      stick.style.top = `${y + 220}px`;

      // Send updated position
      sendJoystickData(x, y);
    });

    joystick.addEventListener("touchend", () => {
      // Reset joystick to center
      stick.style.left = "220px";
      stick.style.top = "220px";
      sendJoystickData(0, 0); // Ensure motors stop
    });
  </script>
</body>
</html>

    )rawliteral");
}

void setup() {
    Serial.begin(115200);
  // Attach pins to PWM with the new API
  ledcAttach(MOTOR1_FORWARD, pwmFreq, pwmResolution);
  ledcAttach(MOTOR1_BACKWARD, pwmFreq, pwmResolution);
  ledcAttach(MOTOR2_FORWARD, pwmFreq, pwmResolution);
  ledcAttach(MOTOR2_BACKWARD, pwmFreq, pwmResolution);

    WiFi.softAP("ESP32-RC", "12345678");
    server.on("/", handleRoot);
    server.on("/joystick", handleJoystick);
    server.begin();
}

void loop() {

      server.handleClient();


}
