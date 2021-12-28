var express = require("express");
var cors = require("cors");
var morgan = require("morgan");
var app = express();
var http = require("http");
var server = http.createServer(app);
const mqtt = require("mqtt");
const cookieParser = require("cookie-parser");
const PORT = 5000;

app.use(express.json());

app.use(
  cors({
    origin: true,
  }),
);

app.use(morgan("dev"));
app.use(cors());
app.use(cookieParser());

// MQTT configuration //////////////////////////////////////////////////////////

var options = {
  // https://stackoverflow.com/a/36460844/4378475
  // port: 1883, 1883 for non secure port, 8883 for secure ssl port, 443 for websocket port
  // host: 'mqtt://fantastic-hairdresser.cloudmqtt.com',
  clientId: "mqttjs_" + Math.random().toString(16).substr(2, 8),
  // username: '',
  // password: '',
  keepalive: 60,
  reconnectPeriod: 1000,
  protocolId: "MQIsdp",
  protocolVersion: 5,
  clean: true,
  encoding: "utf8",
};

// if connecting with secure port start with 'mqtts:// ...' instead, and for socket port start with "wss:// ..."

const URi =
  "mqtts://your_mqtt_token_here@mqtt.flespi.io:8883";
var mqttClient = mqtt.connect(URi, options);

mqttClient.on("connect", function () {
  console.log("mqtt client is connected to broker");
});

mqttClient.on("error", function (err) {
  // mqttClient.subscribe('Topic07');
  console.log("mqtt Connection error: ", err);
});

// mqttClient.subscribe('turnBulb', (data) => {
//     console.log('mqtt client has subscribed to topic turnBulb successfully: ', data);
// });

mqttClient.on("message", function (topic, message) {
  console.log("mqtt message received on client", message.toString()); //if toString is not given, the message comes as buffer
});

// API's ///////////////////////////////////////////////////////////////////////

// temp-route
app.put("/temp-route", (req, res, next) => {
  if (!req.body || !req.body.topicName || !req.body.payload) {
    res.status(400).send(`Info is missing provide topicName and message
    Eg:{
      "topicName": 132151531,
      "payload": {
        "sensor": "Distance",
        "time": 1351824120,
        "data": [
          48.75608,
          2.302038
        ]
      }
    }`);
    console.log("Information is missing");
    return;
  }

  // Publishing a message to MQTT client
  mqttClient.publish(
    req.body.topicName.toLowerCase(),
    Buffer.from(JSON.stringify(req.body.payload)),
    function () {
      console.log("a topic is published from client ");
    },
  );
  res.send({ message: "success" })
});

// Server listening ////////////////////////////////////////////////////////////
server.listen(PORT, () => {
  console.log("Server is Running:", PORT);
});