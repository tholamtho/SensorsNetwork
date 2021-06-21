var mysql = require("mysql");
var mqtt = require("mqtt");
var express = require("express");
var app = express();
app.use(express.static("./public"));
app.set("view engine", "ejs");
app.set("views", "./views");
var server = require("http").Server(app);

var DHT;
DHT = '{"Temp":"11", "Humi":50}';
var object = JSON.parse(DHT);
console.log("Temp:" + object.Temp);
var io = require("socket.io")(server);
server.listen(3000);

io.on("connection", function(socket) {
    console.log("Someone Logged In: " + socket.id);

    socket.on("disconnect", function() {
        console.log("Disconnected: " + socket.id);
    });
    socket.on("Client-send-data", function(data) {
        console.log(data);
        io.sockets.emit("Server-send-data", DHT);
    });
});


app.get("/", function(req, res) {
    res.render("Mainpage");
});