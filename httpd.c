/* J. David's webserver */
/* This is a simple webserver.
 * Created November 1999 by J. David Blackstone.
 * CSE 4344 (Network concepts), Prof. Zeigler
 * University of Texas at Arlington
 */

/*
 *add by liuyunbin
 *   
 *   http请求报文包括
 *   1. 请求行 requestline：包括请求方法字段(get或post),URL字段和HTTP协议版本字段
 *   2. 请求头部 header
 *   3. 空行
 *   4. 请求数据
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

void print_error_and_exit(const char *st) {
 	perror(st);
 	exit(1);
}

/*从客户端读取一行字符，将\r\n和\r换成\n*/
int get_line_from_socket(int sock, char *buf, int size) {
 	int i = 0;
 	char c = '\0';

	/* i + 1 < size 为'\0' 空出一位来  */
 	while (i + 1 < size && c != '\n' && recv(sock, &c, 1, 0) > 0 ) {		
   		if (c == '\r') {
    			/*MSG_PEEK 表示数据并未从缓存中删除,下一次读取的还是该字符*/
    			if (recv(sock, &c, 1, MSG_PEEK) > 0 && c == '\n')
     				recv(sock, &c, 1, 0);
    			else
     				c = '\n';
   		}
   		buf[i++] = c;
  	}
 	buf[i] = '\0';
 
 	return i;
}

/* 读取并忽略header之前的信息   */
void read_and_discard_heads(int client) {
	char buf[1024];

	while (get_line_from_socket(client, buf, sizeof(buf)) > 0 && strcmp("\n", buf) != 0)  /* read & discard headers */
		;
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.        */
/**********************************************************************/
void bad_request(int client) {
 	char buf[1024];

 	snprintf(buf, sizeof(buf), "HTTP/1.0 400 BAD REQUEST\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "Content-type: text/html\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "\r\n");
 	send(client, buf, strlen(buf), 0);
	snprintf(buf, sizeof(buf), "<P>Your browser sent a bad request, ");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "such as a POST without a Content-Length.\r\n");
 	send(client, buf, strlen(buf), 0);
}


/**********************************************************************/
/* Inform the client that a CGI script could not be executed.         */
/**********************************************************************/
void cannot_execute(int client) {
 	char buf[1024];

 	snprintf(buf, sizeof(buf), "HTTP/1.0 500 Internal Server Error\r\n");
	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "Content-type: text/html\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "<P>Error prohibited CGI execution.\r\n");
 	send(client, buf, strlen(buf), 0);
}


/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
/* execute_cgi()  执行文件
 * 1. 创建两个管道，cgi_output[2]用于子程序向主程序传递信息, cgi_input[2]用于主程序向子程序传递信息
 * 2. 子程序：
 *            将标准输出重定向到cgi_output[1], 将标准输入重定向到cgi_input[0]
 *            关闭管道cgi_output[0]的输入口,cgi_input[1]的输出口
 *            添加环境变量，并执行程序
 * 3. 主程序： 
 *            关闭管道cgi_output的输出口,cgi_input的输入口
 *            如果 method为POST，将从客户端接收的数据通过管道cgi_input发送给子程序
 *            将通过管道cgi_outout接收的数据发送给客户端
 *            关闭管道cgi_output的输入口cgi_input的输出口，等待子程序的消亡
 */
void execute_cgi(int client, const char *path, const char *method, const char *query_string) {
 	char buf[1024] = "A";
 	int cgi_output[2];  /*子程序向主程序传递信息的管道*/
 	int cgi_input[2];   /*主程序向子程序传递信息的管道*/
 	pid_t pid;
 	int i;
 	char c;
	int content_length = -1;

 	if (strcasecmp(method, "GET") == 0)
   		read_and_discard_heads(client);
 	else    /* POST */ {
  		while (get_line_from_socket(client, buf, sizeof(buf)) > 0 && strcmp("\n", buf) != 0) {
   			buf[15] = '\0';
   			if (strcasecmp(buf, "Content-Length:") == 0)
    				content_length = atoi(buf+16);
  		}
  		if (content_length == -1) {
   			bad_request(client);
   			return ;
  		}
 	}

 	snprintf(buf, sizeof(buf), "HTTP/1.0 200 OK\r\n");
 	send(client, buf, strlen(buf), 0);

 	if (pipe(cgi_output) < 0 || pipe(cgi_input) < 0 || (pid = fork()) < 0) {
  		cannot_execute(client);
  		return ;
 	}
 	if (pid == 0)  /* child: CGI script */ {
  		char meth_env[255];
  		char query_env[255];
  		char length_env[255];

  		dup2(cgi_output[1], 1);   /*将标准输出重定向到管道cgi_output*/
  		dup2(cgi_input[0], 0);    /*将标准输入重定向到管道cgi_input*/
  		close(cgi_output[0]);     /*关闭cgi_output的输入端*/
  		close(cgi_input[1]);      /*关闭cgi_input 的输出端*/
  		snprintf(meth_env, sizeof(meth_env), "REQUEST_METHOD=%s", method);
  		putenv(meth_env);
  		if (strcasecmp(method, "GET") == 0) {
   			snprintf(query_env, sizeof(query_env), "QUERY_STRING=%s", query_string);
   			putenv(query_env);
  		}
  		else {   /* POST */
   			snprintf(length_env, sizeof(length_env), "CONTENT_LENGTH=%d", content_length);
   			putenv(length_env);
  		}
  		execl(path, path, NULL);
  		exit(0);
 	}
 	else {    /* parent */
  		close(cgi_output[1]);
  		close(cgi_input[0]);
  		if (strcasecmp(method, "POST") == 0) {
   			for (i = 0; i < content_length; i++) {
    				recv(client, &c, 1, 0);
    				write(cgi_input[1], &c, 1);
   			}
		}
  		while (read(cgi_output[0], &c, 1) > 0)
   			send(client, &c, 1, 0);

  		close(cgi_output[0]);
 		close(cgi_input[1]);
  		waitpid(pid, NULL, 0);
 	}
}

/**********************************************************************/
/* Return the informational HTTP headers about a file.                */
/**********************************************************************/
void headers(int client, const char *filename) {
 	char buf[1024];
 	(void)filename;  /* could use filename to determine file type */

 	snprintf(buf, sizeof(buf), "HTTP/1.0 200 OK\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "%s", SERVER_STRING);
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "Content-Type: text/html\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "\r\n");
 	send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message.                      */
/**********************************************************************/
void not_found(int client) {
 	char buf[1024];

 	snprintf(buf, sizeof(buf), "HTTP/1.0 404 NOT FOUND\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), SERVER_STRING);
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "Content-Type: text/html\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "<HTML><TITLE>Not Found</TITLE>\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "<BODY><P>The server could not fulfill\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "your request because the resource specified\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "is unavailable or nonexistent.\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "</BODY></HTML>\r\n");
 	send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.                                 */
/**********************************************************************/
void serve_file(int client, const char *filename) {
 	FILE *resource = NULL;	
 	char buf[1024];

	read_and_discard_heads(client);
 	resource = fopen(filename, "r");
 	if (resource == NULL) {
  		not_found(client);
	}
 	else {
  		headers(client, filename);
 		while (fgets(buf, sizeof(buf), resource) != NULL)
  			send(client, buf, strlen(buf), 0);
 	}
 	fclose(resource);
}


/*****************************************************************************/
/* Inform the client that the requested web method has not been implemented. */
/*****************************************************************************/
void unimplemented(int client) {
 	char buf[1024];

 	snprintf(buf, sizeof(buf), "HTTP/1.0 501 Method Not Implemented\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), SERVER_STRING);
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "Content-Type: text/html\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "</TITLE></HEAD>\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "<BODY><P>HTTP request method not supported.\r\n");
 	send(client, buf, strlen(buf), 0);
 	snprintf(buf, sizeof(buf), "</BODY></HTML>\r\n");
 	send(client, buf, strlen(buf), 0);
}

void output_client_message(int client) {
 	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);

  	getpeername(client, (struct sockaddr *)&client_name, &client_name_len);
	printf( " client ip: %s client port: %d \n", inet_ntoa(client_name.sin_addr), ntohs(client_name.sin_port) );
}


/*************************************************************************/
/* A request has caused a call to accept() on the server port to return. */
/*************************************************************************/
/* accept_request() 与客户端交互
 * 1. 从客户端获得请求行数据
 * 2. 将请求行数据切割成method和url，method为GET或POST，url为请求资源
 * 3. 将路径信息存储在path中
 * 4. 如果method为POST或method为GET且url有参数，则表示有CGI程序，cgi=1
 * 5. 如果method为GET且url有参数，则将query_string指向参数
 * 6. 查看文件资源信息，如果找不到，则返回给客户端
 * 7. 如果文件具有可执行权限，cgi=1
 * 8. 如果cgi=0,将文件返回给客户端，否则，执行该文件
 */
void* accept_request(void* arg) {
	char buf[1024];
 	int  client = *(int*)(arg);
 	char *method = NULL;
 	char *url = NULL;
 	char path[512];
 	struct stat st;
 	int cgi = 0;      /* becomes true if server decides this is a CGI program */
 	char *query_string = NULL;

	free(arg);
	output_client_message(client);
	get_line_from_socket(client, buf, sizeof(buf));
	method = strtok(buf, " ");
 	if (method == NULL || (strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0)) {
  		unimplemented(client);
  		return NULL;
 	}

	if (strcasecmp(method, "POST") == 0)
  		cgi = 1;
	url = strtok(NULL, " ");
	if (url == NULL) {
		unimplemented(client);
		return NULL;
	}

 	if (strcasecmp(method, "GET") == 0) {
  		query_string = strchr(url, '?');
  		if (query_string != NULL) {
  			cgi = 1;
   			*query_string++ = '\0';
 		}
	}

	snprintf(path, sizeof(path), "htdocs%s", url);
 	if (path[strlen(path) - 1] == '/')
 		snprintf(path, strlen(path), "%s%s", path, "index.html");
 	if (stat(path, &st) == -1) {
		read_and_discard_heads(client);
  		not_found(client);
 	}
 	else {
  		if ((st.st_mode&S_IFMT) == S_IFDIR)
   			snprintf(path, strlen(path), "%s%s", path, "/index.html");
  		if ((st.st_mode&S_IXUSR) || (st.st_mode&S_IXGRP) || (st.st_mode&S_IXOTH) )
   			cgi = 1;
  		if (cgi == 0)
   			serve_file(client, path);
  		else
   			execute_cgi(client, path, method, query_string);
 	}
 	close(client);

 	return NULL;
}

void output_server_message(int server) {
 	struct sockaddr_in server_name;
	socklen_t server_name_len = sizeof(server_name);

  	getsockname(server, (struct sockaddr *)&server_name, &server_name_len);
	printf( " server ip: %s server port: %d \n", inet_ntoa(server_name.sin_addr), ntohs(server_name.sin_port) );
}

/****************************************************************************************/
/*
 *     socket() -----> bind() ----->  listen()
 */
/****************************************************************************************/
int get_server_socket() {
 	int httpd_sock;
 	u_short port = 0;
 	struct sockaddr_in name;

 	httpd_sock = socket(PF_INET, SOCK_STREAM, 0);
 	if (httpd_sock == -1)
  		print_error_and_exit("socket");
 	memset(&name, 0, sizeof(name));
 	name.sin_family = AF_INET;
 	name.sin_port = htons(port);
 	name.sin_addr.s_addr = htonl(INADDR_ANY);
 	if (bind(httpd_sock, (struct sockaddr *)&name, sizeof(name)) < 0)
  		print_error_and_exit("bind");
 	if (listen(httpd_sock, 5) < 0)
  		print_error_and_exit("listen");

 	return httpd_sock;
}

/**********************************************************************/
/*  
 *  get_server_socket() ---> accept() ---> pthread_create()  --
 *                             ^                              |
 *                             |                              |   
 *                             --------------------------------  
 */
/**********************************************************************/
int main(void) {
 	int server_sock = get_server_socket();
 	int *client_sock;
 	pthread_t newthread;

	output_server_message(server_sock);

 	while (1) {
		client_sock = (int*)malloc(sizeof(int));
  		*client_sock = accept(server_sock, NULL, NULL);
  		if (*client_sock == -1)
   			print_error_and_exit("accept");
 		if (pthread_create(&newthread , NULL, accept_request, client_sock) != 0)
   			print_error_and_exit("pthread_create");
 	}

 	close(server_sock);

 	return 0;
}

