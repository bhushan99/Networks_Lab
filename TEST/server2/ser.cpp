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


int main(void)
{
fd_set master;
// master file descriptor list
fd_set read_fds; // temp file descriptor list for select()
struct sockaddr_in myaddr;
// server address
struct sockaddr_in remoteaddr; // client address
int fdmax;
// maximum file descriptor number
int listener;
// listening socket descriptor
int newfd;
// newly accept()ed socket descriptor
char buf[256];
// buffer for client data
int nbytes;
int yes=1;
// for setsockopt() SO_REUSEADDR, below
int addrlen;
int i, j;
FD_ZERO(&master);
FD_ZERO(&read_fds);
// clear the master and temp sets
// get the listener
if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
perror("socket");
exit(1);
}
// lose the pesky "address already in use" error message
if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) {
perror("setsockopt");
exit(1);
}
// bind
myaddr.sin_family = AF_INET;
myaddr.sin_addr.s_addr = INADDR_ANY;
myaddr.sin_port = htons(PORT);
memset(&(myaddr.sin_zero), ’\0’, 8);
if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
perror("bind");
exit(1);
}
// listen
if (listen(listener, 10) == -1) {
perror("listen");
exit(1);
}
// add the listener to the master set
FD_SET(listener, &master);
fdmax = listener; // so far, it’s this one
// main loop
for(;;) {
read_fds = master; // copy it
if (select(fdmax+1, NULL,&read_fds, NULL, NULL) == -1) {
perror("select");
exit(1);
}
// run through the existing connections looking for data to read
for(i = 0; i <= fdmax; i++) {
if (FD_ISSET(i, &read_fds)) { // we got one!!
if (i == listener) {
// handle new connections
addrlen = sizeof(remoteaddr);
if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr,
&addrlen)) == -1) {
perror("accept");
} else {
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
}
} else {
// handle data from a client
if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
// got error or connection closed by client
if (nbytes == 0) {
// connection closed
printf("selectserver: socket %d hung up\n", i);
} else {
perror("recv");
}
close(i); // bye!
FD_CLR(i, &master); // remove from master set
} else {
// we got some data from a client
for(j = 0; j <= fdmax; j++) {
// send to everyone!
if (FD_ISSET(j, &master)) {
// except the listener and ourselves
if (j != listener && j != i) {
if (send(j, buf, nbytes, 0) == -1) {
perror("send");
}
}
}
}
}
} // it’s SO UGLY!
}
}
}
return 0;
}

