var mysql = require('mysql');


var mqtt = require('mqtt');
var express = require('express');
var app = express();

app.use(express.static("./public"));
app.set("view engine", "ejs");
app.set("views", "./views");
var server = require("http").Server(app);
var settings = {
    mqttServerUrl: "broker.hivemq.com",
    port: 1883,
    username: "Asura",
    password: "Tholamtho123",
    topic: "ESP_data"
}

var con = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "",
    database: "example"
});

var client = mqtt.connect('mqtt://' + settings.mqttServerUrl + ":" + settings.port);

var led_red;
var led_green;
var led_blue;
var temp;
var humi;
var buzzer_value;
var esp;

con.connect(function(err) {
    if (err) throw err;
    console.log(" Database Connected!");
});
var io = require("socket.io")(server);
server.listen(3000);



io.on("connection", function(sockets) {
    console.log("Someone Logged In: " + sockets.id);

    sockets.on("disconnect", function() {
        console.log("Disconnected: " + sockets.id);
    });
    client.on('connect', function() {
        client.subscribe(settings.topic)
        console.log("Subscribed topic " + settings.topic);
        client.on('message', function(topic, message) {
            console.log(JSON.parse(message));
            temp = JSON.parse(message).Temp;
            humi = JSON.parse(message).Humi;
            console.log(humi);
            console.log(temp);
            console.log("End MQTT");
            esp = JSON.stringify({ Temp: temp, Humi: humi });
            io.sockets.emit("DHT11_Data", esp);

        });
    });



    sockets.on("Led_Data", function(data) {
        var i = 0;
        setTimeout(function() {
            console.log("Received From Web:", data);

            var web_data = JSON.parse(data);

            client.publish("Web_data", data);
            led_red = web_data.RED;
            led_green = web_data.GREEN;
            led_blue = web_data.BLUE;
            buzzer_value = web_data.BUZZER;
            var sql = "INSERT INTO test ( temperature,humidity, ledred, ledblue, ledgreen,buzzer)" + "VALUES ('" + temp + "','" + humi + "','" + led_red + "', '" + led_blue + "','" + led_green + "','" + buzzer_value + "')";

            con.query(sql, function(err, result) {
                if (err) throw err;
                console.log("1 record inserted");
            });
        }, 3000);

    });

    setTimeout(function() {


    });
});


app.get("/", function(req, res) {
    app.use(express.static(__dirname + '/views'));
    res.render("Mainpage");
});