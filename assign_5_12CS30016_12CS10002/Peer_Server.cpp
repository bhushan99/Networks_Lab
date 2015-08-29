#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <iostream>
#include <vector>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 100003
#define BACKLOG 10
#define MAX 256
#define FIS_PORT 100001
#define FIS_IP "10.102.46.60"

using namespace std;

int main(){
	int fis_id,tr_id,cli_id;
	struct sockaddr_in fis_addr,addrport,cli_addr;
	string file=string("FILE "),temp;

	cout<<"Enter names of files you want to send to FIS (Enter NO if not any file) :\n";

	if((fis_id=socket(AF_INET,SOCK_DGRAM,0))==-1){
		perror("Socket() for server:");
		exit(1);
	}

	fis_addr.sin_family=AF_INET;
	fis_addr.sin_addr.s_addr=inet_addr(FIS_IP);
	fis_addr.sin_port=htons(FIS_PORT);
	int fis_len=sizeof(fis_addr);

	cin>>temp;
	file=file+temp;

	while(temp.compare("NO")!=0) {
		if(sendto(fis_id,file.c_str(),256,0,(struct sockaddr *)&fis_addr,(socklen_t)fis_len)==-1)
			perror("sendto() FIS:");
		cin>>temp;
		file=string("FILE ")+temp;
	}

	if((cli_id=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Socket() for Peer CLient:");
		exit(1);
	}

	addrport.sin_family=AF_INET;
	addrport.sin_addr.s_addr=INADDR_ANY;
	addrport.sin_port=htons(PORT);
	if(bind(cli_id,(struct sockaddr *)&addrport,sizeof(addrport))==-1)
		perror("bind()");

	while(1){

		if(listen(cli_id,BACKLOG)==-1)
			perror("listen()");
		int clilen=sizeof(cli_addr);
		tr_id=accept(cli_id,(struct sockaddr *)&cli_addr,(socklen_t *)&clilen);
		if(tr_id==-1){
			perror("accept()");
			continue;
		}
		if(fork()==0)
			break;
	}

	pid_t pid=getpid();
	int ipAddr=cli_addr.sin_addr.s_addr;
	char str[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
	char buf[MAX],*buffer=(char*)malloc(MAX*sizeof(char));
	read(tr_id,buf,MAX);
	cout<<pid<<":peer server connected to Peer Client "<<str<<" File reqested: "<<buf<<"\n";
	FILE *fp=fopen(buf,"r");

	fseek(fp,0,SEEK_END);
	int f_sz=ftell(fp);
	rewind(fp);

	int size=0;
	int nbytes=min(f_sz,MAX-1);
	while((size=fread(buffer,sizeof(char),nbytes,fp))>0){
		write(tr_id,buffer,size);
		free(buffer);
		buffer=(char*)malloc(MAX*sizeof(char));
		memset(buffer,0,MAX);
		f_sz-=size;
		nbytes=min(f_sz,MAX-1);
	}	
	fclose(fp);
	cout<<pid<<" file sent\n";
	shutdown(tr_id,2);
	return 0;
}
