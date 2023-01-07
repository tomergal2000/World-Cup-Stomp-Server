package bgu.spl.net.srv;

// import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;


public class User {

    private String username;
    private String password;
    private ConcurrentHashMap<Integer, String> subIdToChanName;
    private int subIdCounter;
    private Integer connectionId;

    public User(String username, String password, Integer connectionId) {
        this.username = username;
        this.password = password;
        this.subIdToChanName = new ConcurrentHashMap<Integer, String>();
        this.subIdCounter = 0;
        this.connectionId = connectionId;
    }

    public void connect(Integer connectionId) {
        this.connectionId = connectionId;
    }

    public void dissconnect() {
        this.connectionId = null;
    }

    public boolean isConnected() {
        return connectionId != null;
    }

    //TODO: handle ERROR
    public void subscribe(Integer subId, String subName) {
        if (!subIdToChanName.contains(subId))
            subIdToChanName.put(subId, subName);
    }

    public void unsubscribe(Integer subId) {
        subIdToChanName.remove(subId);
    }
    public String getUsername(){
        return username;
    }
    public String getPassword(){
        return password;
    }
    public Integer getConId(){
        return connectionId;
    }
    public ConcurrentHashMap<Integer, String> getSubIdToChanName(){
        return subIdToChanName;
    }

    public void unsubscribeAll() {
        subIdToChanName.clear();
    }

}
