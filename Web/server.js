// Serving up a frontend for our attendace gate
// through the magic of express and nodejs
var http = require('http');
var express = require('express');
const app = express();

const PORT = 8080;

// Access files in the html directory
app.use(express.static(__dirname + '/html'));

/* This is the template for handling an HTML file request
app.get('urlPath', function(req, res) {
    res.sendFile('filename')
});
*/

app.get('/', function(req, res) {
    res.sendFile('login.html', {root: __dirname + "/html"});
});

// -TODO- Front page says login, there is absolutely no security here, nor is there any data collection...
app.get('/attendance', function(req, res) {
    res.sendFile('attendance.html', {root: __dirname + "/html"});
});

app.listen(PORT, () => {
    console.log("Server started! Listening on " + PORT);
});