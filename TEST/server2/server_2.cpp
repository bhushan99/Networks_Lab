#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <iostream>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <algorithm>
#include <ctime>

using namespace std;

#define BACKLOG 10
#define MAX 256
#define NODE0IP "127.0.0.1"
#define NODE0PORT "10001"
#define pb push_back
#define MOD 32

int main() {
	string ip,port;
	ip=string(NODE0IP);
	port=string(NODE1PORT);
	int socket_dgramid=socket(PF_INET,SOCK_DGRAM,0);
	struct sockaddr_in addrinfo;
	addrinfo.sin_family=AF_INET;
	addrinfo.sin_addr.s_addr=inet_addr(ip.c_str());
	addrinfo.sin_port=htons(atoi(port.c_str()));
	bind(socket_dgramid,(struct sockaddr *)&addrinfo,sizeof(addrinfo));
	
				int socket_streamid=socket(PF_INET,SOCK_STREAM,0);
				addrinfo.sin_family=AF_INET;
				addrinfo.sin_addr.s_addr=inet_addr(ip.c_str());
				addrinfo.sin_port=htons(atoi(port.c_str()));
				if(bind(socket_streamid,(struct sockaddr *)&addrinfo,sizeof(addrinfo))==-1)
					perror("STREAM bind");
				if(listen(socket_streamid,BACKLOG)==-1)
					perror("Listen");
			fd_set wfds;
           struct timeval tv;
           tv.tv_sec=1;
           tv.tv_usec=0;
           int retval=select(1, NULL, &wfds, NULL, &tv);

           if(retval==-1) perror("SELECT");
			else if(retval>0) {
				long long tc=time(NULL);
				for(int x=0;x<mxid;x++) {
					if(FD_ISSET(x,&wfds)) {
						
						FILE *fp=fopen(f1.c_str(),"r");
						fseek(fp,0,SEEK_END);
						int filesize=ftell(fp);
						rewind(fp);
						int size,nbytes=min(filesize,MAX-1);
						while((size=fread(bufst,sizeof(char),nbytes,fp))>0 && time(NULL)<=tc+4) {
							if(write(new_socket_streamid,bufst,size)==-1) perror("write");
							memset(bufst,0,MAX);
							filesize-=size;
							nbytes=min(filesize,MAX-1);
						}
						if(size>0) {

						}
						fclose(fp);



					}
				}

			}

			
