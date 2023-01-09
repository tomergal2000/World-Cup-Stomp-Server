package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.impl.stomp.StompMessagingProtocolImpl;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private Integer PortId = null;
    private final MessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private Integer connectionId = null;
    private ConnectionsImpl<T> connections = null;
    
    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, MessagingProtocol<T> MessagingProtocol) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = MessagingProtocol;
    }

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoder<T> reader, StompMessagingProtocolImpl<T> MessagingProtocol, ConnectionsImpl<T> connections) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = MessagingProtocol;
        this.connections = connections;
        PortId = sock.getPort();
        connections.addHandler(this);
        ((StompMessagingProtocolImpl<T>) protocol).start(PortId, connections);
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { // just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage);//this also sends the response
                }
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        try{
        out.write(encdec.encode(msg));
        out.flush();
        }
        catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    public Integer getConnectionId() {
        return connectionId;
    }

    @Override
    public Integer getId() {
        return this.PortId;
    }
}
