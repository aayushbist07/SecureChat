#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

/*
    initialize winsock

    create socket 

    connect to the server

    send/recv

    close the socket
*/
bool Initialize(){
    WSADATA data;
    return WSAStartup(MAKEWORD(2,2),&data) == 0;    
}

void sendMessage(SOCKET s){
    cout<<"enter your name"<<endl;
    string name;
    getline(cin,name);
    string message;
    
    while (1){
        getline(cin,message);
        string msg = name +" : " + message;
        int bytesent = send(s,msg.c_str(),msg.length(),0);
        if (bytesent == SOCKET_ERROR)
        {
            cout<<"error sending message"<<endl;
            break;
        }
        if(message == "quit")
        {
            cout<<"exiting...."<<endl;
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void RecieveMessage(SOCKET s){
    char buffer[4096];
    int recvlen;
    string msg = "";
    while (1){
        int recvlen = recv(s,buffer,sizeof(buffer),0);
        if(recvlen <= 0){
            cout<<"disconnected from the server"<<endl;
            break;
        }
        else{
            msg = string(buffer, recvlen);
            cout<<msg<<endl;
        }
    }
    closesocket(s);
    WSACleanup();
}

int main()
{
    int port = 12345;
    if(!Initialize()){
        cout<<"socket creation failed"<<endl;
    }
    cout<<"client program started"<<endl;

    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM,0);
    if (s == INVALID_SOCKET){
        cout<<"invalid socket "<<endl;
        return 1;
    }

    string serveraddress = "127.0.0.1";
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddress.c_str() , &(serveraddr.sin_addr) );

    if(connect(s , reinterpret_cast<sockaddr*>(&serveraddr) , sizeof(serveraddr) ) == SOCKET_ERROR){
        cout<<"connection to the server unsuccessful"<<endl;
        cout<<" : "<< WSAGetLastError();
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout<<"connection successful"<<endl;

    //creating threads
    thread senderthread(sendMessage, s);
    thread receiverthread(RecieveMessage, s);

    senderthread.join();
    receiverthread.join();

    return 0;
}
