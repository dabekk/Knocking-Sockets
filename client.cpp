#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<iostream>    //cout
#include<fstream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
//opy
#include <vector>
#include "option_handler.h"
 
using namespace std;

/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
     
public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive(int);
};
 
tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}
 
/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        //cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
           // cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    //cout<<"Connected\n";
    return true;
}
 
/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    //cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
string tcp_client::receive(int size=512)
{
    char buffer[size];
    string reply;
     
    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
     
    reply = buffer;
    return reply;
}
 
void add_options(OptionHandler::Handler & h) {

  try {
    h.add_option('h', "host", OptionHandler::REQUIRED, true);
    h.add_option('?', "help",   OptionHandler::NONE, false); 
    h.add_option('p', "port",   OptionHandler::REQUIRED, true); 
    h.add_option('w', "web",   OptionHandler::OPTIONAL, false); 
    h.add_option('f', "file",   OptionHandler::OPTIONAL, false);
    h.add_option('H', "help",   OptionHandler::NONE, false); 
}
  catch (const std::exception & e) {
    std::cerr << e.what() << std::endl; }
}

int main(int argc , char *argv[])
{
	OptionHandler::Handler h = OptionHandler::Handler(argc, argv);
	add_options(h);
	std::string fileName = h.get_argument("file");
	std::string web = h.get_argument("web");
	bool hostFlag = h.get_option("host");
	bool fileFlag = h.get_option("file");
	bool webFlag = h.get_option("web");
	bool helpFlag = h.get_option("help");
	bool portFlag = h.get_option("port");
    tcp_client c;
	ofstream myfile;
	myfile.open(fileName);

	if(hostFlag && portFlag && fileFlag)
	{	
		std::string webString = h.get_argument("web");
		std::string hostString = h.get_argument("host");
		std::string portString = h.get_argument("port");
		int port = atoi(portString.c_str());
		c.conn(hostString , port);	//establish connection
		string webx =  "GET /" +  webString + "\r\n\r\n";	//concatinate host and web
	//	cout << webx;
		c.send_data(webx);	//GET request
		string reply;
		reply = c.receive(1024);	//store what was received
	//	cout << reply;
		myfile << reply;	//input into txt file
		myfile.close();
	}	//error handling
	else if(!hostFlag || !portFlag) {
		cerr << "You must input both the host flag and port flag\n";
	}
	else if(!fileFlag || !webFlag) {
		cerr << "You must input both the file flag and web flag\n";
	}
	else if(helpFlag) {
		cout << "usage: ./a.out -h host -p port [-H] [-w] [-f file]\n";
	}

    //done
    return 0;
}
