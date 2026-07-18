/* Minimal newlib syscall implementations.

   Some newlib code paths expect POSIX-style syscalls: stdio's FILE glue
   references _read/_write/_close/_lseek/_fstat/_isatty, and abort() raises
   a signal via _kill/_getpid. With -specs=nosys.specs those come from
   libnosys, whose stubs carry .gnu.warning sections — every ALGO=sprintf
   link printed "_write is not implemented and will always fail" and friends.

   Defining the symbols here makes the linker prefer these object-file
   versions, so libnosys's warned members are never pulled and the warnings
   disappear. Semantics:

   - _write() routes to the semihosting console via _semihost_write0()
     (SYS_WRITE0), chunked through a small NUL-terminated buffer — so
     printf()/puts() output lands in the OpenOCD terminal like the
     harness's own messages. Embedded NUL bytes end a chunk early;
     acceptable for text output.
   - _fstat() reports a character device and _isatty() returns 1, giving
     stdio sane unbuffered/line-buffered console behavior.
   - The rest fail cleanly (or report EOF / pid 1) without touching state.

   Like the heap (heap.cpp), none of this is linked into the baseline:
   every function sits in its own section (-ffunction-sections) and is
   stripped by --gc-sections unless newlib actually references it. */

#include <errno.h>
#include <sys/stat.h>

void _semihost_write0(const char *s);

/* Write `len` bytes to the semihosting console. SYS_WRITE0 needs a
   NUL-terminated string, so copy through a bounded chunk buffer. */
int _write(int fd, const char *buf, int len)
{
    (void)fd;
    char chunk[65];
    int remaining = len;

    while (remaining > 0) {
        int n = remaining > (int)(sizeof(chunk) - 1U) ? (int)(sizeof(chunk) - 1U)
                                                      : remaining;
        for (int i = 0; i < n; i++) {
            chunk[i] = buf[i];
        }
        chunk[n] = '\0';
        _semihost_write0(chunk);
        buf += n;
        remaining -= n;
    }
    return len;
}

/* No input source: always end-of-file. */
int _read(int fd, char *buf, int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return 0;
}

int _close(int fd)
{
    (void)fd;
    errno = EBADF;
    return -1;
}

/* Console is a character device: not seekable, position always 0. */
int _lseek(int fd, int off, int whence)
{
    (void)fd;
    (void)off;
    (void)whence;
    return 0;
}

/* Report a character device so stdio treats fds as a console. */
int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}

int _getpid(void)
{
    return 1;
}

/* No processes/signals on bare metal; abort()'s raise(SIGABRT) ends here. */
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}
