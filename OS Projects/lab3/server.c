#define _GNU_SOURCE

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/epoll.h>
#include <dirent.h>
#include <fcntl.h>

#include "tree.h"

#define PERROR printf("Program error, reason: %s\n", strerror(errno));

#define CHECK(retval, line) \
    if (retval < 0) { \
        printf("At line %d : ", line); \
        PERROR; \
        return -1; \
    } \

#define BUFLEN 300

const int MAX_FILENAME_SIZE = 100;
const int BUFFER_SIZE = 4096;

size_t line_reader(int sfd, char ** line_string, size_t * line_len);

int print_client_address(int client_addr_len, struct sockaddr_in client_addr) {
     struct in_addr client_ip_addr;
    client_ip_addr = client_addr.sin_addr;
    char * str_client_ip_addr = malloc(client_addr_len);
    inet_ntop(AF_INET, &client_ip_addr, str_client_ip_addr, client_addr_len);

    in_port_t client_port = client_addr.sin_port; // just a uint16

    printf(" Connection made with client from ip %s at port %d\n", str_client_ip_addr, ntohs(client_port));

    free(str_client_ip_addr);
    return 0;
}

int
main(int argc, char *argv[])
{
    int ret;

    if (argc < 3) {
        printf("Error, must provide filename and socket argument\n");
        return -1;
    }

    // Get the name of the file we will write our final merge copy to
    FILE* guide_stream = fopen(argv[1], "r");
    // char * output_filename = malloc(MAX_FILENAME_SIZE);
    // fgets(output_filename, MAX_FILENAME_SIZE, guide_stream);

    char * output_filename = NULL;
    size_t outfile_name_len = 0;

    size_t num_read;
    num_read = getline(&output_filename, &outfile_name_len, guide_stream);

    if (output_filename[num_read-1] == '\n') {
        output_filename[num_read-1] = 0;
    }

    printf("Reading file: %s\n", output_filename);
    int outfd = open(output_filename, O_CREAT | O_RDWR);
    free(output_filename);

    // See how many fragments we have
    printf("Detecting fragments: \n");

    char * fragment_name = NULL;
    size_t fragment_name_len = 0;
    int num_fragments = 0;

    while ( getline(&fragment_name, &fragment_name_len, guide_stream) != -1 ) {
        printf("%s", fragment_name);
        num_fragments++;
        fragment_name_len = 0;
    }

    printf("\nFound %d fragments\n", num_fragments);

    // Re-open and re-read now that we know the length of memory we need to allocate
    // 'read-fragmentnum', we can re-iterate over the lines, and open read file descriptors

    ret = fseek(guide_stream, 0 , SEEK_SET);
    CHECK(ret, __LINE__);

    // Allocate pollFDs
    int total_sockets = num_fragments + 1; // We need to monitor an FD for our SFD and every fragment CFD
    struct pollfd *pollFd;
    pollFd = calloc(total_sockets, sizeof(struct pollfd));

    // Allocate and initialize FDs for reading fragment files
    // int * fragment_fds = malloc(num_fragments * sizeof(int));
    FILE** fragment_streams = malloc(num_fragments * sizeof(FILE *));

    int line_no = 0;
    int cur_fragment = 0;

    free(fragment_name);
    fragment_name = NULL;
    fragment_name_len = 0;

    while ( (num_read = getline(&fragment_name, &fragment_name_len, guide_stream)) != -1 ) {

        // NOTE: You need to add a new line on the last LINE OF THE GUIDE FILE
        if (line_no > 0) { // If we are past the first line of the guide file

            printf("Opening %s", fragment_name);

            if (fragment_name[num_read-1] == '\n') {
                fragment_name[num_read-1] = 0;
            }

            FILE * fragment_stream = fopen(fragment_name, "r");
            if (fragment_stream == 0) {
                printf("fdopen failed!\n");
            }
            fragment_streams[cur_fragment] = fragment_stream;

            cur_fragment++;
        }

         line_no++;
        fragment_name_len = 0;
    }

    free(fragment_name);

    cur_fragment = 0;


    char * hostname = malloc(100);
    gethostname(hostname, 100);
    printf("Server hostname is %s\n", hostname);
    const int port_num = atoi(argv[2]); // might error?
    printf("Port number is %d\n", port_num);
    free(hostname);


    // Socket File Descriptor
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sfd, __LINE__);

    struct sockaddr_in inet_address;
    memset(&inet_address, 0, sizeof(struct sockaddr_in));
    inet_address.sin_family = AF_INET;
    inet_address.sin_port = htons(port_num); // htons translates to network representation
    inet_address.sin_addr.s_addr = INADDR_ANY;
    // When we want to bind a socket, but don't know the specific address of our machine

    ret = bind(sfd, (struct sockaddr *) &inet_address, sizeof(inet_address));
    CHECK(ret, __LINE__);

    ret = listen(sfd, 1);
    CHECK(ret, __LINE__);

    pollFd[0].fd = sfd;
    pollFd[0].events = POLLIN;
    int n_active = 1;
    int n_done_client = 0;

    struct Node *root = NULL;


    while(n_done_client < num_fragments) {
        // TODO: this is here because otherwise server is sending everything at once, before client is ready to read?
        usleep(250);
        ret = poll(pollFd, n_active, -1);
        CHECK(ret, __LINE__);

        if (pollFd[0].revents & POLLIN) {
            // Accept a new client connection
            printf("Accepting new client connection\n");

            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(struct sockaddr_in);

            int cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len); // Update to the client addr we have connected to
            CHECK(ret, __LINE__);

            print_client_address(client_addr_len, client_addr);

            // Add this to the list we are monitoring
            printf("Added a socket for our %d'th fragment stream\n", n_active);
            pollFd[n_active].fd = cfd;
            pollFd[n_active].events = POLLOUT;

            n_active++;
            cur_fragment++;

        }

        // Go from 1 to the current_open_client
        // Checking for requests to write our fragment to the client
        int i;
        for (i = 1; i <= n_active; i++) {

            int fragment = i-1;

            printf("checking fragment %d \n", i);

            if (pollFd[i].revents & POLLOUT) {
                printf("Writing fragment to client\n");

                FILE * stream = fragment_streams[fragment];
                char * line = NULL;
                size_t line_len = 0;
                ssize_t bytes_read = getline(&line, &line_len, stream);

                // int line_delimited_len = line_len+1+1;
                // char * line_delimited = malloc(line_delimited_len);
                // strcpy(line_delimited, line);
                // strcat(line_delimited, "@");


                if (bytes_read == -1) { // We've reached end of fragment
                    // Send a special message to designate that we're ending the fragment
                    printf("Terminating connection\n");
                    write(pollFd[i].fd, "FIN", strlen("FIN"));
                    pollFd[i].events = POLLIN;
                    continue;
                }
                else {
                    // Write the line
                    printf("Writing line %s", line);
                    // if (write(pollFd[i].fd, line_delimited, line_delimited_len) != line_delimited_len) {
                    if (write(pollFd[i].fd, line, line_len) != line_len) {
                        perror("write failed");
                        exit(EXIT_FAILURE);
                    }
                }

                free(line);
                //free(line_delimited);
            }

            else if (pollFd[i].revents & POLLIN) {
                char * line = NULL;
                size_t line_len = 0;
                size_t read = 0;

                read = line_reader(pollFd[i].fd, &line, &line_len);
                if (read == 0) {
                    // Once client is done writing
                    // Swap this fd to the back of the poll array
                    // and decrement n_active;
                    printf("done reading");
                    pollFd[i].events = 0;
                    ++n_done_client;
                    continue;
                }

                printf("Read %d (len %ld): \n", i, line_len);
                printf("%s", line);
                printf("\n");

                char * node_line = malloc(line_len+1);
                strcpy(node_line, line);
                root = insert(root, node_line);
                free(line);
                line = NULL;
                line_len = 0;
            }
        }
    }

    printf("Preorder traversal of the constructed AVL"
            " tree is \n");
    preOrder(root);

    // Copied from client code
    while (root != NULL) {
        struct Node * min = minValueNode(root);

        char * line = min->string;
        int line_len = strlen(line);

        printf("Wrote %d: %s\n", line_len, min->string);
        write(outfd, line, line_len);
        free(min->string);
        // Free the string we copied into our node

        root = deleteNode(root, min->key);
    }

    close(outfd);

    return 0;

}

const char * intermediary_name = "intermediary_srv";

size_t line_reader(int sfd, char ** line_string, size_t * line_len) {

    size_t chars_read = 0;
    size_t total_chars_read = 0;

    int temp_fd = open(intermediary_name, O_CREAT | O_RDWR | O_APPEND);
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
            int ret = unlink(intermediary_name);
            CHECK(ret, __LINE__);
            return total_chars_read;
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
    int ret = unlink(intermediary_name);
    CHECK(ret, __LINE__);

    return total_chars_read;
}