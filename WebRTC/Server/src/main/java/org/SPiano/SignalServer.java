package org.SPiano;

import org.java_websocket.WebSocket;
import org.java_websocket.handshake.ClientHandshake;
import org.java_websocket.server.WebSocketServer;

import java.net.InetSocketAddress;

public class SignalServer extends WebSocketServer {
    public SignalServer(int port) {
        super(InetSocketAddress.createUnresolved("127.0.0.1", port));
    }

    @Override
    public void onOpen(WebSocket webSocket, ClientHandshake clientHandshake) {
        System.out.println("New connection established"+webSocket.getRemoteSocketAddress());
    }

    @Override
    public void onClose(WebSocket webSocket, int i, String s, boolean b) {
        System.out.println("Connection closed"+webSocket.getRemoteSocketAddress());
    }

    @Override
    public void onMessage(WebSocket webSocket, String s) {
        // Broadcast the message to all clients except the sender
    }

    @Override
    public void onError(WebSocket webSocket, Exception e) {
        e.printStackTrace();
    }

    @Override
    public void onStart() {
        System.out.println("Starting SignalServer");
    }
}
