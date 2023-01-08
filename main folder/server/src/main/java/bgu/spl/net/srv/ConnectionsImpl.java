package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentHashMap<Integer, ConnectionHandler<T>> ConIdToHandler;
    private ConcurrentHashMap<String, ArrayList<User>> ChanNameToUserList;
    public  ConcurrentHashMap<Integer, User> ConIdToUser;
    private ArrayList<User> users;
    private int connectionCounter;
    private int messageIdCounter;

    public ConnectionsImpl() {
        ConIdToHandler = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
        ChanNameToUserList = new ConcurrentHashMap<String, ArrayList<User>>();
        ConIdToUser = new ConcurrentHashMap<Integer, User>();
        users = new ArrayList<User>();
        connectionCounter = 0;
        messageIdCounter = 0;

    }

    public boolean send(int connectionId, T msg) {

        ConnectionHandler<T> handler = ConIdToHandler.get(connectionId);

        if (handler == null)
            return false;

        handler.send(msg);

        return true;
    }

    public void send(String channel, T msg) {
        // TODO: when client wants to send message but he is not subs.:
        // we need to diconnect him and send error frame

        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        
        for (User user : subscribedUsers) {
            ConnectionHandler<T> handler = ConIdToHandler.get(user.getConId());
            handler.send(msg);
        }
    }

    public void disconnect(int connectionId) {
        ConIdToUser.get(connectionId).unsubscribeAll();
        ConIdToHandler.remove(connectionId);//maybe somehow kill the handler?
        ConIdToUser.remove(connectionId);
    }

    public void addHandler(ConnectionHandler handler) {
        ConIdToHandler.put(connectionCounter++, handler);
    }

    // assumes that client is not already logged in
    public int connect(String username, String password) {
        // 0 - success - new user --> does it matter?
        // 1 - wrong password
        // 2 - user already logged in
        // 3 - success - existing user
        User toCheck = userExists(username);

        if (toCheck == null) {// means this user is new
            createNewUser(username, password);
            // send CONNECTED frame
            return 0;
        } else if (toCheck.getPassword() != password)
            return 1;

        else if (ConIdToUser.contains(toCheck))
            return 2;

        ConIdToUser.put(connectionCounter, toCheck);
        toCheck.connect(connectionCounter);
        // send CONNECTED frame
        return 3;
    }

    public boolean subscribe(String channel, Integer conId) {
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        User user = ConIdToUser.get(conId);
        if (subscribedUsers == null) {
            ArrayList<User> chanToAdd = new ArrayList<User>();
            chanToAdd.add(user);
            ChanNameToUserList.put(channel, chanToAdd);
            return true;
        } else{
            if (!subscribedUsers.contains(user)){
                subscribedUsers.add(user);
                return true;
            }
            else{
                return false;
            }
        }
    }

    public void unsubscribe(String channel, Integer conId) {
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        if (subscribedUsers != null) {
            User user = ConIdToUser.get(conId);
            subscribedUsers.remove(user);
        }
    }

    private User userExists(String username) {
        for (User user : users) {
            if (user.getUsername() == username) 
                return user;
        }
        return null;
    }

    private void createNewUser(String username, String password) {
        User toAdd = new User(username, password, connectionCounter);
        users.add(toAdd);
        ConIdToUser.put(connectionCounter, toAdd);
    }

    public boolean isSubscribed(int ConId, String channel){
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        User user = ConIdToUser.get(ConId);
        return subscribedUsers.contains(user);
    }

    public int getMessageId() {
        return ++messageIdCounter;
    }

}
