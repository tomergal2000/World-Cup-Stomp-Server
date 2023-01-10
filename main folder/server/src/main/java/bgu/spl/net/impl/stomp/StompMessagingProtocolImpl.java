package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;
import java.util.ArrayList;

public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T> {

    private int connectionId;
    private ConnectionsImpl<T> connections;
    private boolean shouldTerminate;
    // private ArrayList<String> allowedPrefix;

    public void start(int connectionId, Connections<T> connections) {// who calls start()?
        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl<T>)connections;
        shouldTerminate = false;
        // allowedPrefix.add("CONNECT");
        // allowedPrefix.add("SEND");
        // allowedPrefix.add("SUBSCRIBE");
        // allowedPrefix.add("UNSUBSCRIBE");
        // allowedPrefix.add("DISCONNECT");

    }

    public T process(T frame) {
        
        String message = (String) frame;
        ArrayList<String> words = new ArrayList<String>();
        String word = "";
        boolean isValue = false;
        stringToWords(words, message);

        String type = words.get(0);

        switch (type) {
            case "CONNECT":
                String username = words.get(3);
                String password = words.get(4);
                int returnVal = connections.connect(connectionId, username, password);
                if(returnVal != 0){
                    ERROR(returnVal);
                }
                else{
                String connected = "CONNECTED" + '\n' + "version:1.2" + '\n' + '\n' + '\u0000';
                connections.send(connectionId, (T)connected);
                }
                break;

            case "SEND":
                String channel1 = words.get(1).substring(1);

                if(!connections.isSubscribed(connectionId, channel1)){
                    ERROR(3);
                }
                else{
                String msg = "MESSAGE\n";
                // int subscriptionId = -1;
                // go over my user's channels and find the right subId
                // ConcurrentHashMap<Integer, String> userSubs = connections.ConIdToUser.get(connectionId)
                //         .getSubIdToChanName();
                // for (Integer key : userSubs.keySet()) {
                //     if (userSubs.get(key).equals(channel1))
                //         subscriptionId = key;
                // }
                msg += "subscription:";
                msg += "message-id:" + connections.getMessageId() + '\n';
                msg += "destination:/" + channel1 + '\n' + '\n';
                for(int i = 2; i < words.size(); i++){
                    msg += words.get(i) + '\n' + '\u0000';
                }
                connections.send(channel1, (T)msg);
                }
                break;

            case "SUBSCRIBE":

                String channel2 = words.get(1).substring(1);
                boolean didSomething = connections.subscribe(channel2, connectionId);

                if(didSomething){
                String receipt1 = "RECEIPT" + '\n' + "receipt-id:" + words.get(3) + '\n' + '\n' + '\u0000';
                connections.send(connectionId, (T)receipt1);
                }
                break;

            case "UNSUBSCRIBE":
                int subId = Integer.parseInt(words.get(1));
                String channelName = connections.ConIdToUser.get(connectionId).getChannelList().get(subId);
                connections.ConIdToUser.get(connectionId).unsubscribe(channelName);
                boolean wasSubscribed= connections.unsubscribe(channelName, connectionId);
                if(wasSubscribed){
                    String receipt2 = "RECEIPT" + '\n' + "receipt-id:" + words.get(2) + '\n' + '\n' + '\u0000';
                    connections.send(connectionId, (T)receipt2);
                }
                else{
                    ERROR(3);
                }
                
                break;

            case "DISCONNECT":
                String receipt = "RECEIPT" + '\n' + "receipt-id:-1"  + '\n' + '\n' + '\u0000';
                connections.send(connectionId, (T)receipt);
                connections.disconnect(connectionId);
                shouldTerminate = true;
                break;
            
            default:
                ERROR(4);

        }
        return (T) message; //just to satisfy the interface
    }

    private void stringToWords(ArrayList<String> words, String inFrame){
        String word = "";
        boolean isValue = false;
        int firstLineEnding = inFrame.indexOf('\n');
        words.add(inFrame.substring(0, firstLineEnding));
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

    // private boolean InputError1(T frame) {
    //     String check = (String) frame;
    //     if(check.length() < 11)
    //         return false;
    //     if(!allowedPrefix.contains(check.substring(0, 10)))

    //     return true;

    // }

    private void ERROR(int type){
        
        String frame = "ERROR\n";
        String errorMessage = "";
        
        if(type == 0){
            errorMessage = "Socket Error\n";
        }
        if(type == 1){
            errorMessage = "Wrong Password\n";
        }
        else if(type == 2){
            errorMessage = "user already logged in\n";
        }
        else if(type == 3){
            errorMessage = "User not subscribed to this channel\n";
        } 
        else{
            errorMessage = "Input is not valid\n";
        }

        frame += errorMessage + "\n" + '\u0000';

        connections.send(connectionId, (T)frame);
        connections.disconnect(connectionId);
        shouldTerminate = true;
    }

    public boolean shouldTerminate() {
        return this.shouldTerminate;
        //implement actual closing (terminate function)
    }
}
