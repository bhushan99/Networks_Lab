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
#include <arpa/inet.h>
#include <cstring> 

#define PORT 100001
#define BACKLOG 10
#define MAX 256

using namespace std;

int main(){
	map<string,string> files;
	int s_id;
	struct sockaddr_in addrport;
	addrport.sin_family = AF_INET;
	addrport.sin_addr.s_addr = INADDR_ANY;
	addrport.sin_port = htons(PORT);

	if((s_id=socket(AF_INET,SOCK_DGRAM,0))==-1){
		perror("Server Socket ");
		exit(1);
	}

	if(bind(s_id,(struct sockaddr *)&addrport,sizeof(addrport))==-1)
		perror("FIS bind()");

	while(1){
		struct sockaddr_in from;
		char buf[256];
		int from_len=sizeof(from);
		if(recvfrom(s_id,buf,sizeof(buf),0,(struct sockaddr *)&from,(socklen_t *)&from_len)==-1)
			perror("FIS recvfrom()");

		int ipAddr=from.sin_addr.s_addr;
		char str[INET_ADDRSTRLEN];
		inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

		if(strncmp(buf,"FILE",4)==0){
			files[string(buf+5)]=string(str);
			cout<<"File information obtained from "<<str<<"\n";
		}
		else{
			if(sendto(s_id,files[string(buf+9)].c_str(),256,0,(struct sockaddr *)&from,(socklen_t)from_len)==-1)
				perror("FIS sendto()");
			cout<<"File information sent to "<<str<<"\n";
		}
	}

	return 0;
}