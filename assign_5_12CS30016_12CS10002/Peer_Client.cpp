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

#define PORT 100002
#define BACKLOG 10
#define MAX 256
#define FIS_PORT 100001
#define PS_PORT 100003
#define FIS_IP "10.102.46.60"

using namespace std;

int main(){

	string str;
	int i=0,j;
	
	while(1){
		cout<<getpid()<<" Enter the file you want to download (enter NO for no download):\n";
		cin>>str;
		if(str.compare("NO")==0){
			for(j=0;j<i;j++)
				wait();
			return 0;
		}
		i++;
		if(fork()==0)
			break;
	}

	char ps_ip[50],ps_port[7],data[MAX];
	int fis_id,ps_id;
	struct sockaddr_in ps_addr,fis_addr;
	string file=string("DOWNLOAD ")+str;
	pid_t pid=getpid();

	if((fis_id=socket(AF_INET,SOCK_DGRAM,0))==-1){
		perror("Socket() for server");
		exit(1);
	}
	cout<<pid<<":Socket Created for communication with FIS\n";

	fis_addr.sin_family=AF_INET;
	fis_addr.sin_addr.s_addr=inet_addr(FIS_IP);
	fis_addr.sin_port=htons(FIS_PORT);
	int fis_len=sizeof(fis_addr);
	if(sendto(fis_id,file.c_str(),256,0,(struct sockaddr *)&fis_addr,(socklen_t)fis_len)==-1)
		perror("Peer Client sendto() FIS");
	cout<<pid<<":Request sent to FIS\n";
	if(recvfrom(fis_id,ps_ip,sizeof(ps_ip),0,(struct sockaddr *)&fis_addr,(socklen_t *)&fis_len)==-1)
		perror("rcvfrom() FIS");
	cout<<pid<<":Peer Server information received. File located on "<<ps_ip<<endl;


	if((ps_id=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Socket() for Peer Server");
		exit(1);
	}
	cout<<pid<<":Socket Created for communication with Peer Server\n";
	ps_addr.sin_family=AF_INET;
	ps_addr.sin_addr.s_addr=inet_addr(ps_ip);
	ps_addr.sin_port=htons(PS_PORT);
	int ps_len=sizeof(ps_addr);

	if(connect(ps_id,(struct sockaddr *)&ps_addr,ps_len)==-1){
		perror("connect()");
		cout<<"Cannot connect to Peer Server\n";
		exit(1);
	}
	cout<<pid<<":Connected to Peer Server "<<ps_ip<<"\n";
	write(ps_id,str.c_str(),MAX);
	int size;
	char *buffer=(char*)malloc(MAX*sizeof(char));
	FILE *fp=fopen(str.c_str(),"w");
	while((size=read(ps_id,buffer,MAX))>0){
		fwrite(buffer,sizeof(char),size,fp);
		free(buffer);
		buffer=(char*)malloc(MAX*sizeof(char));
		memset(buffer,0,MAX);
	}
	fclose(fp);
    cout<<pid<<"File "<<str.c_str()<<" download finished\n";
	return 0;
}