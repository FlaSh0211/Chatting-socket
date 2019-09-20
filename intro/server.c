
#include	"unp.h"

int
main(int argc, char **argv)
{
	
	int	i, j, k, l, maxi, maxfd, listenfd, connfd, sockfd;
	int count=0, nready, client[FD_SETSIZE];

	
	char readedline[MAXLINE], message[MAXLINE];
	char	buf[MAXLINE],buf1[MAXLINE],buf2[MAXLINE],buf3[MAXLINE],buf4[MAXLINE],buf5[MAXLINE],buf6[MAXLINE];
	char	bbuf[MAXLINE]; 	
	char name[FD_SETSIZE][MAXLINE];
	ssize_t	n;
	fd_set	rset, allset;
	ssize_t namelen;
	socklen_t	clilen;
	
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	
	printf("[address: %s : %d]\n", Inet_ntop(AF_INET, &servaddr.sin_addr, buf, sizeof(buf)), ntohs(servaddr.sin_port));
	
	maxfd = listenfd;			
	maxi = -1;					
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;		
	
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	FD_SET(fileno(stdin), &allset);

	for ( ; ; ) {
		rset = allset;		
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	
			clilen = sizeof(cliaddr);
			
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
					
			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
							
			if (i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset);	
			if (connfd > maxfd)
				maxfd = connfd;			
			if (i > maxi)
				maxi = i;	
			
			if((namelen=Read(connfd, name[i], MAXLINE))!=0)
				printf("%s is connected from  %s\n", name[i], Inet_ntop(AF_INET, &cliaddr.sin_addr, buf4, sizeof(buf4)));
				
			snprintf(buf1, sizeof(buf1), "[%s is connected]\r\n", name[i]);
			for(j=0;j<maxi;j++){
				if(j==i) continue;
				if(client[j]!=-1)
					Writen(client[j], buf1, strlen(buf1));	
			}

			if (--nready <= 0)
				continue;			
		}
		
	
		
		for (i = 0; i <= maxi; i++) {	
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = Readline(sockfd, buf2, MAXLINE)) == 0) {
						
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					printf("%s is leaved at %s\n", name[i], Inet_ntop(AF_INET, &cliaddr.sin_addr, buf4, sizeof(buf4)));
				} 
				else{
					if(strcmp(buf2,"/list\n")==0){
						count=0;
						for(l=0;l<=maxi;l++)
							if(client[l]!=-1){
								count++;
								sprintf(buf6,"[%s from %s : %d]\n", name[l], Inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), ntohs(cliaddr.sin_port));
								strcat(bbuf, buf6);
							}
						
						sprintf(buf5,"[the number of current user is %d]\n", count);
						//Writen(sockfd, buf5, sizeof(buf5));
						strcat(buf5, bbuf);
						Writen(sockfd, buf5, sizeof(buf5));
				
					}
					else{
						for(j=0; j <=maxi;j++){
							if(i==j) continue;
							if((sockfd=client[j]) > 0)
								Writen(sockfd, buf2, n);
						}
					}
				}
			
				for(k=0;k<MAXLINE;k++){
					buf2[k]='\0';
					buf5[k]='\0';
					bbuf[k]='\0';
				}

				if (--nready <= 0)
					break;
				
			}				
		}
	}
}

