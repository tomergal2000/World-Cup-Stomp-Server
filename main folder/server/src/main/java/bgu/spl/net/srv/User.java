package bgu.spl.net.srv;

// import java.util.ArrayList;
import java.util.ArrayList;


public class User {

    private String username;
    private String password;
    private ArrayList<String> channelList;
    // private int subIdCounter;
    private Integer connectionId;

    public User(String username, String password, Integer connectionId) {
        this.username = username;
        this.password = password;
        this.channelList = new ArrayList<String>();
        // this.subIdCounter = 0;
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

    //TODO: handle ERROR?
    public void subscribe(Integer subId, String subName) {
        if (!channelList.contains(subName))
            channelList.add(subId, subName);
    }

    public void unsubscribe(String channel) {
        channelList.remove(channel);
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
    
    public ArrayList<String> getChannelList(){
        return channelList;
    }
    public Integer getSubId(String channelName){
        return channelList.indexOf(channelName);
    }
    public void unsubscribeAll() {
        channelList.clear();
    }

}
