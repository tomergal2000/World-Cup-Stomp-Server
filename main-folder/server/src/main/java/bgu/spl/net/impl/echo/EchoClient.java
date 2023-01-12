package bgu.spl.net.impl.echo;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class EchoClient {

    public static void main(String[] args) throws IOException {

        String testSUB = "SUBSCRIBE\ndestination:/germany_spain\nid:0\nreceipt:73\n\n\u0000";
        String testCON = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:films\n\n\u0000";
        String testUNS = "UNSUBSCRIBE\nid:0\nreceipt:82\n\n\u0000";
        String testDIS = "DISCONNECT\nreceipt:113\n\n\u0000";
        String testCONERROR = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:mahasisma\n\n\u0000";
        String testSEND = "SEND\ndestination:/germany_spain\nuser:meni\nteam a:germany\nteam b:spain\nevent name:kickoff\ntime:0\n\u0000";

        if (args.length == 0) {
            args = new String[]{"localhost", testCON};
        }

        if (args.length < 2) {
            System.out.println("you must supply two arguments: host, message");
            System.exit(1);
        }

        //BufferedReader and BufferedWriter automatically using UTF-8 encoding
        try (Socket sock = new Socket(args[0], 7777);
                BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {

            System.out.println("sending CONNECT message to server");
            out.write(args[1]);
            // out.newLine();
            out.flush();

            System.out.println("awaiting response");
            String line = in.readLine();
            System.out.println("message from server: ");
            System.out.println(line);

            System.out.println("sending SUBSCRIBE message to server: ");
            out.write(testSUB);
            // out.newLine();
            out.flush();

            line = in.readLine();
            System.out.println(line);

            System.out.println("sending SEND message to server: ");
            out.write(testSEND);
            // out.newLine();
            out.flush();

            line = in.readLine();
            System.out.println(line);
            
            // System.out.println("sending DISCONNECT message to server: ");
            // out.write(testDIS);
            // // out.newLine();
            // out.flush();

            // line = in.readLine();
            // System.out.println(line);

        }
        // try (Socket sock = new Socket(args[0], 7777);
        //     BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
        //     BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {

        //     System.out.println("sending CONNECT message to server");
        //     out.write(testCONERROR);
        //     // out.newLine();
        //     out.flush();

        //     System.out.println("awaiting response");
        //     String line = in.readLine();
        //     System.out.println("message from server: ");
        //     System.out.println(line);
        // }
    }
}