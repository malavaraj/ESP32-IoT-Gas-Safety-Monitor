#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define GAS_SENSOR_PIN 34
#define RELAY_PIN      26
#define BUZZER_PIN     27

// =====================================================
// GAS THRESHOLD
// =====================================================

#define GAS_THRESHOLD 500

// =====================================================
// RELAY TYPE
// =====================================================
// Your relay is configured as ACTIVE HIGH.
//
// GPIO HIGH -> Relay ON  -> Fan ON
// GPIO LOW  -> Relay OFF -> Fan OFF
// =====================================================

bool RELAY_ACTIVE_LOW = false;

// =====================================================
// WEB SERVER
// =====================================================

WebServer server(80);

// =====================================================
// VARIABLES
// =====================================================

int gasValue = 0;

bool gasLeakage = false;
bool fanState = false;
bool buzzerState = false;

bool manualFan = false;

// =====================================================
// SENSOR TIMER
// =====================================================

unsigned long previousMillis = 0;

const unsigned long sensorInterval = 500;

// =====================================================
// RELAY CONTROL
// =====================================================

void controlRelay(bool state)
{
  fanState = state;

  if (RELAY_ACTIVE_LOW)
  {
    // Active LOW
    digitalWrite(RELAY_PIN, state ? LOW : HIGH);
  }
  else
  {
    // Active HIGH
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  }
}

// =====================================================
// BUZZER CONTROL
// =====================================================

void controlBuzzer(bool state)
{
  buzzerState = state;

  if (state)
  {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// =====================================================
// READ GAS SENSOR
// =====================================================

void readGasSensor()
{
  gasValue = analogRead(GAS_SENSOR_PIN);

  if (gasValue >= GAS_THRESHOLD)
  {
    gasLeakage = true;
  }
  else
  {
    gasLeakage = false;
  }
}

// =====================================================
// AUTOMATIC SAFETY CONTROL
// =====================================================

void automaticControl()
{
  readGasSensor();

  // ===================================================
  // GAS LEAKAGE
  // ===================================================

  if (gasLeakage)
  {
    // Safety priority
    // FAN MUST BE ON

    controlRelay(true);

    // BUZZER ON
    controlBuzzer(true);
  }

  // ===================================================
  // SAFE CONDITION
  // ===================================================

  else
  {
    // BUZZER OFF
    controlBuzzer(false);

    // Manual fan control
    if (manualFan)
    {
      controlRelay(true);
    }
    else
    {
      controlRelay(false);
    }
  }
}

// =====================================================
// WEB PAGE
// =====================================================

void handleRoot()
{
  String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>Gas Safety Monitor</title>

<style>

* {
    box-sizing: border-box;
}

body {
    margin: 0;
    font-family: Arial, Helvetica, sans-serif;
    background: #050505;
    color: white;
    min-height: 100vh;
}

/* HEADER */

.header {
    text-align: center;
    padding: 25px 15px;
    background: #090909;
    border-bottom: 1px solid #222;
}

.header h1 {
    margin: 0;
    font-size: 28px;
    letter-spacing: 1px;
}

.header p {
    color: #888;
    margin-top: 8px;
    font-size: 14px;
}

/* MAIN */

.container {
    width: 95%;
    max-width: 1000px;
    margin: 25px auto;
}

/* STATUS CARD */

.status-card {
    background: #0d0d0d;
    border: 1px solid #252525;
    border-radius: 18px;
    padding: 30px;
    text-align: center;
    margin-bottom: 20px;
}

.status-title {
    color: #888;
    font-size: 15px;
    margin-bottom: 15px;
}

.status {
    font-size: 32px;
    font-weight: bold;
}

.safe {
    color: #00e676;
}

.danger {
    color: #ff3b30;
}

/* GAS VALUE */

.gas-value {
    font-size: 65px;
    font-weight: bold;
    margin: 15px 0;
}

.unit {
    color: #777;
    font-size: 16px;
}

/* GRID */

.grid {
    display: grid;
    grid-template-columns:
    repeat(auto-fit, minmax(220px, 1fr));
    gap: 18px;
}

.card {
    background: #0d0d0d;
    border: 1px solid #252525;
    border-radius: 16px;
    padding: 25px;
    text-align: center;
}

.card-title {
    color: #888;
    font-size: 15px;
    margin-bottom: 12px;
}

.card-value {
    font-size: 25px;
    font-weight: bold;
}

/* ON / OFF */

.on {
    color: #00e676;
}

.off {
    color: #888;
}

/* CONTROL */

.control {
    margin-top: 20px;
    background: #0d0d0d;
    border: 1px solid #252525;
    border-radius: 16px;
    padding: 25px;
    text-align: center;
}

.control h2 {
    margin-top: 0;
    font-size: 20px;
}

.control p {
    color: #777;
}

/* BUTTONS */

button {
    border: none;
    padding: 14px 30px;
    margin: 8px;
    border-radius: 10px;
    font-size: 16px;
    font-weight: bold;
    cursor: pointer;
}

button:hover {
    transform: scale(1.03);
}

.fan-on {
    background: #00c853;
    color: white;
}

.fan-off {
    background: #222;
    color: white;
    border: 1px solid #444;
}

/* WARNING */

.warning {
    display: none;
    background: #220505;
    border: 1px solid #ff3b30;
    color: #ff5c55;
    padding: 18px;
    border-radius: 12px;
    margin-top: 20px;
    text-align: center;
    font-weight: bold;
}

/* FOOTER */

.footer {
    text-align: center;
    color: #555;
    padding: 25px;
    font-size: 13px;
}

/* MOBILE */

@media (max-width: 600px) {

    .header h1 {
        font-size: 23px;
    }

    .gas-value {
        font-size: 50px;
    }

    .status {
        font-size: 25px;
    }

    button {
        width: 85%;
        margin: 7px;
    }

}

</style>

</head>

<body>

<!-- HEADER -->

<div class="header">

<h1>Gas Safety Monitor</h1>

<p>
Gas leakage monitoring and exhaust fan control
</p>

</div>

<!-- MAIN -->

<div class="container">

<!-- GAS STATUS -->

<div class="status-card">

<div class="status-title">
Gas Status
</div>

<div id="status" class="status safe">
SAFE
</div>

<div id="gasValue" class="gas-value">
0
</div>

<div class="unit">
Sensor Value
</div>

</div>

<!-- INFORMATION CARDS -->

<div class="grid">

<!-- FAN -->

<div class="card">

<div class="card-title">
Exhaust Fan
</div>

<div id="fan" class="card-value off">
OFF
</div>

</div>

<!-- BUZZER -->

<div class="card">

<div class="card-title">
Buzzer
</div>

<div id="buzzer" class="card-value off">
OFF
</div>

</div>

<!-- GAS LIMIT -->

<div class="card">

<div class="card-title">
Gas Limit
</div>

<div class="card-value">
500
</div>

</div>

</div>

<!-- WARNING -->

<div id="warning" class="warning">

GAS LEAKAGE DETECTED

<br><br>

Exhaust fan is running automatically.

</div>

<!-- FAN CONTROL -->

<div class="control">

<h2>Exhaust Fan Control</h2>

<p>
Manual fan control is available when the gas level is safe.
</p>

<button class="fan-on"
onclick="fanOn()">

FAN ON

</button>

<button class="fan-off"
onclick="fanOff()">

FAN OFF

</button>

</div>

</div>

<!-- FOOTER -->

<div class="footer">

ESP32 IoT Gas Safety System

</div>

<script>

// =====================================================
// UPDATE DATA
// =====================================================

function updateData()
{

    fetch("/data")

    .then(response => response.json())

    .then(data =>
    {

        // GAS VALUE

        document.getElementById("gasValue")
        .innerHTML = data.gas;


        // GAS STATUS

        let status =
        document.getElementById("status");


        if (data.leakage)
        {

            status.innerHTML =
            "GAS LEAKAGE";

            status.className =
            "status danger";

            document.getElementById("warning")
            .style.display = "block";

        }

        else
        {

            status.innerHTML =
            "SAFE";

            status.className =
            "status safe";

            document.getElementById("warning")
            .style.display = "none";

        }


        // FAN

        let fan =
        document.getElementById("fan");


        if (data.fan)
        {

            fan.innerHTML = "ON";

            fan.className =
            "card-value on";

        }

        else
        {

            fan.innerHTML = "OFF";

            fan.className =
            "card-value off";

        }


        // BUZZER

        let buzzer =
        document.getElementById("buzzer");


        if (data.buzzer)
        {

            buzzer.innerHTML = "ON";

            buzzer.className =
            "card-value danger";

        }

        else
        {

            buzzer.innerHTML = "OFF";

            buzzer.className =
            "card-value off";

        }

    })

    .catch(error =>
    {
        console.log("Connection error");
    });

}


// =====================================================
// FAN ON
// =====================================================

function fanOn()
{

    fetch("/fan/on")

    .then(() =>
    {
        updateData();
    });

}


// =====================================================
// FAN OFF
// =====================================================

function fanOff()
{

    fetch("/fan/off")

    .then(() =>
    {
        updateData();
    });

}


// =====================================================
// UPDATE EVERY 1 SECOND
// =====================================================

setInterval(updateData, 1000);

updateData();

</script>

</body>

</html>

)rawliteral";

  server.send(
    200,
    "text/html",
    html
  );
}

// =====================================================
// SEND SENSOR DATA
// =====================================================

void handleData()
{
  String json = "{";

  json += "\"gas\":";
  json += String(gasValue);

  json += ",";

  json += "\"leakage\":";
  json += gasLeakage ? "true" : "false";

  json += ",";

  json += "\"fan\":";
  json += fanState ? "true" : "false";

  json += ",";

  json += "\"buzzer\":";
  json += buzzerState ? "true" : "false";

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

// =====================================================
// FAN ON FROM WEBSITE
// =====================================================

void handleFanOn()
{
  manualFan = true;

  automaticControl();

  server.send(
    200,
    "text/plain",
    "FAN ON"
  );
}

// =====================================================
// FAN OFF FROM WEBSITE
// =====================================================

void handleFanOff()
{
  readGasSensor();

  // ===================================================
  // SAFETY CHECK
  // ===================================================

  if (gasLeakage)
  {
    // Gas detected.
    // FAN CANNOT be turned OFF.

    manualFan = false;

    controlRelay(true);

    controlBuzzer(true);

    server.send(
      200,
      "text/plain",
      "GAS LEAKAGE - FAN CANNOT BE TURNED OFF"
    );
  }

  else
  {
    // Gas safe.
    // FAN can be turned OFF.

    manualFan = false;

    controlRelay(false);

    controlBuzzer(false);

    server.send(
      200,
      "text/plain",
      "FAN OFF"
    );
  }
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ===================================================
  // PIN SETUP
  // ===================================================

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // ===================================================
  // INITIAL STATE
  // ===================================================

  // Fan OFF
  controlRelay(false);

  // Buzzer OFF
  controlBuzzer(false);

  // ===================================================
  // ADC
  // ===================================================

  analogReadResolution(12);

  analogSetPinAttenuation(
    GAS_SENSOR_PIN,
    ADC_11db
  );

  // ===================================================
  // WIFI
  // ===================================================

  Serial.println();

  Serial.println(
    "Connecting to Wi-Fi..."
  );

  WiFi.begin(
    ssid,
    password
  );

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println(
    "WIFI CONNECTED!"
  );

  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  // ===================================================
  // WEB ROUTES
  // ===================================================

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/data",
    handleData
  );

  server.on(
    "/fan/on",
    handleFanOn
  );

  server.on(
    "/fan/off",
    handleFanOff
  );

  // ===================================================
  // START WEB SERVER
  // ===================================================

  server.begin();

  Serial.println(
    "Web Server Started!"
  );

  Serial.print(
    "Open in browser: http://"
  );

  Serial.println(
    WiFi.localIP()
  );
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Handle website requests
  server.handleClient();

  // ===================================================
  // SENSOR TIMER
  // ===================================================

  unsigned long currentMillis =
  millis();

  if (
    currentMillis - previousMillis
    >= sensorInterval
  )
  {
    previousMillis =
    currentMillis;

    // Automatic gas detection
    automaticControl();

    // =================================================
    // SERIAL MONITOR
    // =================================================

    Serial.print(
      "Gas: "
    );

    Serial.print(
      gasValue
    );

    Serial.print(
      " | Status: "
    );

    if (gasLeakage)
    {
      Serial.print(
        "GAS LEAKAGE"
      );
    }
    else
    {
      Serial.print(
        "SAFE"
      );
    }

    Serial.print(
      " | Fan: "
    );

    if (fanState)
    {
      Serial.print(
        "ON"
      );
    }
    else
    {
      Serial.print(
        "OFF"
      );
    }

    Serial.print(
      " | Buzzer: "
    );

    if (buzzerState)
    {
      Serial.println(
        "ON"
      );
    }
    else
    {
      Serial.println(
        "OFF"
      );
    }
  }
}
