#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>
#include <string>
#include <fstream>
#include <time.h>
using namespace std;
#define PORT 8000
#define HOST "0.0.0.0"
#define BUFF_MAX 1024

enum FileType {
	HTML,
	CSS,
	PNG,
	JPEG,
	JS,
	XCON,
	GIF,
    MP4,
    FLAC,
    None
};

int request_handler(int new_socket, char* request);
int send_response(int new_socket, char* send_buffer, int datalen);
int login(string username, string password);
FileType parse_type(char* file_name);

char error_header[] = "HTTP/1.1 500 Internal Server Error\r\n\n";

int main(void)
{
	int opt = 1; // for setsockopt
	int pid, recvstat;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int new_socket, valread;
	
	char recv_buffer[BUFF_MAX] = { 0 };
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		cout << "Fail to create a socket." << endl;
		exit(1);
	} else {
		cout << "Create a socket success." << endl;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	bind(sockfd, (struct sockaddr*)&address, sizeof(address));

	listen(sockfd, 20);

	while ((new_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) > 0) {
		// cout << "accepted" << endl;
		if ((pid = fork()) < 0) {
			// fork error
			cout << "fork error" << endl;
			exit(1);
		}
		if (pid == 0) {
			// inside child process
			cout << "start child process" << endl;
			if (close(sockfd) != 0) {
				cout << "close sockfd in child error" << endl;
				// exit(1);
			}
            while (1) {
                if ((recvstat = recv(new_socket, recv_buffer, BUFF_MAX, 0)) <= 0) {
                    cout << "recv error" << endl;
                    break;
                    // exit(1);
                }
                cout << "recived " << recvstat << "bytes" << endl;
                cout << "request: " << endl;
                cout << "===============================================" << endl;
                cout << recv_buffer << endl;
                cout << "===============================================" << endl;
                request_handler(new_socket, recv_buffer); // TODO
            }
			
			if (close(new_socket) != 0) {
				cout << "close new socket in child error" << endl;
				// exit(1);
			}
			cout << "close child process" << endl;
			exit(0);
		} else {
			// pid > 0, inside parent process
			if (close(new_socket) != 0) {
				cout << "close new socket in child error" << endl;
				// exit(1);
			}
		}
	}

	return 0;
}

int request_handler(int new_socket, char* request)
{
	FILE *fp;
	char send_buffer[BUFF_MAX], buffer1[100];
	long int f_size;
	int size, nmemb, delta;
	const char* d = " ";
	char *method, *target, *http_version;
	char *headerline, *bodyline;
	FileType ftype;
	// int register_status, login_status;
	char Location[100];
	time_t     now;
	struct tm  tstruct;
	char       tbuf[80];

	// TODO extrate the file to be sent and calculate the length
	method = strtok(request, d);
	cout << "method: " << method << endl;
	target = strtok(NULL, d);
	cout << "target: "<< target << endl;
	http_version = strtok(NULL, "\n");
	cout << "http_version: "<< http_version << endl;
    
    // open file for GET request
	if (strcmp(method, "GET") == 0) {
    	ftype = parse_type(target);
		if (strcmp(target, "/") == 0) {
			fp = fopen("index.html", "r");
			ftype = HTML;
            size = sizeof(char);
            nmemb = BUFF_MAX;
        } else {
			cout << "==================" << ftype << endl;
            switch (ftype) {
                case HTML: case CSS: case JS:
                    if ((fp = fopen(target + 1, "r")) == NULL) {
                        send_response(new_socket, error_header, strlen(error_header));
                        fclose(fp);
                        return -1;
                    }
                    size = sizeof(char);
                    nmemb = BUFF_MAX;
                    break;
                case PNG: case JPEG: case XCON: case GIF: case MP4: case FLAC:
                    if ((fp = fopen(target + 1, "rb")) == NULL) {
                        send_response(new_socket, error_header, strlen(error_header));
                        fclose(fp);
                        return -1;
                    }
                    size = 1;
                    nmemb = sizeof(send_buffer);
                    break;
                default:
                    cout << "No such file" << endl;
                    send_response(new_socket, error_header, strlen(error_header));
                    return -1;
                    break;
            }
        }
        fseek(fp, 0L, SEEK_END);
        f_size = ftell(fp);
        rewind(fp);
		strcpy(send_buffer, "HTTP/1.1 200 OK\r\n");

	} else if (strcmp(method, "POST") == 0) {
		// if (strcmp(target, "/login") == 0) {
		// 	while ((headerline = strtok(NULL, "\n")) && headerline[0] != '\r') {
		// 		// cout << "headerline: "<< headerline << endl;
		// 		continue;
		// 	}
		// 	bodyline = strtok(NULL, "\n");
		// 	cout << "bodyline: "<< bodyline << endl;
		// 	istringstream input_stream(bodyline);
		// 	string key;
		// 	string value;
		// 	string username, password;
		// 	while (getline(input_stream, key, '=')) {
		// 		getline(input_stream, value, '&');
		// 		if (key == "username") {
		// 			username = value;
		// 		} else if (key == "password") {
		// 			password = value;
		// 		}
		// 	}
		// 	cout << "username: " << username << endl;
		// 	cout << "password: " << password << endl;
		// 	login_status = login(username, password);
		// 	// fp = fopen("index.html", "r");
		// 	// fseek(fp, 0L, SEEK_END);
		// 	// f_size = ftell(fp);
		// 	// // fseek(fp, SEEK_SET, 0);
		// 	// rewind(fp);
		// 	// ftype = HTML;
		// 	fp = fopen("user.html", "r");
		// 	ftype = HTML;
        //     size = sizeof(char);
        //     nmemb = BUFF_MAX;
		// 	strcpy(Location, "/user.html");
		// 	fseek(fp, 0L, SEEK_END);
		// 	f_size = ftell(fp);
		// 	rewind(fp);
		// 	strcpy(send_buffer, "HTTP/1.1 303 OK\r\n");
		// 	// strcpy(send_buffer, "Set-Cookie: isLoggedIn=true\r\n");
		// 	// strcpy(send_buffer, "Set-Cookie: username=ray\r\n");
		// }
		if (strcmp(target, "/sendmsg") == 0) {
			while ((headerline = strtok(NULL, "\n")) && headerline[0] != '\r') {
				// cout << "headerline: "<< headerline << endl;
				continue;
			}
			bodyline = strtok(NULL, "\n");
			cout << "bodyline: "<< bodyline << endl;
			istringstream input_stream(bodyline);
			string key;
			string value;
			string username, message;
			while (getline(input_stream, key, '=')) {
				getline(input_stream, value, '&');
				if (key == "username") {
					username = value;
				} else if (key == "message") {
					message = value;
				}
			}
			cout << "username: " << username << endl;
			cout << "message: " << message << endl;

			ofstream outfile;
			outfile.open("file.html", ios_base::app); // append instead of overwrite
			now = time(0);
			tstruct = *localtime(&now);
			strftime(tbuf, sizeof(tbuf), "%Y-%m-%d.%X", &tstruct);
			outfile << "[" << tbuf << "] " << username << ": " << message << "<br>\n"; 
			fp = fopen("board.html", "r");
			ftype = HTML;
            size = sizeof(char);
            nmemb = BUFF_MAX;
			strcpy(Location, "/board.html");
			fseek(fp, 0L, SEEK_END);
			f_size = ftell(fp);
			rewind(fp);
			strcpy(send_buffer, "HTTP/1.1 303 OK\r\n");
		}
	}

    // generate and send request header
	// strcpy(send_buffer, "HTTP/1.1 200 OK\r\n");
    strcat(send_buffer, "Connection: keep-alive\r\n");
	strcat(send_buffer, "Cache-control: public, max-age=0\r\n");
	sprintf(buffer1, "Content-Length: %ld\r\n", f_size);
	if (strlen(Location) != 0) {
		strcat(send_buffer, "Location: ");
		strcat(send_buffer, Location);
		strcat(send_buffer, "\r\n");
	}
	strcat(send_buffer, buffer1);
	switch (ftype) {
		case HTML:
			strcat(send_buffer, "Content-type: text/html; charset=UTF-8\r\n");
			break;
		case XCON:
			strcat(send_buffer, "Content-type: image/x-icon\r\n");
			break;
		case JPEG:
			strcat(send_buffer, "Content-type: image/jpg\r\n");
			break;
		case PNG:
			strcat(send_buffer, "Content-type: image/png\r\n");
			break;
		case JS:
			strcat(send_buffer, "Content-type: application/javascript\r\n");
			break;
		case CSS:
			strcat(send_buffer, "Content-Type: text/css; charset=utf-8\r\n");
			break;
		case GIF:
			strcat(send_buffer, "Content-Type: image/gif\r\n");
			break;
        case MP4:
            strcat(send_buffer, "Content-Type: video/mp4\r\n");
            break;
        case FLAC:
            strcat(send_buffer, "Content-Type: audio/flac\r\n");
            break;
		default:
			strcat(send_buffer, "Content-Type: type error\r\n");
	}
	strcat(send_buffer, "\r\n");

	send_response(new_socket, send_buffer, strlen(send_buffer));

	//size = sizeof(char); // 1
	//nmemb = BUFF_MAX - 1; // sizeof(send_buffer)
	
    // send the request content
    cout << "start sending" << endl;
	while (f_size > 0) {
		delta = fread(send_buffer, size, nmemb, fp);
        if (delta <= nmemb && ferror(fp)) {
            cout << "Fail to read the file" << endl;
            fclose(fp);
            return -1;
        }
        send_response(new_socket, send_buffer, delta);
        f_size -= delta;
        //cout << delta << " bytes sent" << endl;
	}
    cout << "===============================================" << endl;
    fclose(fp);
	return 0;
}

int send_response(int new_socket, char* send_buffer, int datalen)
{
	int sent = 0;
	//cout << "start sending" << endl;
	while (datalen > 0) {
		sent = send(new_socket, send_buffer, datalen, 0);
		if (sent < 0) {
			cout << "send error" << endl;
			return -1;
		}
		if (sent == 0) {
			cout << "client has disconnected" << endl;
			return -1;
		}
		send_buffer += sent;
		datalen -= sent;
		//cout << sent << " bytes sent" << endl;
	}
	//cout << "===============================================" << endl;
	return 0;
}

int login(string username, string password)
{
	return 0;
}

FileType parse_type(char* file_name)
{
    int i;
    char type[10];
    
    for (i = 0; i < strlen(file_name); i++) {
        if (file_name[i] == '.') {
            strcpy(type, &file_name[i]);
            break;
        }
    }
    
    if (strcmp(type, ".html") == 0) {
        return HTML;
    } else if (strcmp(type, ".css") == 0) {
        return CSS;
    } else if (strcmp(type, ".png") == 0) {
        return PNG;
    } else if (strcmp(type, ".jpeg") == 0 || strcmp(type, ".jpg") == 0) {
        return JPEG;
    } else if (strcmp(type, ".js") == 0) {
        return JS;
    } else if (strcmp(type, ".xcon") == 0) {
        return XCON;
    } else if (strcmp(type, ".gif") == 0) {
        return GIF;
    } else if (strcmp(type, ".mp4") == 0) {
        return MP4;
    } else if (strcmp(type, ".flac") == 0) {
        return FLAC;
    } else {
        return None;
    }
}