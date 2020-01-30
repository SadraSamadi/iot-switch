package com.rogueprogrammers.iotswitch

import android.os.Bundle
import android.widget.TextView
import android.widget.ToggleButton
import androidx.appcompat.app.AppCompatActivity
import com.github.nkzawa.socketio.client.IO
import com.github.nkzawa.socketio.client.Socket

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        // Get state text view reference.
        val textView = findViewById<TextView>(R.id.textView)
        // Get state button reference.
        val toggleButton = findViewById<ToggleButton>(R.id.toggleButton)
        textView.setText(R.string.connecting)
        toggleButton.isEnabled = false
        // Connect to server socket.
        val socket = IO.socket("https://iot-switch-server.herokuapp.com/")
        // Wait for connection.
        socket.on(Socket.EVENT_CONNECT) {
            runOnUiThread {
                // Enable button and show state.
                toggleButton.isEnabled = true
                textView.setText(R.string.connected)
            }
        }
        // Listen to disconnection from server socket.
        socket.on(Socket.EVENT_DISCONNECT) {
            runOnUiThread {
                // Disable button and show state.
                toggleButton.isEnabled = false
                textView.setText(R.string.disconnected)
            }
        }
        // Listen to state event.
        socket.on("state") { args ->
            val state = args[0]
            runOnUiThread {
                // Get state and update ui.
                toggleButton.isEnabled = true
                if (state == "on")
                    toggleButton.isChecked = true
                else if (state == "off")
                    toggleButton.isChecked = false
            }
        }
        // Listen to button event.
        toggleButton.setOnCheckedChangeListener { _, isChecked ->
            toggleButton.isEnabled = false
            // Toggle state and send it to server for updating.
            if (isChecked)
                socket.emit("update", "on")
            else
                socket.emit("update", "off")
        }
        // Start connection.
        socket.connect()
    }

}
