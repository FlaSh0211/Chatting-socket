#include	"unp.h"

void client(FILE *fp, int sockfd, char* username)
{
	
	
	char	line[MAXLINE]; 
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	int		maxfdp1;
	
	fd_set		rset;	
	FD_ZERO(&rset);
	
	
	for ( ; ; ) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd)+ 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {	
			if (Readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("client: 오류");
			Fputs(recvline, stdout);
		}

		if (FD_ISSET(fileno(fp), &rset)) {  
			if (Fgets(line, MAXLINE, fp) == NULL)
				return;		
			if(strcmp(line, "q!")==0)
				exit(0);
			else if(strcmp(line, "/list\n")==0){
				sprintf(sendline, "%s", line);
				Writen(sockfd, sendline, strlen(sendline));
				
			}

			sprintf(sendline, "%s: %s", username, line);
			Writen(sockfd, sendline, strlen(sendline));
		}
	}	
	
}

int
main(int argc, char **argv)
{
	int 		sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 4)
		err_quit("오류");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	Writen(sockfd, argv[3], sizeof(argv[3]));
	client(stdin,sockfd,argv[3]);

	

	exit(0);
}
