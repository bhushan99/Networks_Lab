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
#define NODE0PORT "10000"
#define pb push_back
#define MOD 32

int main() {
	string ip,port;
	ip=string(NODE0IP);
	port=string(NODE0PORT);
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
	char bufdg[MAX],bufst[MAX];
	memset(bufst,0,MAX);
	memset(bufdg,0,MAX);
	int clinum=1;
	while(1) {
		struct sockaddr_in from;
		int len=sizeof(from);
		memset(bufdg,0,MAX);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
			perror("recvfrom");
		if(strncmp(bufdg,"GET",3)==0) {
			cout<<"client ID: "<<clinum<<endl;
			string file=string(bufdg+4);
			cout<<"File name: "<<file<<endl;
			FILE *fp1=fopen(file.c_str(),"r");
			fseek(fp1,0,SEEK_END);
			int fsx=ftell(fp1);
			cout<<"File size: "<<fsx<<endl;
			fclose(fp1);
			string md=string("md5sum ")+file;
			system(md.c_str());
			
			if(fork()==0) {
				cout<<"Start time: "<<time(NULL)<<endl;
				
				if(listen(socket_streamid,BACKLOG)==-1)
					perror("Listen");
				string tose="FOUND ";
				char tz[10];
				sprintf(tz,"%d ",clinum);
				tose+=string(tz);
				char ty[10];
				sprintf(ty,"%d",fsx);
				tose+=string(ty);
				if(sendto(socket_dgramid,tose.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
					perror("sendto");
				
				int new_socket_streamid;
				if((new_socket_streamid=accept(socket_streamid,(struct sockaddr*)&from,(socklen_t*)&len))==-1) {
					perror("accept");
					exit(1);
				}
				FILE *fp=fopen(file.c_str(),"r");
				fseek(fp,0,SEEK_END);
				int filesize=ftell(fp);
				rewind(fp);
				int size,nbytes=min(filesize,MAX-1);
				while((size=fread(bufst,sizeof(char),nbytes,fp))>0 ) {
					if(write(new_socket_streamid,bufst,size)==-1) perror("write");
					memset(bufst,0,MAX);
					filesize-=size;
					nbytes=min(filesize,MAX-1);
				}
				fclose(fp);
				close(new_socket_streamid);
				close(socket_streamid);
				cout<<"End time: "<<time(NULL)<<endl;
				exit(0);
			}
			clinum++;
		}	
	}
	return 0;
}