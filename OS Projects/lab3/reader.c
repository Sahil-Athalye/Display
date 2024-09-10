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
        if (getdelim(line_string, line_len, '@', temp_stream) != -1) {
            break;
        }
    }

    close(temp_fd);
    fclose(temp_stream);
    int ret = unlink("intermediary");
    CHECK(ret, __LINE__);

    return total_chars_read;
}