// dynaread()
/*
int dynaread(int fd, const long max, struct pfixbuf *infobuf);

dynaread() attempts to read max bytes from fd, and writes information
about into infobuf containing information about the length of the buffer
and a pointer to the start of the buffer.

On failure -1 is returned, and pfixbuf is set to the following.

struct pfixbuf
{
    long buflen;    // buffer length, set to 0 on failure
    void *buf;      // buffer pointer set to NULL on failure
};
*/

