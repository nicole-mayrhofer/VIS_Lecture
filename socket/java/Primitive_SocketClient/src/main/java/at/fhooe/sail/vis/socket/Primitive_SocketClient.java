package at.fhooe.sail.vis.socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Scanner;

public class Primitive_SocketClient {

	public Primitive_SocketClient(String _ip, int _port) throws IOException {  // für Übung:
		Socket activeSocket = new Socket(_ip, _port);
		InputStream in = activeSocket.getInputStream();
		OutputStream out = activeSocket.getOutputStream();

		byte [] buffer = new byte[1024];
		Scanner scan = new Scanner(System.in);

		while (true) {
			System.out.println("Eingabe: ");
			String msg = scan.nextLine();

			if (msg.length() == 0) {
				System.out.println("no empty messages!");
				continue;
			} else {
				// check if quit cmd was provided
				if(msg.equalsIgnoreCase("quit")) {
					System.out.println("shutting down client...");
					break;
				}

				out.write(msg.getBytes());
				out.flush();

				int rVal = in.read(buffer);
				// check <0 and ==0
				String reply = new String(buffer, 0, rVal);
				System.out.println("message from server: " + reply);
			}
		} // while true
		out.close();
		in.close();
		activeSocket.close();
	}

	public static void main(String[] _argv) throws IOException {
        System.out.println("Starting Primitive_SocketClient ...");

		if (_argv.length != 2) {
			System.out.println("Usage: Primitive_SocketClient <IP> <PORT>");
			return;
		}

		int port = Integer.parseInt(_argv[1]);
		Primitive_SocketClient client = new Primitive_SocketClient(_argv[0], port);

        System.out.println("Stopping Primitive_SocketClient ...");
    }
}
