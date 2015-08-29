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

using namespace std;

#include "Node.hxx"

#define BACKLOG 10
#define MAX 256
#define NODE0IP "10.102.46.60"
#define NODE0PORT "10000"
#define pb push_back
#define MOD 32

std::vector< pair<int, pair< string , string > > > fileinfo;
Node curr,pre,succ;

bool inRange(int from,int to,int index) {
	if(from<to) {
		if(index>from&&index<=to) return true;
		return false;
	} 
	if(index>from || index<=to) return true;
	return false;
} 

void rem(int from,int to) {
	while(1) {
		int y;
		for(y=0;y<fileinfo.size();y++) {
			if(inRange(from,to,fileinfo[y].first)) break;
		}
		if(y==fileinfo.size()) break;
		fileinfo.erase(fileinfo.begin()+y);
	}
}

int main() {
	string ip,port;
	cout<<"Enter IP address: ";
	cin>>ip;
	cout<<"Enter port: ";
	cin>>port;
	int id=hash(ip+string(":")+port);
	curr.ip=ip;
	curr.port=port;
	curr.ID=id;
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
	bind(socket_streamid,(struct sockaddr *)&addrinfo,sizeof(addrinfo));
	listen(socket_streamid,BACKLOG);
	char bufdg[MAX],bufst[MAX];
	memset(bufst,0,MAX);
	memset(bufdg,0,MAX);
	string tos="NEW ";
	char c1[10] ;
	sprintf(c1,"%d ",id);
	tos+=string(c1);
	tos+=ip+string(" ")+port;
	struct sockaddr_in to,from;
	to.sin_family=AF_INET;
	to.sin_port=htons(atoi(NODE0PORT));
	to.sin_addr.s_addr=inet_addr(NODE0IP);
	if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
		perror("sendto");
	string fl1;
	cout<<"Enter files to share('NO' at finish):\n";
	cin>>fl1;
	while(fl1!="NO") {
		tos="SHARE ";
		int fic=hash(fl1);
		char v2[10];
		sprintf(v2,"%d ",fic);
		tos+=string(v2);
		tos+=ip+string(" ")+port;
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(NODE0PORT));
		to.sin_addr.s_addr=inet_addr(NODE0IP);
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");
		cin>>fl1;
	}
	cout<<"File sharing over.\n";

	while(1) {
		memset(bufdg,0,MAX);
		int len=sizeof(from);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
			perror("recvfrom");
		printf("received message : %s\n",bufdg );
		if(strncmp(bufdg,"KEEP",4)==0) {
			string fid="",fip="",fport="";
			int turn=1;
			for(int i=5;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: fid+=bufdg[i];break;
					case 2: fip+=bufdg[i];break;
					case 3: fport+=bufdg[i];break;
				}
			}
			int f1id=atoi(fid.c_str());
			fileinfo.pb(pair<int,pair<string,string> >(f1id,pair<string,string>(fip,fport)));
			sort(fileinfo.begin(), fileinfo.end());
		}
		else if(strncmp(bufdg,"SUCC",4)==0) {
			string nid="",nip="",nport="";
			int turn=1;
			for(int i=5;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			succ.ID=n1id;
			succ.ip=nip;
			succ.port=nport;
		}
		else if(strncmp(bufdg,"PRED",4)==0) {
			string nid="",nip="",nport="";
			int turn=1;
			for(int i=5;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			to.sin_addr.s_addr=inet_addr(nip.c_str());
			to.sin_port=htons(atoi(nport.c_str()));
			int last=pre.ID;
			pre.ID=n1id;
			pre.ip=nip;
			pre.port=nport;
			int x=0;
			for(;x<fileinfo.size();x++){
				if(inRange(last,n1id,fileinfo[x].first)) {
					string scb="KEEP ";
					char d[10];
					sprintf(d,"%d ",fileinfo[x].first);
					scb+=string(d);
					scb+=fileinfo[x].second.first+string(" ")+fileinfo[x].second.second; 
					if(sendto(socket_dgramid,scb.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
						perror("sendto");
				}
			}
			rem(last,n1id);
		}
		else if(strncmp(bufdg,"QUERY",5)==0) {
			string fid="",fip="",fport="";
			int turn=1;
			for(int i=6;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: fid+=bufdg[i];break;
					case 2: fip+=bufdg[i];break;
					case 3: fport+=bufdg[i];break;
				}
			}
			int f1id=atoi(fid.c_str());
			if(pre.ID==-1||inRange(pre.ID,curr.ID,f1id)) {
				int u=0;
				for(;u<fileinfo.size();u++) {
					if(f1id==fileinfo[u].first) break;
				}
				string tose="FOUND ";
				tose+=fid;
				tose+=string(" ")+fileinfo[u].second.first+string(" ")+fileinfo[u].second.second;
				struct sockaddr_in tk;
				tk.sin_family=AF_INET;
				tk.sin_addr.s_addr=inet_addr(fip.c_str());
				tk.sin_port=htons(atoi(fport.c_str()));
				if(sendto(socket_dgramid,tose.c_str(),MAX,0,(struct sockaddr *)&tk,sizeof(tk))==-1)
					perror("sendto");
			} else {
				struct sockaddr_in tk;
				tk.sin_family=AF_INET;
				tk.sin_addr.s_addr=inet_addr(succ.ip.c_str());
				tk.sin_port=htons(atoi(succ.port.c_str()));
				if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&tk,sizeof(tk))==-1)
					perror("sendto");
			}
		}
		else if(strncmp(bufdg,"GET",3)==0) {
			string file=string(bufdg+4);
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
			cout<<"file "<<file<<" sent.\n";
			shutdown(new_socket_streamid,2);
		}
		else if(strncmp(bufdg,"SHARE",5)==0) {
			string fid="",fip="",fport="";
			int turn=1;
			for(int i=6;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: fid+=bufdg[i];break;
					case 2: fip+=bufdg[i];break;
					case 3: fport+=bufdg[i];break;
				}
			}
			int f1id=atoi(fid.c_str());
			if(pre.ID==-1||inRange(pre.ID,curr.ID,f1id)) {
				fileinfo.pb(pair<int,pair<string,string> >(f1id,pair<string,string>(fip,fport)));
				sort(fileinfo.begin(), fileinfo.end());
			} else {
				struct sockaddr_in tk;
				tk.sin_family=AF_INET;
				tk.sin_addr.s_addr=inet_addr(succ.ip.c_str());
				tk.sin_port=htons(atoi(succ.port.c_str()));
				if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&tk,sizeof(tk))==-1)
					perror("sendto");
			}
		}
	}
	close(socket_dgramid);
	close(socket_streamid);
	return 0;
}