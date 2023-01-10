package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;//TODO: maybe blocking list bc user can try to connect concurrently from 2 computers (clients)
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentHashMap<Integer, ConnectionHandler<T>> ConIdToHandler;
    private ConcurrentHashMap<String, ArrayList<User>> ChanNameToUserList;
    public  ConcurrentHashMap<Integer, User> ConIdToUser;
    private ArrayList<User> users;
    private AtomicInteger messageIdCounter;

    public ConnectionsImpl() {
        ConIdToHandler = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
        ChanNameToUserList = new ConcurrentHashMap<String, ArrayList<User>>();
        ConIdToUser = new ConcurrentHashMap<Integer, User>();
        users = new ArrayList<User>();
        messageIdCounter = new AtomicInteger(0);
    }

    public boolean send(int connectionId, T msg) {

        ConnectionHandler<T> handler = ConIdToHandler.get(connectionId);

        if (handler == null)
            return false;

        handler.send(msg);

        return true;
    }

    public void send(String channel, T msg) {

        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        
        for (User user : subscribedUsers) {
            msg = (T)insertSubId(msg, user.getSubId(channel).toString());
            ConnectionHandler<T> handler = ConIdToHandler.get(user.getConId());
            handler.send(msg);
        }
    }

    private String insertSubId(T msg, String subscrId) {
        String toSend = (String) msg;
        String edited = "";
        edited += toSend.substring(0, 21) + subscrId + '\n';
        edited += toSend.substring(21);
        
        //for debugging:
        System.out.println(edited);
        
        return edited;
    }

    public void disconnect(int connectionId) {
        ConIdToUser.get(connectionId).unsubscribeAll();
        ConIdToHandler.remove(connectionId);
        ConIdToUser.remove(connectionId);
    }

    public void addHandler(Integer conId, ConnectionHandler handler) {
        ConIdToHandler.put(conId , handler);
    }

    // assumes that client is not yet logged in
    public int connect(int connectionId, String username, String password) {
        // 0 - success
        // 1 - wrong password
        // 2 - user already logged in
        User toCheck = userExists(username);

        if (toCheck == null) {// means this user is new
            createNewUser(username, password, connectionId);
            return 0;
        } else if (toCheck.getPassword() != password)
            return 1;

        else if (ConIdToUser.contains(toCheck))
            return 2;

        ConIdToUser.put(connectionId, toCheck);
        toCheck.connect(connectionId);
        return 0;
    }

    public boolean subscribe(Integer subId, String channel, Integer conId) {
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        User user = ConIdToUser.get(conId);
        if (subscribedUsers == null) {
            ArrayList<User> chanToAdd = new ArrayList<User>();
            chanToAdd.add(user);
            ChanNameToUserList.put(channel, chanToAdd);
            return user.subscribe(subId, channel);
        } 
        else if(!subscribedUsers.contains(user)){
            subscribedUsers.add(user);
            return user.subscribe(subId, channel);
        }
        else{//subscribedUserd already contains user
            return user.subscribe(subId, channel);
        }
    }

    public boolean unsubscribe(Integer subId, String channel, Integer conId) {
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        if (subscribedUsers != null) {
            User user = ConIdToUser.get(conId);
            boolean wasSubscribed = user.unsubscribe(subId);
            if (wasSubscribed){
                subscribedUsers.remove(user);
                return true;
            }
            return false;
        }
        return false;
    }

    private User userExists(String username) {
        for (User user : users) {
            if (user.getUsername() == username) 
                return user;
        }
        return null;
    }

    private void createNewUser(String username, String password, int connectionId) {
        User toAdd = new User(username, password, connectionId);
        users.add(toAdd);
        ConIdToUser.put(connectionId, toAdd);
    }

    public boolean isSubscribed(int ConId, String channel){
        ArrayList<User> subscribedUsers = ChanNameToUserList.get(channel);
        User user = ConIdToUser.get(ConId);
        return subscribedUsers.contains(user);
    }

    public int getMessageId() {
        return messageIdCounter.getAndIncrement();
    }

    public User getUser(Integer conId){
        try{
            return ConIdToUser.get(conId);
        }catch (NullPointerException e){
            return null;
        }
    }
}
