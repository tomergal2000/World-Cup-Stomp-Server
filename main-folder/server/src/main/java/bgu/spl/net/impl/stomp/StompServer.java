package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
       
        //you can use any server... 
        // Server.threadPerClient(
        //         true,
        //         7777, //port
        //         () -> new StompMessagingProtocolImpl<String>(), //protocol factory
        //         StompMessageEncDec::new //message encoder decoder factory
        // ).serve();

        Server.reactor(
                true,
                Runtime.getRuntime().availableProcessors(),
                7777, //port
                () -> new StompMessagingProtocolImpl<String>(), //protocol factory
                StompMessageEncDec::new //message encoder decoder factory
        ).serve();
    }
}
