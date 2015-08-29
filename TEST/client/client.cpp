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
#include <ctime>
#include <algorithm>

using namespace std;

#define BACKLOG 10
#define MAX 256
#define NODE0IP "127.0.0.1"
#define NODE0PORT "10000"
#define pb push_back
#define MOD 32

int main() {
	string ip,port;
	cout<<"Enter IP address: ";
	cin>>ip;
	cout<<"Enter port: ";
	cin>>port;
	int socket_dgramid=socket(PF_INET,SOCK_DGRAM,0);
	struct sockaddr_in addrinfo;
	addrinfo.sin_family=AF_INET;
	addrinfo.sin_addr.s_addr=inet_addr(ip.c_str());
	addrinfo.sin_port=htons(atoi(port.c_str()));
	bind(socket_dgramid,(struct sockaddr *)&addrinfo,sizeof(addrinfo));
	
	char bufdg[MAX],bufst[MAX];
	memset(bufst,0,MAX);
	memset(bufdg,0,MAX);
	cout<<"Enter file to download('NO' at finish):\n";
	string file;
	cin>>file;
	while(file!="NO") {
		string tos="GET ";
		tos+=file;
		struct sockaddr_in gh,from,scv;
		gh.sin_family=AF_INET;
		gh.sin_port=htons(atoi(NODE0PORT));
		gh.sin_addr.s_addr=inet_addr(NODE0IP);
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&gh,sizeof(gh))==-1)
			perror("sendto");
		int len=sizeof(from);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
			perror("recvfrom");
		if(strncmp(bufdg,"FOUND",5)==0) {
				string clid="",fszl="";
				int turn=1;
				for(int i=6;i<strlen(bufdg);i++) {
					if(bufdg[i]==' ') {turn++;continue;}
					switch(turn){
						case 1: clid+=bufdg[i];break;
						case 2: fszl+=bufdg[i];break;
					}
				}
				cout<<"client ID: "<<clid<<endl;
				cout<<"File name: "<<file<<endl;
				cout<<"File size: "<<fszl<<endl;
				int fxc=atoi(fszl.c_str());
				int socket_streamid=socket(PF_INET,SOCK_STREAM,0);
				scv.sin_family=AF_INET;
				scv.sin_port=htons(atoi(NODE0PORT));
				scv.sin_addr.s_addr=inet_addr(NODE0IP);
				if(connect(socket_streamid,(struct sockaddr *)&scv,sizeof(scv))==-1)
				{
					shutdown(socket_dgramid,2);
					shutdown(socket_streamid,2);
					perror("connect");
					exit(1);
				}
				FILE* fp=fopen(file.c_str(),"w");
				int size;
				int tsz=0;
				int per=0,prev=0;
				while((size=read(socket_streamid,bufst,MAX))>0) {
					tsz+=size;
					per=(double)tsz/fxc*10;
					if(per>prev) cout<<per<<"0% ";
					prev=per;
					fwrite(bufst,sizeof(char),size,fp);
					memset(bufst,0,MAX);
				}
				cout<<endl;
				fclose(fp);
				string md=string("md5sum ")+file;
				system(md.c_str());
				close(socket_streamid);
			
		}
		cin>>file;
	}
	close(socket_dgramid);
	return 0;
}