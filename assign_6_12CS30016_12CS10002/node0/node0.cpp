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
std::vector<pair<int,pair<string,string> > > nodes;
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
	ip=string(NODE0IP);
	port=string(NODE0PORT);
	int id=hash(ip+string(":")+port);
	curr.ip=ip;
	curr.port=port;
	curr.ID=id;
	nodes.pb(pair<int,pair<string,string> >(id,pair<string,string>(ip,port)));
	cout<<"Enter files to share('NO' for finish):\n";
	string file;
	cin>>file;
	pre.ID=-1;
	succ.ID=-1;
	while(file!="NO") {
		int fid=hash(file);
		fileinfo.pb(pair<int,pair<string,string> >(fid,pair<string,string>(ip,port)));
		cin>>file;
	}
	cout<<"File sharing over.\n";
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
	
	char bufdg[MAX],bufst[MAX];
	memset(bufst,0,MAX);
	memset(bufdg,0,MAX);
	
	while(1) {
		struct sockaddr_in from;
		int len=sizeof(from);
		memset(bufdg,0,MAX);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
			perror("recvfrom");
		printf("received message : %s\n",bufdg );
		if(strncmp(bufdg,"NEW",3)==0) {
			string nid="",nip="",nport="";
			int turn=1;
			for(int i=4;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			if(nodes.size()==1) {
				int preid=pre.ID;
				if(preid==-1) preid=id;
				pre.ID= succ.ID=n1id;
				pre.ip= succ.ip=nip;
				pre.port= succ.port=nport;
				string tosend="SUCC ";
				char c[10];
				sprintf(c,"%d ",id);
				tosend+=string(c);
				tosend+=ip+" "+port;
				if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
					perror("sendto");
				tosend[0]='P';tosend[1]='R';tosend[2]='E';tosend[3]='D';
				if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
					perror("sendto");		
			
					for(int j=0;j<fileinfo.size();j++) {
						if(inRange(preid,n1id,fileinfo[j].first)) {
							string tr="KEEP ";
							char z[10];
							sprintf(z,"%d ",fileinfo[j].first);
							tr+=string(z);
							tr+=fileinfo[j].second.first;
							tr+=string(" ");
							tr+=fileinfo[j].second.second;
							if(sendto(socket_dgramid,tr.c_str(),MAX,0,
								(struct sockaddr *)&from,sizeof(from))==-1)
								perror("sendto");
						}
					}
					rem(preid,n1id);	

			}
			else {
				int x=0;
				while(x<nodes.size()&&nodes[x].first<n1id) x++;
				if(x<nodes.size()) {

					string tosend="SUCC ";
					char c[10];
					sprintf(c,"%d ",nodes[x].first);
					tosend+=string(c);
					tosend+=nodes[x].second.first+" "+nodes[x].second.second;
					if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
						perror("sendto");
					if(nodes[x].first==id) {
						int preid=pre.ID;
						if(preid==-1) preid=id;
						pre.ID=n1id;
						pre.ip=nip;
						pre.port=nport;

						for(int j=0;j<fileinfo.size();j++) {
							if(inRange(preid,n1id,fileinfo[j].first)) {
								string tr="KEEP ";
								char z[10];
								sprintf(z,"%d ",fileinfo[j].first);
								tr+=string(z);
								tr+=fileinfo[j].second.first;
								tr+=string(" ");
								tr+=fileinfo[j].second.second;
								if(sendto(socket_dgramid,tr.c_str(),MAX,0,
									(struct sockaddr *)&from,sizeof(from))==-1)
									perror("sendto");
							}
						}
						rem(preid,n1id);

					} else{
						struct sockaddr_in sfg;
						sfg.sin_port=htons(atoi(nodes[x].second.second.c_str()));
						sfg.sin_addr.s_addr=inet_addr(nodes[x].second.first.c_str());
						sfg.sin_family=AF_INET;
						string sa="PRED ";
						sa+=nid;
						sa+=string(" ")+nip+string(" ")+nport;
						if(sendto(socket_dgramid,sa.c_str(),MAX,0,(struct sockaddr *)&sfg,sizeof(sfg))==-1)
							perror("sendto");
					}
					tosend="PRED ";
					if(x){
						char d[10];
						sprintf(d,"%d ",nodes[x-1].first);
						tosend+=string(d);
						tosend+=nodes[x-1].second.first+" "+nodes[x-1].second.second;
						if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
							perror("sendto");
						if(nodes[x-1].first==id) {
							succ.ID=n1id;
							succ.ip=nip;
							succ.port=nport;
						} else {
							struct sockaddr_in sfg;
							sfg.sin_port=htons(atoi(nodes[x-1].second.second.c_str()));
							sfg.sin_addr.s_addr=inet_addr(nodes[x-1].second.first.c_str());
							sfg.sin_family=AF_INET;
							string sa="SUCC ";
							sa+=nid;
							sa+=string(" ")+nip+string(" ")+nport;
							if(sendto(socket_dgramid,sa.c_str(),MAX,0,(struct sockaddr *)&sfg,sizeof(sfg))==-1)
								perror("sendto");
						}
					}else{
						int mx=nodes.size()-1;
						char d[10];
						sprintf(d,"%d ",nodes[mx].first);
						tosend+=string(d);
						tosend+=nodes[mx].second.first+" "+nodes[mx].second.second;
						if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
							perror("sendto");
						if(nodes[mx].first==id) {
							succ.ID=n1id;
							succ.ip=nip;
							succ.port=nport;
						} else {
							struct sockaddr_in sfg;
							sfg.sin_port=htons(atoi(nodes[mx].second.second.c_str()));
							sfg.sin_addr.s_addr=inet_addr(nodes[mx].second.first.c_str());
							sfg.sin_family=AF_INET;
							string sa="SUCC ";
							sa+=nid;
							sa+=string(" ")+nip+string(" ")+nport;
							if(sendto(socket_dgramid,sa.c_str(),MAX,0,(struct sockaddr *)&sfg,sizeof(sfg))==-1)
								perror("sendto");
						}
					}
				} else {
					string tosend="SUCC ";
					char c[10];
					sprintf(c,"%d ",nodes[0].first);
					tosend+=string(c);
					tosend+=nodes[0].second.first+" "+nodes[0].second.second;
					if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
						perror("sendto");
					if(nodes[0].first==id) {
						int preid=pre.ID;
						if(preid==-1) preid=id;
						pre.ID=n1id;
						pre.ip=nip;
						pre.port=nport;

						for(int j=0;j<fileinfo.size();j++) {
							if(inRange(preid,n1id,fileinfo[j].first)) {
								string tr="KEEP ";
								char z[10];
								sprintf(z,"%d ",fileinfo[j].first);
								tr+=string(z);
								tr+=fileinfo[j].second.first;
								tr+=string(" ");
								tr+=fileinfo[j].second.second;
								if(sendto(socket_dgramid,tr.c_str(),MAX,0,
									(struct sockaddr *)&from,sizeof(from))==-1)
									perror("sendto");
							}
						}
						rem(preid,n1id);

					} else{
						struct sockaddr_in sfg;
						sfg.sin_port=htons(atoi(nodes[0].second.second.c_str()));
						sfg.sin_addr.s_addr=inet_addr(nodes[0].second.first.c_str());
						sfg.sin_family=AF_INET;
						string sa="PRED ";
						sa+=nid;
						sa+=string(" ")+nip+string(" ")+nport;
						if(sendto(socket_dgramid,sa.c_str(),MAX,0,(struct sockaddr *)&sfg,sizeof(sfg))==-1)
							perror("sendto");
					}
					tosend="PRED ";
					char d[10];
					sprintf(d,"%d ",nodes[x-1].first);
					tosend+=string(d);
					tosend+=nodes[x-1].second.first+" "+nodes[x-1].second.second;
					if(sendto(socket_dgramid,tosend.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
						perror("sendto");
					if(nodes[x-1].first==id) {
						int preid=pre.ID;
						if(preid==-1) preid=id;
						pre.ID=n1id;
						pre.ip=nip;
						pre.port=nport;

						for(int j=0;j<fileinfo.size();j++) {
							if(inRange(preid,n1id,fileinfo[j].first)) {
								string tr="KEEP ";
								char z[10];
								sprintf(z,"%d ",fileinfo[j].first);
								tr+=string(z);
								tr+=fileinfo[j].second.first;
								tr+=string(" ");
								tr+=fileinfo[j].second.second;
								if(sendto(socket_dgramid,tr.c_str(),MAX,0,
									(struct sockaddr *)&from,sizeof(from))==-1)
									perror("sendto");
							}
						}
						rem(preid,n1id);

					} else{
						struct sockaddr_in sfg;
						sfg.sin_port=htons(atoi(nodes[x-1].second.second.c_str()));
						sfg.sin_addr.s_addr=inet_addr(nodes[x-1].second.first.c_str());
						sfg.sin_family=AF_INET;
						string sa="SUCC ";
						sa+=nid;
						sa+=string(" ")+nip+string(" ")+nport;
						if(sendto(socket_dgramid,sa.c_str(),MAX,0,(struct sockaddr *)&sfg,sizeof(sfg))==-1)
							perror("sendto");
					}
				}
			}
			nodes.pb(pair<int,pair<string,string> >(n1id,pair<string,string>(nip,nport)));
			sort(nodes.begin(),nodes.end());

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
				if(sendto(socket_dgramid,tose.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
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
	close(socket_streamid);
	close(socket_dgramid);
	return 0;

}