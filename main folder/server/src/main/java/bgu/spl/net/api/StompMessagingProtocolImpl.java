package bgu.spl.net.api;

import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;
import java.util.ArrayList;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {

    private int connectionId;
    private ConnectionsImpl<String> connections;
    private boolean shouldTerminate;

    public void start(int connectionId, Connections<String> connections) {// who calls start()?
        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl<String>) connections;
        shouldTerminate = false;
    }

    public void process(String message) {
        ArrayList<String> words = new ArrayList<String>();
        String word = "";
        boolean isValue = false;
        // notice: if it is a SEND frame, changes need to be made
        for (int c = 0; c < message.length(); c++) {// haha c++ but it's java
            if (message.charAt(c) == ':')
                isValue = true;
            if (isValue) {
                if (message.charAt(c) != '\n' && message.charAt(c) != '\u0000')
                    word += message.charAt(c);
                else {
                    words.add(word);
                    word = "";
                    isValue = false;
                }
            }

        }

        String type = words.get(0);

        switch (type) {
            case "CONNECT":
                String username = words.get(3);
                String password = words.get(4);
                int returnVal = connections.connect(username, password);
                //maybe ERROR
                String connected = "CONNECTED" + '\n' + "version:1.2" + '\n' + '\n' + '\u0000';
                connections.send(connectionId, connected);

                break;

            case "SEND":
                String channel1 = words.get(1).substring(1);

                // TODO: Implement
                String msg = "MESSAGE" + '\n';
                int subscriptionId = 0;
                // go over my user's channels and
                ConcurrentHashMap<Integer, String> userSubs = connections.ConIdToUser.get(connectionId)
                        .getSubIdToChanName();
                for (Integer key : userSubs.keySet()) {
                    if (userSubs.get(key).equals(channel1))
                        subscriptionId = key;
                }
                msg += "subscription:" + subscriptionId + '\n';
                msg += "message-id:" + connections.getMessageId() + '\n';
                msg += "destination:/" + channel1 + '\n' + '\n';
                msg += words.get(2) + '\n' + '\u0000';
                connections.send(channel1, msg);
                break;

            case "SUBSCRIBE":
                String channel2 = words.get(1).substring(1);
                connections.subscribe(channel2, connectionId);

                String receipt1 = "RECEIPT" + '\n' + "receipt-id:" + words.get(3) + '\n' + '\n' + '\u0000';
                connections.send(connectionId, receipt1);
                break;

            case "UNSUBSCRIBE":
                int subId = Integer.parseInt(words.get(1));
                connections.ConIdToUser.get(connectionId).unsubscribe(subId);
                String channel3 = connections.ConIdToUser.get(connectionId).getSubIdToChanName().get(subId);
                connections.unsubscribe(channel3, connectionId);

                String receipt2 = "RECEIPT" + '\n' + "receipt-id:" + words.get(2) + '\n' + '\n' + '\u0000';
                connections.send(connectionId, receipt2);
                break;

            case "DISCONNECT":
                shouldTerminate = true;
                connections.disconnect(connectionId);
                break;
            //add ERROR
        }
    }

    public boolean shouldTerminate() {
        return this.shouldTerminate;
        //implement actual closing (terminate function)
    }
}
