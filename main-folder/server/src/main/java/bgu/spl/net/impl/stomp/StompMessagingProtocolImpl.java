package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;
import bgu.spl.net.srv.User;

import java.util.ArrayList;

public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T> {

    private int connectionId;
    private ConnectionsImpl<T> connections;
    private boolean shouldTerminate;
    private User user = null;
    // private ArrayList<String> allowedPrefix;

    public void start(int connectionId, Connections<T> connections) {
        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl<T>) connections;
        shouldTerminate = false;
    }

    public T process(T frame) {

        System.out.println("I received the frame: " + (String) frame);

        String message = (String) frame;
        ArrayList<String> words = new ArrayList<String>();
        stringToWords(words, message);
        String type = words.get(0);

        for(String line : words){
            System.out.println(line);
        }

        switch (type) {
            case "CONNECT":

                String username = words.get(3);
                String password = words.get(4);
                int returnVal = connections.connect(connectionId, username, password);
                if (returnVal != 0) {
                    ERROR(returnVal);
                } else {
                    String connected = "CONNECTED\nversion:1.2\n\n";
                    System.out.println("Server is trying to send CONNECTED frame");
                    connections.send(connectionId, (T) connected);
                    user = connections.getUser(connectionId);
                }
                break;

            case "SEND":

                if (user == null) {
                    ERROR(5);
                    break;
                }

                String channel1 = words.get(2).substring(1);

                if (!connections.isSubscribed(connectionId, channel1)) {
                    ERROR(3);
                } else {
                    String msg = "MESSAGE ";
                    msg += "subscription:";
                    msg += "message-id:" + connections.getMessageId();
                    msg += "destination:/" + channel1 +"\n\n"; //NOTICE WHEN TESTING WITH ECHOCLIENT.JAVA
                    for (int i = 4; i < words.size(); i++) {
                        msg += words.get(i);
                    }
                    connections.send(channel1, (T) msg);
                }
                break;

            case "SUBSCRIBE":
                if (user == null) {
                    ERROR(5);
                    break;
                }

                Integer subId = Integer.parseInt(words.get(2));
                String channel2 = words.get(1).substring(1);
                boolean legalSub = connections.subscribe(subId, channel2, connectionId);

                if (legalSub) {
                    String receipt1 = "RECEIPT receipt-id:" + words.get(3) + '\n';
                    connections.send(connectionId, (T) receipt1);
                } else {
                    ERROR(6);
                }
                break;

            case "UNSUBSCRIBE":
                if (user == null) {
                    ERROR(5);
                    break;
                }
                int subId2 = Integer.parseInt(words.get(1));
                String channelName = user.getSubName(subId2);
                boolean wasSubscribed = false;
                if (channelName != null)
                    wasSubscribed = connections.unsubscribe(subId2, channelName, connectionId);
                if (wasSubscribed) {
                    String receipt2 = "RECEIPT" + '\n' + "receipt-id:" + words.get(2) + '\n' + '\n';
                    connections.send(connectionId, (T) receipt2);
                } else {
                    ERROR(3);
                }

                break;

            case "DISCONNECT":
                if (user == null) {
                    ERROR(5);
                    break;
                }
                String receipt = "RECEIPT receipt-id:-1" + '\n';
                connections.send(connectionId, (T) receipt);
                connections.disconnect(connectionId);
                shouldTerminate = true;
                break;

            default:
                ERROR(4);

        }
        return (T) message; // just to satisfy the interface
    }

    private void stringToWords(ArrayList<String> words, String inFrame) {
        String word = "";
        boolean isValue = false;
        int firstLineEnding = inFrame.indexOf('\n');
        words.add(inFrame.substring(0, firstLineEnding));
        if(words.get(0) != "SEND"){
            for (int c = firstLineEnding; c < inFrame.length(); c++) {// haha c++ but it's java
                if (inFrame.charAt(c) == ':')
                    isValue = true;
                else if (isValue) {
                    if (inFrame.charAt(c) != '\n' && inFrame.charAt(c) != '\u0000')
                        word += inFrame.charAt(c);
                    else {
                        words.add(word);
                        word = "";
                        isValue = false;
                    }
                }
            }
        }
        else{
            words.remove(0);
            String[] lines = inFrame.split("\n");
            for(String line : lines){
                for(String singleWord : line.split(":")){
                    words.add(singleWord);
                }
            }
        }
    }


    private void ERROR(int type) {
        int magic_number = (int)(Math.random() * 17000);
        String frame = "ERROR\nreceipt-id:" + magic_number + "\n";
        String errorMessage = "";

        if (type == 0) {
            errorMessage = "Socket Error\n";
        }
        if (type == 1) {
            errorMessage = "Wrong Password\n";
        } else if (type == 2) {
            errorMessage = "user already logged in\n";
        } else if (type == 3) {
            errorMessage = "User not subscribed to this channel\n";
        } else if (type == 4) {
            errorMessage = "Input is not valid\n";
        } else if (type == 5) {
            errorMessage = "Client needs to connect first\n";
        } else {
            errorMessage = "Invalid Subscription info";
        }

        frame += errorMessage + "\n";

        connections.send(connectionId, (T) frame);
        connections.disconnect(connectionId);
        shouldTerminate = true;
    }

    public boolean shouldTerminate() {
        return this.shouldTerminate;
        // implement actual closing (terminate function)
    }
}
