#include <stdio.h>
#include <vmci_sockets.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>


#define BUFFSIZE 16384*10
enum {
    BUFSIZE = 4096,
    CONNECTION_BACKLOG = 10,
};


#ifdef _WIN32
int execmd(char* cmd,char* result) {
    char buffer[BUFFSIZE];

	printf("window run command %s\n",cmd);
	FILE* pipe = _popen(cmd, "r"); 
    if (!pipe)
          return 0;

    while(!feof(pipe)) {
    if(fgets(buffer, BUFFSIZE, pipe)){  
            strcat(result,buffer);
        }
    }
//	printf("info: %s \n",result);
//	printf("info-1: %s \n",buffer);
    _pclose(pipe);                    
    return 1;      
}
#else
int execmd(char* cmd,char* result) {
    char buffer[BUFFSIZE];              

    printf("window run command %s\n",cmd);
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
          return 0; 

    while(!feof(pipe)) {
    if(fgets(buffer, BUFFSIZE, pipe)){     
            strcat(result,buffer);
        }
    }
    pclose(pipe);
    return 1;
}
#endif
void handler(int s)
{

         if (s == SIGSEGV) printf(" now got a segmentation violation signal\n");
         if (s == SIGILL) printf(" now got an illegal instruction signal\n");
         if (s == SIGABRT) printf(" now got an abt instruction signal\n");
//         exit(1);

}


int main(int argc, char * argv[])

{


    struct sockaddr_vm my_addr = { 0 };
#if _WIN32
    WSADATA wsaData = { 0 };
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
        printf("Could not register with Winsock DLL.\n");
        goto cleanup;
    }
#endif

    int afVMCI = VMCISock_GetAFValue();
 #if _WIN32
	SOCKET sockfd = INVALID_SOCKET;
    SOCKET client_fd = INVALID_SOCKET;
#else
	int sockfd = 0;
	int client_fd = 0;
	
#endif

#ifdef _WIN32	
	system("chcp 437");
#else
/*add language support*/
#endif

    if ((sockfd = socket(afVMCI, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        goto cleanup;
    }

    my_addr.svm_family = afVMCI;
    my_addr.svm_cid = VMADDR_CID_ANY;
    my_addr.svm_port = 12345;

    if (bind(sockfd, (struct sockaddr*) &my_addr, sizeof my_addr) == -1) {
        perror("bind");
        goto close;
    }

	unsigned int cid;

    if ((cid = VMCISock_GetLocalCID()) == (unsigned int)-1) {
        fprintf(stdout, "VMCISock_GetLocalCID failed\n");
    } 

#ifdef _WIN32
	int size = sizeof(my_addr);
#else
	socklen_t size = sizeof(my_addr);
#endif

    if (getsockname(sockfd, (struct sockaddr *)&my_addr, &size) == -1) {
        perror("getsockname");
        goto close;
    }
    fprintf(stderr, "server (cid, port): %u:%u\n", cid, my_addr.svm_port);

	fd_set read_fds;    
	int nfds;
	char recv_buf[BUFFSIZE];
	char buff[BUFFSIZE];
    
	for (;;) {
#ifdef _WIN32
        if (listen(sockfd, 5) == -1) {
#else
		if (listen(sockfd, CONNECTION_BACKLOG) == -1){
#endif
            perror("listen");
            goto close;
        }

		struct sockaddr_vm their_addr;
#ifdef _WIN32
		int size = sizeof(their_addr);
#else
    	socklen_t size = sizeof(their_addr);
#endif

        if ((client_fd = accept(sockfd, (struct sockaddr *) &their_addr, &size)) == -1) {
            perror("accept");
            goto close;
        }
        fprintf(stderr, "client connected\n");

        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);
        nfds = client_fd + 1;
        if (select(nfds, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            goto close;
        }

     	memset(recv_buf,0,sizeof(recv_buf));
        if (FD_ISSET(client_fd, &read_fds)) {
            int s;
            if ((s = recv(client_fd, (void*)recv_buf, sizeof(recv_buf), 0)) < 0) {
                fprintf(stderr, "recv failed: %s\n", strerror(errno));
            } else {
                int i;
				int numbytes = 0;
                fprintf(stderr, "recved %d bytes\n", (int)s);
                for (i=0; i<s; i++) {
                    putc(recv_buf[i], stderr);
                }
                putc('\n', stderr);
				int r=0;
			 	r= execmd((char *)recv_buf,(char *)buff);
				if(r == 0){
					printf("run window command failed!");
					goto close;
				}
				else{
					printf("result:%s\n",buff);
				}


				numbytes = send(client_fd,buff, strlen(buff), 0);
				if(numbytes == -1)
				{
				#ifdef _WIN32
					printf("send err %d\n", WSAGetLastError());
			   		system("echo """);
				#endif
				}
     			memset(buff,0,sizeof(buff));
		}
	}
#ifdef _WIN32
	closesocket(client_fd);
#else
	close(client_fd);
#endif
	fprintf(stderr, "%s","connected closed\n");
}

close:
#ifdef _WIN32
	 closesocket(sockfd);
#else
    close(sockfd);
#endif

cleanup:
#ifdef _WIN32
    WSACleanup();
	system("pause");
#endif
    return 0;
}
