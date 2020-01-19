#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>//internet protocol family
#include<string.h>
#include<unistd.h>
#define PORT 8080
char *typemime;
void response();
void sendfile();

int main(){
	int socket_create;
	struct sockaddr_in server;
	int accept_socket;
	int len;
	int bufsize = 2048;    
   	char *buffer = malloc(bufsize);
	char type[4];
	char path[1024];

	//create socket with sockert function (domain,type,protocol)
	if((socket_create = socket(AF_INET, SOCK_STREAM, 0)) > 0){
		printf(" Created the Socket\n ");
	}

	//bind socket

	server.sin_family = AF_INET; // match the socket() call
	server.sin_addr.s_addr = htonl(INADDR_ANY); //bind to any local address
	server.sin_port = 0; //specify port to listen on

	if (bind(socket_create, (struct sockaddr *) &server, sizeof server ) == -1){
		printf("error Binding\n");
	}
	len = sizeof server;
	if(getsockname(socket_create, (struct sockaddr *) &server, &len)==-1){
		perror("getting socket name\n");
	}
	printf("port is #%d\n",ntohs(server.sin_port));
	while (1){
		// listen to incoming connection
		if (listen(socket_create, 10) < 0){
			printf("Listening error \n");
			exit(1);
		}
		//start accepting connection

		if ((accept_socket =  accept(socket_create , (struct sockaddr *) &server, &len)) < 0){
			printf("error accepting\n");
		}
		
		recv(accept_socket, buffer,bufsize,0);
        printf("%s\n",buffer );
        write(accept_socket , "Server is ok\n" , 12);

		//different file read
		read(socket_create,buffer,1024);
		sscanf(buffer,"%s %s",type,path);	
        char *mi = strrchr(path, '.')+1;
		char *nama = strtok(path, "/");


		if(mi){
			typemime = mi;
		}else{
			typemime = NULL;
		}

		if(!strcmp(type, "GET") && !strcmp(path, "/")) {
            char *data = "create GET request";
            typemime = "text/html";
			response(accept_socket, "HTTP/1.1 200 OK", data,strlen(data));
			
        }else{
            sendfile(accept_socket,nama);
        }
		close(accept_socket);

	}
	close(socket_create);
	return 0;

}
void response(int accept_socket, char *str , void *data, int length){
            char res[length+100];
			int re_length = sprintf(res,
				"%s\n"
				"Length: %d\n"
				"Type: %s\n"
				"\n",
				str,
				length,
				typemime);
				memcpy(res + re_length, data, length );
				send(accept_socket, res, re_length + length, 0);
}
void sendfile(int accept_socket, char *nama){
	char *src;
			FILE *file = fopen(nama,"r");
			size_t bufsize;
			if(file != NULL){
				if(fseek(file, 0L, SEEK_END) == 0){
					bufsize = ftell(file);
					src = malloc(sizeof(char) * (bufsize + 1));
					fseek(file, 0L, SEEK_SET);
					fread(src,sizeof(char), bufsize,file);
					response(accept_socket,"HTTP/1.1 200 OK", src, bufsize);
				}
			}else{
				char * er = "FILE NOT FOUND";
				typemime = "text/html";
				response(accept_socket,"HTTP/1.1 404 NOT FOUND",er,strlen(er));
			}
}