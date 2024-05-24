/*
  npm run start in CLI to launch server
*/

// **********************************************
// Imports
// **********************************************

const express = require("express");
const fs = require("fs");

// **********************************************
// Start HTTP Server
// **********************************************

const app = express();
const port = 8000;

// **********************************************
// Middleware to analyse and parse 
// request body in JSON 
// **********************************************

app.use(express.json());

// **********************************************
// Route handling for Post request on "/"
// **********************************************

app.post("/", (req, res) => {
  // Data from Atom Lite
  const data = req.body; 
  // Print the data
  console.log("Distance 1 : " + data.sensor1 + " \t|\t Distance 2 : " + data.sensor2);
  res.send("Data received with success !");
});

// **********************************************
// Route handling for Post request on "/save"
// **********************************************

app.post("/save", (req, res) => {
  // Data from Atom Lite
  const data = req.body; 
  // Format the data as CSV
  const csvData = `${data.sensor1},${data.sensor2}\n`; 
  // Append data in distances.csv
  fs.appendFile("./distances.csv", csvData, (err) => {
    if (err) {
      console.error("Error writing to file:", err);
      res.status(500).send("Error writing to file");
    } else {
      console.log("Data saved to distances.csv");
      res.send("Data saved with success !");
    }
  });
});

// **********************************************
// Route handling for Get request on "/"
// **********************************************

app.get("/", (req, res) => {
  res.send("Express Server running");
});

// **********************************************
// Express server listening
// **********************************************

app.listen(port, () => {
  console.log(`Express Server running on port ${port}`);
});
