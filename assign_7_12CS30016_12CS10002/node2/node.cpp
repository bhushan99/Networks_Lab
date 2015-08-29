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
#define NODE0IP "127.0.0.1"
#define NODE0PORT "9004"
#define pb push_back
#define MOD 8
#define M 3
#define rep(i, n) for(int i = 0; i < (n); ++i)
#define iph "127.0.0.1"
#define porth "9009"

std::vector< pair<int, pair< string , string > > > fileinfo;
Node FingerTable[M];
Node curr,pre;

bool inRange(int from,int to,int index) {
	if(from<=to) {
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

void rem_entries(string ip,string port) {
	while(1) {
		int y;
		for(y=0;y<fileinfo.size();y++) {
			if(fileinfo[y].second.first==ip&&fileinfo[y].second.second==port) break;
		}
		if(y==fileinfo.size()) break;
		fileinfo.erase(fileinfo.begin()+y);
	}
}

void printFingerTable(int id) {
	rep(i,M) {
		cout<<(id+(1<<i))%MOD<<'\t';
		cout<<FingerTable[i].ID<<'\t'<<FingerTable[i].ip<<'\t'<<FingerTable[i].port<<endl;
	}
	cout<<endl;
}

void client(string ip,string parport) {

	string port;
	int socket_dgramid=socket(PF_INET,SOCK_DGRAM,0);
	struct sockaddr_in addrinfo,temp;
	addrinfo.sin_family=AF_INET;
	addrinfo.sin_addr.s_addr=inet_addr(ip.c_str());
	addrinfo.sin_port=0;
	bind(socket_dgramid,(struct sockaddr *)&addrinfo,sizeof(addrinfo));
	int szx=sizeof(temp);
	getsockname(socket_dgramid,(sockaddr*)&temp,(socklen_t*)&szx);

	char l[10];
	sprintf(l,"%d",ntohs(temp.sin_port));
	port=string(l);

	char bufdg[MAX],bufst[MAX];
	memset(bufst,0,MAX);
	memset(bufdg,0,MAX);

	cout<<"\nCommand Format:\n";
	cout<<"download <filename>\n";
	cout<<"share <filename>\n";
	cout<<"exit (to delete this node from file sharing system)\n\n";

	string line;
	getline(cin,line);
	while(1) {
		if(strncmp(line.c_str(),"download ",9)==0) {

			string file=string(line.c_str()+9);
			int fid=hash(file);
			string tos="QSUCC ";
			char t[10];
			sprintf(t,"%d ",fid);
			tos+=string(t);
			tos+=ip+string(" ")+port;
			
			struct sockaddr_in gh,from;
			gh.sin_family=AF_INET;
			gh.sin_port=htons(atoi(NODE0PORT));
			gh.sin_addr.s_addr=inet_addr(NODE0IP);
			if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&gh,sizeof(gh))==-1)
				perror("sendto");
			
			int len=sizeof(from);
			if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
				perror("recvfrom");
			printf("received message : %s\n",bufdg );
			
			if(strncmp(bufdg,"SUCC",4)==0) {
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
				tos[0]='Q';tos[1]='U';tos[2]='E';tos[3]='R';tos[4]='Y';
				gh.sin_family=AF_INET;
				gh.sin_port=htons(atoi(nport.c_str()));
				gh.sin_addr.s_addr=inet_addr(nip.c_str());
				if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&gh,sizeof(gh))==-1)
					perror("sendto");
				len=sizeof(from);
				memset(bufdg,0,MAX);
				if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
					perror("recvfrom");
				printf("received message : %s\n",bufdg );
			}
			
			if(strncmp(bufdg,"FOUND",5)==0) {
				string flid="",oip="",oport="";
				int turn=1;
				for(int i=6;i<strlen(bufdg);i++) {
					if(bufdg[i]==' ') {turn++;continue;}
					switch(turn){
						case 1: flid+=bufdg[i];break;
						case 2: oip+=bufdg[i];break;
						case 3: oport+=bufdg[i];break;
					}
				}
				int f1id=atoi(flid.c_str());
				if(f1id==fid) {
					string tr="GET ";
					tr+=file;
					struct sockaddr_in scv;
					scv.sin_family=AF_INET;
					scv.sin_addr.s_addr=inet_addr(oip.c_str());
					scv.sin_port=htons(atoi(oport.c_str()));
					if(sendto(socket_dgramid,tr.c_str(),MAX,0,(struct sockaddr *)&scv,sizeof(scv))==-1)
						perror("sendto");

					int socket_streamid=socket(PF_INET,SOCK_STREAM,0);
					scv.sin_family=AF_INET;
					scv.sin_addr.s_addr=inet_addr(oip.c_str());
					scv.sin_port=htons(atoi(oport.c_str()));
					if(connect(socket_streamid,(struct sockaddr *)&scv,sizeof(scv))==-1)
					{
						shutdown(socket_dgramid,2);
						shutdown(socket_streamid,2);
						perror("connect");
						exit(1);
					}
					FILE* fp=fopen(file.c_str(),"w");

					int size;
					int err=0;
					while((size=read(socket_streamid,bufst,MAX))>0) {
						if(size==5&&strncmp(bufst,"ERROR",5)==0) {
							cout<<"File "<<file<<" doesn't exist\n";
							fclose(fp);
							close(socket_streamid);
							err=1;
							break;
						}
						fwrite(bufst,sizeof(char),size,fp);
						memset(bufst,0,MAX);
					}
					if(err) {cin>>file;continue;}
					fclose(fp);
					cout<<"file "<<file<<" download finished.\n";
					close(socket_streamid);
				}
			} 

			else {
				cout<<"File "<<file<<" doesn't exist\n";
				cout<<"Enter file to download:\n";
			}
		}

		else if(strncmp(line.c_str(),"share ",6)==0) {

			string fl1=string(line.c_str()+6);
			string tos="QSUCC ";
			int fic=hash(fl1);
			cout<<fl1<<" shared with FID "<<fic<<endl;
			char c2[10] ;
			sprintf(c2,"%d ",fic);
			tos+=string(c2);
			tos+=ip+string(" ")+port;
			struct sockaddr_in to,from2;
			to.sin_family=AF_INET;
			to.sin_port=htons(atoi(NODE0PORT));
			to.sin_addr.s_addr=inet_addr(NODE0IP);
			if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
				perror("sendto");
		
			int len2=sizeof(from2);
			if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
				perror("recvfrom");
		
			string prid="",prip="",prpo="";
			int loop=1;
			for(int j=5;j<strlen(bufdg);j++) {
				if(bufdg[j]==' ') {loop++;continue;}
				switch(loop){
					case 1: prid+=bufdg[j];break;
					case 2: prip+=bufdg[j];break;
					case 3: prpo+=bufdg[j];break;
				}
			}
		
			string nst="KEEP ";
			nst+=string(c2);
			nst+=ip+string(" ")+parport;
			to.sin_family=AF_INET;
			to.sin_port=htons(atoi(prpo.c_str()));
			to.sin_addr.s_addr=inet_addr(prip.c_str());
			if(sendto(socket_dgramid,nst.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
				perror("sendto");

		}
		else if(strncmp(line.c_str(),"exit",4)==0) {
			string dl="DELETE ";dl+=ip+string(" ")+parport;
			temp.sin_family=AF_INET;
			temp.sin_port=htons(atoi(NODE0PORT));
			temp.sin_addr.s_addr=inet_addr(NODE0IP);
			if(sendto(socket_dgramid,dl.c_str(),MAX,0,(struct sockaddr *)&temp,sizeof(temp))==-1)
				perror("sendto");
			break;
		}

		getline(cin,line);
	}

	close(socket_dgramid);
	exit(0);
}


int main() {
	string ip,port;
	ip=string(iph);
	port=string(porth);

	int id=hash(ip+string(":")+port);
	cerr<<"NODE ID : "<<id<<endl;

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
	
	string tos="QPRED ";
	struct sockaddr_in to,from,from2;
	int len2=sizeof(from2);
	string prid="",prip="",prpo="";
	int loop;
	string fl1;
	cout<<"Enter files to share('NO' at finish):\n";
	cin>>fl1;

	while(fl1!="NO") {
		tos="QSUCC ";
		int fic=hash(fl1);
		cout<<fl1<<" shared with FID "<<fic<<endl;
		char c2[10] ;
		sprintf(c2,"%d ",fic);
		tos+=string(c2);
		tos+=ip+string(" ")+port;
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(NODE0PORT));
		to.sin_addr.s_addr=inet_addr(NODE0IP);
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");
	
		len2=sizeof(from2);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
			perror("recvfrom");
	
		prid="",prip="",prpo="";
		loop=1;
		for(int j=5;j<strlen(bufdg);j++) {
			if(bufdg[j]==' ') {loop++;continue;}
			switch(loop){
				case 1: prid+=bufdg[j];break;
				case 2: prip+=bufdg[j];break;
				case 3: prpo+=bufdg[j];break;
			}
		}
	
		string nst="KEEP ";
		nst+=tos.substr(6,tos.length()-6);
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(prpo.c_str()));
		to.sin_addr.s_addr=inet_addr(prip.c_str());
		if(sendto(socket_dgramid,nst.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");

		cin>>fl1;
	}
	cout<<"File sharing over.\n";

	tos="QSUCC ";
	char t20[10];
	sprintf(t20,"%d ",(id+1)%MOD);
	tos+=string(t20);
	tos+=ip+string(" ")+port;
	to.sin_family=AF_INET;
	to.sin_port=htons(atoi(NODE0PORT));
	to.sin_addr.s_addr=inet_addr(NODE0IP);
	if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
		perror("sendto");
	len2=sizeof(from2);
	if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
		perror("recvfrom");
	printf("received message : %s\n",bufdg );
	prid="",prip="",prpo="";
	loop=1;
	for(int j=5;j<strlen(bufdg);j++) {
		if(bufdg[j]==' ') {loop++;continue;}
		switch(loop){
			case 1: prid+=bufdg[j];break;
			case 2: prip+=bufdg[j];break;
			case 3: prpo+=bufdg[j];break;
		}
	}
	FingerTable[0].ID=atoi(prid.c_str());
	FingerTable[0].ip=prip;
	FingerTable[0].port=prpo;

	tos="QPRED ";
	tos+=prid+string(" ")+ip+string(" ")+port;
	to.sin_family=AF_INET;
	to.sin_port=htons(atoi(NODE0PORT));
	to.sin_addr.s_addr=inet_addr(NODE0IP);
	if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
		perror("sendto");
	if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
		perror("recvfrom");
	printf("received message : %s\n",bufdg );
	prid="",prip="",prpo="";
	loop=1;
	for(int j=5;j<strlen(bufdg);j++) {
		if(bufdg[j]==' ') {loop++;continue;}
		switch(loop){
			case 1: prid+=bufdg[j];break;
			case 2: prip+=bufdg[j];break;
			case 3: prpo+=bufdg[j];break;
		}
	}
	pre.ID=atoi(prid.c_str());
	pre.ip=prip;
	pre.port=prpo;

	to.sin_family=AF_INET;
	to.sin_port=htons(atoi(FingerTable[0].port.c_str()));
	to.sin_addr.s_addr=inet_addr(FingerTable[0].ip.c_str());
	tos="PRED ";
	char t21[10];
	sprintf(t21,"%d ",id);
	tos+=string(t21);
	tos+=ip+string(" ")+port;
	if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
		perror("sendto");

	rep(i,M) {
		if(i==0) continue;
		tos="QSUCC ";
		char c2[10] ;
		sprintf(c2,"%d ",(id+(1<<i))%MOD);
		tos+=string(c2);
		tos+=ip+string(" ")+port;
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(NODE0PORT));
		to.sin_addr.s_addr=inet_addr(NODE0IP);
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");
		memset(bufdg,0,MAX);
		len2=sizeof(from2);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
			perror("recvfrom");
		printf("received message : %s\n",bufdg );
		while(strncmp(bufdg,"SUCC",4)){
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
			len2=sizeof(from2);
			if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
				perror("recvfrom");
			printf("received message : %s\n",bufdg );
		}
		
		prid="",prip="",prpo="";
		loop=1;
		for(int j=5;j<strlen(bufdg);j++) {
			if(bufdg[j]==' ') {loop++;continue;}
			switch(loop){
				case 1: prid+=bufdg[j];break;
				case 2: prip+=bufdg[j];break;
				case 3: prpo+=bufdg[j];break;
			}
		}
		FingerTable[i].ID=atoi(prid.c_str());
		FingerTable[i].ip=prip;
		FingerTable[i].port=prpo;
	}

	Node previous[M];
	rep(i,M) {
		tos="QPRED ";
		char c2[10] ;
		sprintf(c2,"%d ",(MOD+id+1-(1<<i))%MOD);
		tos+=string(c2);
		tos+=ip+string(" ")+port;
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(NODE0PORT));
		to.sin_addr.s_addr=inet_addr(NODE0IP);
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");
		len2=sizeof(from2);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
			perror("recvfrom");
		printf("received message : %s\n",bufdg );
		prid="",prip="",prpo="";
		loop=1;
		for(int j=5;j<strlen(bufdg);j++) {
			if(bufdg[j]==' ') {loop++;continue;}
			switch(loop){
				case 1: prid+=bufdg[j];break;
				case 2: prip+=bufdg[j];break;
				case 3: prpo+=bufdg[j];break;
			}
		}
		previous[i].ID=atoi(prid.c_str());
		previous[i].ip=prip;
		previous[i].port=prpo;
	}

	rep(i,M) {
		tos="UPDATE ";
		char c2[10] ;
		sprintf(c2,"%d ",id);
		tos+=string(c2);
		tos+=ip+string(" ")+port;
		char c3[10];
		sprintf(c3," %d",i);
		tos+=string(c3);
		to.sin_family=AF_INET;
		to.sin_port=htons(atoi(previous[i].port.c_str()));
		to.sin_addr.s_addr=inet_addr(previous[i].ip.c_str());
		if(sendto(socket_dgramid,tos.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
			perror("sendto");
	}

	printFingerTable(id);

	if(fork()==0) {
		client(ip,port);
	}

	while(1) {
	
		memset(bufdg,0,MAX);
		int len=sizeof(from);
		if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from,(socklen_t*)&len)==-1)
			perror("recvfrom");
		printf("received message : %s\n",bufdg );

		if(strncmp(bufdg,"QSUCC",5)==0) {
			string nid="",nip="",nport="";
			int turn=1;
			for(int i=6;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			if(id==FingerTable[0].ID) {
				string tu="SUCC ";
				char k[10];
				sprintf(k,"%d ",FingerTable[0].ID);
				tu+=string(k);
				tu+=FingerTable[0].ip+string(" ")+FingerTable[0].port;
				if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
					perror("sendto");
			} else {
				if(!inRange(id,FingerTable[0].ID,n1id)){
					bool sent=false;
					for(int j=M-1;j>=0;j--) {
						if(inRange(id,n1id,FingerTable[j].ID) && FingerTable[j].ID!=n1id) {
							struct sockaddr_in hjk;
							hjk.sin_family=AF_INET;
							hjk.sin_port=htons(atoi(FingerTable[j].port.c_str()));
							hjk.sin_addr.s_addr=inet_addr(FingerTable[j].ip.c_str());
							if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
								perror("sendto");
							sent=true;
							break;
						}				
					}
					if(!sent){
					string tu="SUCC ";
					char k[10];
					sprintf(k,"%d ",id);
					tu+=string(k);
					tu+=ip+string(" ")+port;
					struct sockaddr_in hjk;
					hjk.sin_family=AF_INET;
					hjk.sin_port=htons(atoi(nport.c_str()));
					hjk.sin_addr.s_addr=inet_addr(nip.c_str());
					if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
						perror("sendto");}
				}
				else {
					struct sockaddr_in hjk;
					hjk.sin_family=AF_INET;
					hjk.sin_port=htons(atoi(nport.c_str()));
					hjk.sin_addr.s_addr=inet_addr(nip.c_str());
					string tu="SUCC ";
					char k[10];
					sprintf(k,"%d ",FingerTable[0].ID);
					tu+=string(k);
					tu+=FingerTable[0].ip+string(" ")+FingerTable[0].port;
					if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
						perror("sendto");
				}
			}
		}

		else if(strncmp(bufdg,"QPRED",5)==0) {
			string nid="",nip="",nport="";
			int turn=1;
			for(int i=6;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			if(id==FingerTable[0].ID) {
				string tu="PRED ";
				char k[10];
				sprintf(k,"%d ",FingerTable[0].ID);
				tu+=string(k);
				tu+=FingerTable[0].ip+string(" ")+FingerTable[0].port;
				if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
					perror("sendto");
			} else {
				if(!inRange(id,FingerTable[0].ID,n1id)){
					bool sent=false;
					for(int j=M-1;j>=0;j--) {
						if(inRange(id,n1id,FingerTable[j].ID) && FingerTable[j].ID!=n1id) {
							struct sockaddr_in hjk;
							hjk.sin_family=AF_INET;
							hjk.sin_port=htons(atoi(FingerTable[j].port.c_str()));
							hjk.sin_addr.s_addr=inet_addr(FingerTable[j].ip.c_str());
							if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
								perror("sendto");
							sent=true;
							break;
						}				
					}
					if(!sent){
					string tu="PRED ";
					char k[10];
					sprintf(k,"%d ",pre.ID);
					tu+=string(k);
					tu+=pre.ip+string(" ")+pre.port;
					struct sockaddr_in hjk;
					hjk.sin_family=AF_INET;
					hjk.sin_port=htons(atoi(nport.c_str()));
					hjk.sin_addr.s_addr=inet_addr(nip.c_str());
					if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
						perror("sendto");}
				}
				else {
					struct sockaddr_in hjk;
					hjk.sin_family=AF_INET;
					hjk.sin_port=htons(atoi(nport.c_str()));
					hjk.sin_addr.s_addr=inet_addr(nip.c_str());
					string tu="PRED ";
					char k[10];
					sprintf(k,"%d ",id);
					tu+=string(k);
					tu+=ip+string(" ")+port;
					if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
						perror("sendto");
				}
			}
		}

		else if(strncmp(bufdg,"UPDATE",6)==0) 
		{
			string nid="",nip="",nport="",index1="";
			int turn=1;
			for(int i=7;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: nid+=bufdg[i];break;
					case 2: nip+=bufdg[i];break;
					case 3: nport+=bufdg[i];break;
					case 4: index1+=bufdg[i];break;
				}
			}
			int n1id=atoi(nid.c_str());
			int index=atoi(index1.c_str());
			if( (inRange((id+(1<<index))%MOD,FingerTable[index].ID,n1id) && n1id!=FingerTable[index].ID)
				|| n1id==(id+(1<<index))%MOD ) 
			{
				FingerTable[index].ID=n1id;
				FingerTable[index].ip=nip;
				FingerTable[index].port=nport;
				struct sockaddr_in hjk;
				hjk.sin_family=AF_INET;
				hjk.sin_port=htons(atoi(pre.port.c_str()));
				hjk.sin_addr.s_addr=inet_addr(pre.ip.c_str());
				if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
					perror("sendto");
			}
			printFingerTable(id);
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
			FingerTable[0].ID=n1id;
			FingerTable[0].ip=nip;
			FingerTable[0].port=nport;
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

		else if(strncmp(bufdg,"GET",3)==0) {
			if(fork()==0) {
				string file=string(bufdg+4);
				int new_socket_streamid;
				if((new_socket_streamid=accept(socket_streamid,(struct sockaddr*)&from,(socklen_t*)&len))==-1) {
					perror("accept");
					exit(1);
				}
				FILE *fp=fopen(file.c_str(),"r");
				string err="ERROR";
				if(!fp) {
					write(new_socket_streamid,err.c_str(),5);
					close(new_socket_streamid);
					exit(1);
				}
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
				close(new_socket_streamid);
				close(socket_streamid);
				exit(0);
			}
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
			int u=0;
			for(;u<fileinfo.size();u++) {
				if(f1id==fileinfo[u].first) break;
			}
			string tose;
			if(u==fileinfo.size()) tose="ERROR ";
			else {
				tose="FOUND ";
				tose+=fid;
				tose+=string(" ")+fileinfo[u].second.first+string(" ")+fileinfo[u].second.second;
			}
			if(sendto(socket_dgramid,tose.c_str(),MAX,0,(struct sockaddr *)&from,sizeof(from))==-1)
				perror("sendto");			
		}

		else if(strncmp(bufdg,"KEEP",4)==0) {
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

		else if(strncmp(bufdg,"DELETE",6)==0) {
			char sid[10];
			sprintf(sid,"%d ",FingerTable[0].ID);
			string tsucc="KEEP ";
			struct sockaddr_in le,le2;int l4=sizeof(le2);

			for(int j=0;j<fileinfo.size();j++) {
				le.sin_port=htons(atoi(FingerTable[0].port.c_str()));
				le.sin_family=AF_INET;
				le.sin_addr.s_addr=inet_addr(FingerTable[0].ip.c_str());
				char fd[10];
				sprintf(fd,"%d ",fileinfo[j].first);
				tsucc+=string(fd);
				tsucc+=fileinfo[j].second.first+string(" ")+fileinfo[j].second.second;
				if(sendto(socket_dgramid,tsucc.c_str(),MAX,0,(struct sockaddr *)&le,sizeof(le))==-1)
					perror("sendto");
				tsucc="KEEP ";
			}

			le.sin_port=htons(atoi(FingerTable[0].port.c_str()));
			le.sin_family=AF_INET;
			le.sin_addr.s_addr=inet_addr(FingerTable[0].ip.c_str());
			tsucc="PUP ";
			char sid2[10];sprintf(sid2,"%d ",pre.ID);tsucc+=string(sid2);
			tsucc+=pre.ip+string(" ")+pre.port;
			if(sendto(socket_dgramid,tsucc.c_str(),MAX,0,(struct sockaddr *)&le,sizeof(le))==-1)
				perror("sendto");

			rep(i,M) {
				string tostr="QPRED ";
				char c2[10] ;
				sprintf(c2,"%d ",(MOD+id+1-(1<<i))%MOD);
				tostr+=string(c2);
				tostr+=ip+string(" ")+port;
				to.sin_family=AF_INET;
				to.sin_port=htons(atoi(NODE0PORT));
				to.sin_addr.s_addr=inet_addr(NODE0IP);
				if(sendto(socket_dgramid,tostr.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
					perror("sendto");
				len2=sizeof(from2);
				memset(bufdg,0,MAX);
				if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
					perror("recvfrom");
				while(strncmp(bufdg,"PRED",4)){
					
					string nid="",nip="",nport="";
					int turn=1;
					for(int i=6;i<strlen(bufdg);i++) {
						if(bufdg[i]==' ') {turn++;continue;}
						switch(turn){
							case 1: nid+=bufdg[i];break;
							case 2: nip+=bufdg[i];break;
							case 3: nport+=bufdg[i];break;
						}
					}
					int n1id=atoi(nid.c_str());
					if(id==FingerTable[0].ID) {
						string tu="PRED ";
						char k[10];
						sprintf(k,"%d ",FingerTable[0].ID);
						tu+=string(k);
						tu+=FingerTable[0].ip+string(" ")+FingerTable[0].port;
						if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&from2,sizeof(from2))==-1)
							perror("sendto");
					} else {
						if(!inRange(id,FingerTable[0].ID,n1id)){
							bool sent=false;
							for(int j=M-1;j>=0;j--) {
								if(inRange(id,n1id,FingerTable[j].ID) && FingerTable[j].ID!=n1id) {
									struct sockaddr_in hjk;
									hjk.sin_family=AF_INET;
									hjk.sin_port=htons(atoi(FingerTable[j].port.c_str()));
									hjk.sin_addr.s_addr=inet_addr(FingerTable[j].ip.c_str());
									if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
										perror("sendto");
									break;
								}				
							}
							if(!sent){
							string tu="PRED ";
							char k[10];
							sprintf(k,"%d ",pre.ID);
							tu+=string(k);
							tu+=pre.ip+string(" ")+pre.port;
							struct sockaddr_in hjk;
							hjk.sin_family=AF_INET;
							hjk.sin_port=htons(atoi(nport.c_str()));
							hjk.sin_addr.s_addr=inet_addr(nip.c_str());
							if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
								perror("sendto");}
						}
						else {
							struct sockaddr_in hjk;
							hjk.sin_family=AF_INET;
							hjk.sin_port=htons(atoi(nport.c_str()));
							hjk.sin_addr.s_addr=inet_addr(nip.c_str());
							string tu="PRED ";
							char k[10];
							sprintf(k,"%d ",id);
							tu+=string(k);
							tu+=ip+string(" ")+port;
							if(sendto(socket_dgramid,tu.c_str(),MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
								perror("sendto");
						}
					}
					memset(bufdg,0,MAX);
					if(recvfrom(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&from2,(socklen_t*)&len2)==-1)
						perror("recvfrom");
				}
				
				prid="",prip="",prpo="";
				loop=1;
				for(int j=5;j<strlen(bufdg);j++) {
					if(bufdg[j]==' ') {loop++;continue;}
					switch(loop){
						case 1: prid+=bufdg[j];break;
						case 2: prip+=bufdg[j];break;
						case 3: prpo+=bufdg[j];break;
					}
				}
				previous[i].ID=atoi(prid.c_str());
				previous[i].ip=prip;
				previous[i].port=prpo;
			}

			tsucc="REPLACE ";
			char sid1[10];sprintf(sid1,"%d ",id);tsucc+=string(sid1);
			tsucc+=ip+string(" ")+port+string(" ");
			tsucc+=string(sid);
			tsucc+=FingerTable[0].ip+string(" ")+FingerTable[0].port;

			rep(i,M) {
				string tsucc1=tsucc;
				char c15[10];
				sprintf(c15," %d",i);
				tsucc1+=string(c15);
				to.sin_family=AF_INET;
				to.sin_port=htons(atoi(previous[i].port.c_str()));
				to.sin_addr.s_addr=inet_addr(previous[i].ip.c_str());
				if(sendto(socket_dgramid,tsucc1.c_str(),MAX,0,(struct sockaddr *)&to,sizeof(to))==-1)
					perror("sendto");
			}

			break;
		}

		else if(strncmp(bufdg,"PUP",3)==0) {
			string fid="",fip="",fport="";
			int turn=1;
			for(int i=4;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: fid+=bufdg[i];break;
					case 2: fip+=bufdg[i];break;
					case 3: fport+=bufdg[i];break;
				}
			}
			int f1id=atoi(fid.c_str());
			pre.ID=f1id;
			pre.ip=fip;
			pre.port=fport;
		}

		else if(strncmp(bufdg,"REPLACE",7)==0) {
			string rid="",rip="",rport="",fid="",fip="",fport="",index1="";
			int turn=1;
			for(int i=8;i<strlen(bufdg);i++) {
				if(bufdg[i]==' ') {turn++;continue;}
				switch(turn){
					case 1: rid+=bufdg[i];break;
					case 2: rip+=bufdg[i];break;
					case 3: rport+=bufdg[i];break;
					case 4: fid+=bufdg[i];break;
					case 5: fip+=bufdg[i];break;
					case 6: fport+=bufdg[i];break;
					case 7: index1+=bufdg[i];break;
				}
			}
			rem_entries(rip,rport);
			int index=atoi(index1.c_str());
			int f1id=atoi(fid.c_str());
			int r1id=atoi(rid.c_str());
			if(FingerTable[index].ID==r1id) {
				FingerTable[index].ID=f1id;
				FingerTable[index].port=fport;
				FingerTable[index].ip=fip;
				struct sockaddr_in hjk;
				hjk.sin_family=AF_INET;
				hjk.sin_port=htons(atoi(pre.port.c_str()));
				hjk.sin_addr.s_addr=inet_addr(pre.ip.c_str());
				if(sendto(socket_dgramid,bufdg,MAX,0,(struct sockaddr *)&hjk,sizeof(hjk))==-1)
					perror("sendto");
			}
			printFingerTable(id);
		}

	}

	close(socket_dgramid);
	close(socket_streamid);
	return 0;

}