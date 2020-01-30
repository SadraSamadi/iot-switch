const http = require('http');
const express = require('express');
const SocketIO = require('socket.io');
const { BehaviorSubject } = require('rxjs');

// Get prot number from environment or set it to the specified.
const port = parseInt(process.env.PORT) || 3303;
// Create a web listener.
const app = express();
// Create a web server.
const server = http.createServer(app);
// Create a socket server.
const io = SocketIO(server);
// Create a wrapper for the state.
const subject = new BehaviorSubject('off');

// Set a default route page to check the state.
app.get('/', (req, res) => res.end(`IoT Socket Server (State: ${subject.value})`));

// Listen to the state and send it to all clients on any change.
subject.subscribe(state => sendState(io, state));

// Send state to socket.
function sendState(socket, state) {
  socket.emit('state', state);
  console.log(`state emitted: ${state}`);
}

// Listen to socket for a new connection.
io.on('connection', socket => {
  console.log('client connected');
  sendState(socket, subject.value);
  // Listen to client for update event.
  socket.on('update', state => {
    // Change the state to new value.
    console.log(`updating state: ${state}`);
    subject.next(state);
  });
  socket.on('disconnect', () => console.log('client disconnected'));
});

// Start server with the port.
server.listen(port, () => console.log(`server started on port ${port}`));
