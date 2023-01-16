package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        
        if(args[1].equals("tpc")){
            Server.threadPerClient(
                true,
                Integer.parseInt(args[0]), //port
                () -> new StompMessagingProtocolImpl<String>(), //protocol factory
                StompMessageEncDec::new //message encoder decoder factory
            ).serve();
        }

        else if(args[1].equals("reactor")){
            Server.reactor(
                true,
                Runtime.getRuntime().availableProcessors(),
                Integer.parseInt(args[0]), //port
                () -> new StompMessagingProtocolImpl<String>(), //protocol factory
                StompMessageEncDec::new //message encoder decoder factory
            ).serve();
        }
    }
}
