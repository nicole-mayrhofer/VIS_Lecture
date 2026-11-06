package at.fhooe.sail.vis.socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class Primitive_SocketServer {

    public static void main(String[] _argv) throws IOException {
		System.out.println("Starting Primitive_SocketServer ... ");

        if (_argv.length != 1) {
            System.out.println("Usage: Primitive_SocketServer <PORT>");
            return;
        }

        int port = Integer.parseInt(_argv[0]);  //java hier wieder mit index 0 beginnen -- c++ = 1

        ServerSocket passiveSocket = new ServerSocket(port);
        // warum kann es hier zu einer Exception kommen? Zahl ist kein Port oder wenn der Port bereits belegt ist

        System.out.println("Server is waiting for clients on port " + port);

        boolean accepting = true;
        while (accepting) {
            Socket activeSocket = passiveSocket.accept();
            System.out.println("Client connected...");

            InputStream in = activeSocket.getInputStream();
            OutputStream out = activeSocket.getOutputStream();

            byte [] buffer = new byte [1024];
            while (true) {
                int rVal = in.read(buffer);
                if(rVal < 0) {
                    System.out.println("connection killed...");
                    break;
                } else if (rVal == 0) {
                    System.out.println("client disconnected...");
                    break;
                } else {
                    String msg = new String(buffer, 0, rVal);
                    if (msg.equalsIgnoreCase("drop")) {
                        System.out.println("receive drop command...");
                    } else if (msg.equalsIgnoreCase("shutdown")) {
                        System.out.println("receive shutdown command...");
                    } else {
                        System.out.println("Message from client " + msg);
                        String reply = "ECHO: " + msg;
                        out.write(reply.getBytes());
                        out.flush();
                    }
                }
            } // while true
            out.close();
            in.close();
            activeSocket.close();
        }   // while accepting
        passiveSocket.close();


		System.out.println("Stopping Primitive_SocketServer ... ");
    } // main
}
