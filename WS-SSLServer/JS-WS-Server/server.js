// ***** FIELDS AND INIT *****

const WebSocket = require("ws");
const port = process.env.PORT || 8080;
const authToken = process.env.AUTH_TOKEN;

// Create WebSocket server
const wss = new WebSocket.Server({ port });

// Set to store all connected clients
const clients = new Set();

// ***** METHODS *****

// Event triggered when a client connects
wss.on("connection", function connection(ws, req) {
  console.log("New WebSocket connection established");

  // Extract token from the request URL
  const url = new URL(req.url, `http://${req.headers.host}`);
  const token = url.searchParams.get("token");

  // Check if the token matches the expected authToken
  if (token !== authToken) {
    console.log("Unauthorized connection attempt");
    ws.close(1008, "Unauthorized");
    return;
  }

  // Add the client to the set of connected clients
  clients.add(ws);

  // Event triggered when connection with client is closed
  ws.on("close", function close() {
    console.log("WebSocket connection closed");
    // Remove the client from the set of connected clients when it disconnects
    clients.delete(ws);
  });

  // Event triggered when server receives a message from the client
  ws.on("message", function incoming(message) {
    console.log("Message received from client: %s", message);

    // Parse the JSON message received from the client
    let jsonMessage;
    try {
      jsonMessage = JSON.parse(message);
    } catch (error) {
      console.error("Error parsing JSON:", error);
      return;
    }

    // Send the parsed JSON message to all connected clients
    clients.forEach((client) => {
      // Check if the client is still connected before sending the message
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(jsonMessage));
      }
    });
  });
});
