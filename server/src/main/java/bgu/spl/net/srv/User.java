package bgu.spl.net.srv;

// import java.util.ArrayList;
import java.util.HashMap;


public class User {

    private String username;
    private String password;
    private HashMap<Integer, String> subIdToChan;
    private Integer connectionId;

    public User(String username, String password, Integer connectionId) {
        this.username = username;
        this.password = password;
        this.subIdToChan = new HashMap<Integer, String>();
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

    public boolean subscribe(Integer subId, String subName) {
        boolean keyISUsed = subIdToChan.containsKey(subId);
        boolean nameIsUsed = subIdToChan.containsValue(subName);
        
        if(!keyISUsed && !nameIsUsed){
            subIdToChan.put(subId, subName);
            return true;
        }
        else if(keyISUsed && nameIsUsed && subIdToChan.get(subId).equals(subName))
            return true;
        
        else return false;

    }

    public boolean unsubscribe(Integer subId) {
        return subIdToChan.remove(subId) != null;
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
    
    public Integer getSubId(String channelName){
        for(Integer key : subIdToChan.keySet()){
            if(subIdToChan.get(key).equals(channelName))
                return key;
        }
        return -1;
    }
    public String getSubName(Integer subId){
        return subIdToChan.get(subId);
    }
    public void unsubscribeAll() {
        subIdToChan.clear();
    }

    public void printSubs() {
    }

}
