#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>

struct file_line {
    //struct rb_node node;
    int line_key;
    char * line_string;
};

#define BUFLEN 200

#define PERROR printf("Program error, reason: %s\n", strerror(errno));
#define CHECK(retval) \
    if (retval < 0) { \
        PERROR; \
    } \

int connect_to_server(const char * shell_ip_address, const int port_num, const int sfd) {

    struct sockaddr_in inet_address;

    memset(&inet_address, 0, sizeof(struct sockaddr_in));
    inet_address.sin_family = AF_INET;
    inet_address.sin_port = htons(port_num); // htons translates to network representation
    if ( inet_aton(shell_ip_address,  &inet_address.sin_addr) < 0 ) {
        printf(" inet_aton() error");
        PERROR;
        return -3;
    }

    struct in_addr server_ip_addr = inet_address.sin_addr;
    in_port_t server_port = inet_address.sin_port; // just a uint16

    socklen_t inet_address_len = sizeof(inet_address);
    if (connect(sfd, (struct sockaddr *) &inet_address, inet_address_len) < 0) {
        printf(" Error connecting to SFD via address \n");
        PERROR;
        return -3;
    }
    printf(" Connection made with server at ip %s at port %d\n", shell_ip_address, ntohs(server_port));

    return 0;
}

// Wraps getline
// mutates line_string and line_len when we have received a full line on our intermediary fd
// returns the total number of characters detected by 'read'
// ideally return value == line_len
size_t line_reader(int sfd, char ** line_string, size_t * line_len) {

    size_t chars_read = 0;
    size_t total_chars_read = 0;
    
    char * temp_buffer = malloc(BUFLEN);

    int temp_fd = open("intermediary", O_RDWR | O_APPEND);
    FILE * temp_stream = fdopen(temp_fd, "rw");

    // While we haven't been able to read a full line from the intermediary stream
    while ( getline(line_string, line_len, temp_stream) == 0) {
        chars_read = read(sfd, temp_buffer, BUFLEN);
        total_chars_read += chars_read;

        // Dump what we've read onto an intermediary stream
        fwrite(temp_buffer, chars_read, chars_read, temp_stream);
    }

    free(temp_buffer);
    close(temp_fd);
    fclose(temp_stream);

    return total_chars_read;
}

size_t line_writer(int sfd, char * line_string, size_t * line_len) {


    size_t chars_written = 0;
    size_t total_chars_written = 0;

    char * temp_buffer = malloc(BUFLEN);

    // just advance the pointer within str1 until ptr + buf_len >= line_len
        // int write1 = write(sfd, str1, strlen(str1));

    free(temp_buffer);


    return total_chars_written;
}




int process_line(char * recv_line, struct file_line * fl) {
    // Parse line number out of recv_line
    int recv_line_len = strlen(recv_line);

    int line_no_size;
    for (line_no_size = 0 ; line_no_size < recv_line_len ; line_no_size++) {
        if (recv_line[line_no_size] = ' ') {
            break;
        }
    }

    char * line_no_substr = malloc(line_no_size);
    memcpy(line_no_substr, recv_line, line_no_size);

    int line_no = atoi(line_no_substr);
    free(line_no_substr);

    fl->line_key = line_no;
    char * recv_line_copy = malloc(recv_line_len);
    memcpy(recv_line_copy, recv_line, recv_line_len); // Recv_line will get freed as the next line is communicated via socket
    
    fl->line_key = atoi(recv_line);
    fl->line_string = recv_line_copy;

    return 0;
}

int
main(int argc, char *argv[])
{

    const char * shell_ip_address = "128.252.167.161";
    const int port_num = 35000;
        
    char * hostname = malloc(BUFLEN);
    gethostname(hostname, BUFLEN);
    printf("Server hostname is %s\n", hostname);
    free(hostname);

    if (argc < 2) {
        printf("Error, must provide 1 argument\n");
        return -1;
    }

    // Socket File Descriptor
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        printf(" Error creating socket \n");
        PERROR;
        return -2;
    }

    int ret = connect_to_server(shell_ip_address, port_num, sfd);
    if (ret < 0) {
        return ret;
    }


    if (strcmp(argv[1], "quit") == 0) {
        
        char * q = "quit";
        int buf1_wrote = write(sfd, q, strlen(q));
        if (buf1_wrote <=0) {
            printf(" Failed to send q_code\n");
            PERROR;
            return -4;
        }
        printf("Sent 'quit' signal to server\n");
        return 0;
    }

    char * str1 = " This is message 1 \n";
    int write1 = write(sfd, str1, strlen(str1));
    printf("write1 %d\n", write1);
    if (write1 <=0) {
        printf(" Failed write1\n");
        PERROR;
        return -4;
    }

    char * str2 = " This is message 2 \n";
    int write2 = write(sfd, str2, strlen(str2));
    printf("write2 %d\n", write2);
    if (str2 <=0) {
        printf(" Failed write2\n");
        PERROR;
        return -4;
    }

    // Close sends causes EPOLLRDHUP on the CFD that the server is polling to represent this connection
    if (close(sfd) < 0) {
        printf( "Failed to close SFD\n");
        PERROR;
        return -5;
    }
}
