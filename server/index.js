const http = require('http');
const express = require('express');
const socket_io = require('socket.io');
const { BehaviorSubject } = require('rxjs');

const port = parseInt(process.env.PORT) || 3303;
const app = express();
const server = http.createServer(app);
const io = socket_io(server);
const state = new BehaviorSubject('off');

io.on('connection', client => {
    console.log('client connected');
    let subscription = state.subscribe(s => {
        client.emit('state', s);
        console.log(`state emitted: ${s}`);
    });
    client.on('update', s => {
        console.log(`updating state: ${s}`);
        state.next(s);
    });
    client.on('disconnect', () => {
        subscription.unsubscribe();
        console.log('client disconnected');
    });
});

server.listen(port, () => console.log(`server started on port ${port}`));
