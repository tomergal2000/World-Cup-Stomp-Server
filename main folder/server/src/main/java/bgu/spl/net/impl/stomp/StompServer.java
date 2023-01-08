package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.ConnectionsImpl;
import bgu.spl.net.impl.echo.*;;

public class StompServer {

    public static void main(String[] args) {
       
        // you can use any server... 
        ConnectionsImpl<String> connections= new ConnectionsImpl<String>();

        Server.threadPerClient(
                7777, //port
                () -> new EchoProtocol(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();

        // Server.reactor(
        //         Runtime.getRuntime().availableProcessors(),
        //         7777, //port
        //         () -> new EchoProtocol<>(), //protocol factory
        //         LineMessageEncoderDecoder::new //message encoder decoder factory
        // ).serve();
    }
}
