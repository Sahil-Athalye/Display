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
#include "tree.h"

struct file_line {
    //struct rb_node node;
    int line_key;
    char * line_string;
};

//#define BUFLEN 15000
 #define BUFLEN 300

#define PERROR printf("Program error, reason: %s\n", strerror(errno));

#define CHECK(retval, line) \
    if (retval < 0) { \
        printf("At line %d : ", line); \
        PERROR; \
        return -1; \
    } \

int connect_to_server(const char * shell_ip_address, const int port_num, const int sfd) {

    struct sockaddr_in inet_address;
    int ret;

    memset(&inet_address, 0, sizeof(struct sockaddr_in));
    inet_address.sin_family = AF_INET;
    inet_address.sin_port = htons(port_num); // htons translates to network representation
    ret =  inet_aton(shell_ip_address,  &inet_address.sin_addr);
    CHECK(ret, __LINE__);

    // struct in_addr server_ip_addr = inet_address.sin_addr;
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

    int temp_fd = open("intermediary", O_CREAT | O_RDWR | O_APPEND);
    CHECK(temp_fd, __LINE__);

    FILE * temp_stream = fdopen(temp_fd, "a+");
    if (temp_stream == NULL) {
        printf("fdopen failed!\n");
        return -1;
    }

    while (1) {

        char * temp_buffer = malloc(BUFLEN);

        chars_read = read(sfd, temp_buffer, BUFLEN);
        CHECK(chars_read, __LINE__);
        if (strstr(temp_buffer, "FIN")) { // Server is done sending the fragment

            close(temp_fd);
            fclose(temp_stream);
            int ret = unlink("intermediary");
            CHECK(ret, __LINE__);
            return 0;
        }
        total_chars_read += chars_read;

        temp_buffer[chars_read-1]=0;

        // Dump what we've read onto an intermediary stream
        fseek(temp_stream, 0, SEEK_END);
        int ret = fprintf(temp_stream, temp_buffer);
        CHECK(ret, __LINE__);
        ret = fflush(temp_stream);
        CHECK(ret, __LINE__);

        free(temp_buffer);

        fseek(temp_stream, 0 , SEEK_SET);
        if (getdelim(line_string, line_len, '\n', temp_stream) != -1) {
            break;
        }
    }

    close(temp_fd);
    fclose(temp_stream);
    int ret = unlink("intermediary");
    CHECK(ret, __LINE__);

    return total_chars_read;
}

const int num_expected_args = 3;

int
main(int argc, char *argv[])
{

    if (argc != num_expected_args) {
        printf("usage: ./client <server ip> <port>\n");
        return -1;
    }

    const char * shell_ip_address = argv[1];
    const int port_num = atoi(argv[2]);

    char * hostname = malloc(BUFLEN);
    gethostname(hostname, BUFLEN);
    printf("Server hostname is %s\n", hostname);
    free(hostname);

    // Socket File Descriptor
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sfd, __LINE__);

    int ret = connect_to_server(shell_ip_address, port_num, sfd);
    CHECK(ret, __LINE__);


    char * line = NULL;
    size_t line_len = 0;

    size_t read = 0;
    struct Node *root = NULL;

    // Accept fragment lines from the server
    int i = 0;
    while( (read = line_reader(sfd, &line, &line_len)) != 0 ) {
        printf("Read %d (len %ld): \n", i, line_len);
        printf("%s", line);
        printf("\n");

        char * node_line = malloc(line_len+1);
        strcpy(node_line, line);
        // We will free this copy later

        root = insert(root, node_line);
        printf("Preorder traversal of the constructed AVL"
                " tree is: \n");
        preOrder(root);
        printf("\n");

        free(line);
        line = NULL;
        line_len = 0;
        i++;
    }

    // Write fragment lines back to the server in sorted order.

    printf("Root = %s", root->string);

    while (root != NULL) {
        struct Node * min = minValueNode(root);

        char * line = min->string;
        int line_len = strlen(line);

        printf("Wrote %d: %s\n", line_len, min->string);
        write(sfd, line, line_len);
        // usleep(1000);
        free(min->string);
        // Free the string we copied into our node

        root = deleteNode(root, min->key);
    }

    printf("Terminating connection\n");
    write(sfd, "FIN", strlen("FIN"));



    // Close sends causes EPOLLRDHUP on the CFD that the server is polling to represent this connection
    if (close(sfd) < 0) {
        printf( "Failed to close SFD\n");
        PERROR;
        return -5;
    }

    return 0;
}