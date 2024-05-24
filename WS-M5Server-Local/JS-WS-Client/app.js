/*
  npm run start in CLI to launch server
*/

// **********************************************
// Imports
// **********************************************

const WebSocket = require("ws");
const fs = require("fs");

// **********************************************
// WebSocket setup
// **********************************************

const ws = new WebSocket("ws://*******ATOM_IP*******:81");

// **********************************************
// Tools for decoding the JSON message
// **********************************************

let saving = false;

function parseDistance(data) {
  try {
    // Parsing data
    const jsonData = JSON.parse(data);
    console.log(jsonData);
    // Saving toogle with special messages
    if (jsonData.specialMsg === "SAVE_START") {
      saving = true;
      return;
    } else if (jsonData.specialMsg === "SAVE_END") {
      saving = false;
      return;
    }
    // Save distances in a .csv file
    if (saving) {
      const csvData = `${jsonData.sensor1},${jsonData.sensor2}\n`;
      // Append data in distances.csv
      fs.appendFile("./distances.csv", csvData, (err) => {
        if (err) {
          console.error("Error writing to file:", err);
        } else {
          console.log("Data saved to distances.csv");
        }
      });
    }
  } catch (error) {
    console.error("Error parsing JSON data:", error);
  }
}

// **********************************************
// Received Message Event
// **********************************************

ws.on("message", (data) => {
  parseDistance(data);
});

// **********************************************
// Open Connexion Event
// **********************************************

ws.on("open", () => {
  console.log("WebSocket connection established");
  const message = "Hello from client !";
  ws.send(message);
});

// **********************************************
// Close Connexion Event
// **********************************************

ws.on("close", () => {
  console.log("WebSocket connection closed");
});

// **********************************************
// Error Event
// **********************************************

ws.on("error", (error) => {
  console.error("WebSocket error:", error);
});
