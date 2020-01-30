package com.rogueprogrammers.iotswitchclient

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import android.widget.ToggleButton
import com.github.nkzawa.socketio.client.IO
import com.github.nkzawa.socketio.client.Socket

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val textView = findViewById<TextView>(R.id.textView)
        val toggleButton = findViewById<ToggleButton>(R.id.toggleButton)
        textView.setText(R.string.connecting)
        toggleButton.isEnabled = false
        val socket = IO.socket("https://iot-switch-server.herokuapp.com")
        socket.on(Socket.EVENT_CONNECT) {
            runOnUiThread {
                toggleButton.isEnabled = true
                textView.setText(R.string.connected)
            }
        }
        socket.on(Socket.EVENT_DISCONNECT) {
            runOnUiThread {
                toggleButton.isEnabled = false
                textView.setText(R.string.disconnected)
            }
        }
        socket.on("state") { args ->
            val state = args[0]
            runOnUiThread {
                toggleButton.isEnabled = true
                if (state == "on")
                    toggleButton.isChecked = true
                else if (state == "off")
                    toggleButton.isChecked = false
            }
        }
        toggleButton.setOnCheckedChangeListener { _, isChecked ->
            toggleButton.isEnabled = false
            if (isChecked)
                socket.emit("update", "on")
            else
                socket.emit("update", "off")
        }
        socket.connect()
    }

}
