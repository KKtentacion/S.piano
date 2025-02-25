package org.SPiano;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter port: ");
        int port = scanner.nextInt();

        SignalServer signalServer = new SignalServer(8080);
        signalServer.start();
    }
}