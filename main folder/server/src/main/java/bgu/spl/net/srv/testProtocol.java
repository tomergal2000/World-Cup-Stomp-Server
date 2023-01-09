package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;
import java.util.ArrayList;

public class testProtocol {

    public static void main(String[] args) {
        String testSUB = "SUBSCRIBE\ndestination:/germany_spain\nid:17\nreceipt:73\n\n\u0000";
        String testCON = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:films\n\n\u0000";
        String testUNS = "UNSUBSCRIBE\nid:17\nreceipt:82\n\n\u0000";
        String testDIS = "DISCONNECT\nreceipt:113\n\n\u0000";
        testSUB(testSUB);
        testCON(testCON);
        testUNS(testUNS);
        testDIS(testDIS);
        //TODO: testSEND
    }

    public static ArrayList<String> stringToList(String inFrame){
        ArrayList<String> words = new ArrayList<String>();
        String word = "";
        boolean isValue = false;
        int firstLineEnding = inFrame.indexOf('\n');
        words.add(inFrame.substring(0, firstLineEnding));
        for (int c = firstLineEnding; c < inFrame.length(); c++) {// haha c++ but it's java
            if (inFrame.charAt(c) == ':')
                isValue = true;
            else if (isValue) {
                if (inFrame.charAt(c) != '\n' && inFrame.charAt(c) != '\u0000')
                    word += inFrame.charAt(c);
                else {
                    words.add(word);
                    word = "";
                    isValue = false;
                }
            }
        }
        return words;
    }

    public static void testSUB(String inFrame){
        ArrayList<String> words = stringToList(inFrame);
        for(String word : words){
            System.out.println(word);
        }
        String receipt1 = "RECEIPT" + '\n' + "receipt-id:" + words.get(3) + '\n' + '\n' + '\u0000';
        System.out.println(receipt1);
    }

    public static void testCON(String inFrame){
        ArrayList<String> words = stringToList(inFrame);
        for(String word : words){
            System.out.println(word);
        }
        String connected = "CONNECTED" + '\n' + "version:1.2" + '\n' + '\n' + '\u0000';
        System.out.println(connected);
    }

    public static void testUNS(String inFrame){
        ArrayList<String> words = stringToList(inFrame);
        for(String word : words){
            System.out.println(word);
        }
        String receipt2 = "RECEIPT" + '\n' + "receipt-id:" + words.get(2) + '\n' + '\n' + '\u0000';
        System.out.println(receipt2);
    }

    public static void testDIS(String inFrame){
        ArrayList<String> words = stringToList(inFrame);
        for(String word : words){
            System.out.println(word);
        }
        String receipt2 = "RECEIPT" + '\n' + "receipt-id:-1" + '\n' + '\n' + '\u0000';
        System.out.println(receipt2);
    }

}
