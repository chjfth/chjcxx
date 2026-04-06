// This is included by parent directory fsapi.h .
// Do not compile this file alone.


#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////////
namespace fsapi { 
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.

// [2026-04-06] Code assisted by DeepSeek.


// Helper function to convert errno to your error codes
static fserror_et posix_xerr(int err)
{
    switch(err)
    {
    case ENOENT:
        return E_not_found;
    case EACCES:
    case EPERM:
        return E_access_denied;
    case ENOSPC:
        return E_disk_full;
    case EINVAL:
        return E_bad_param;
    case EEXIST:
        // File exists - handled differently in open logic
        return E_unknown;
    }
    return E_unknown;
}

// Convert your flags to POSIX open flags
static int convert_open_flags(open_for_et ofor, open_share_et oshare)
{
    int flags = 0;
    
    // Access mode
    if((ofor & open_for_read) && (ofor & (open_for_write|open_for_append)))
        flags = O_RDWR;
    else if(ofor & open_for_read)
        flags = O_RDONLY;
    else if(ofor & (open_for_write|open_for_append))
        flags = O_WRONLY;
    else
        flags = O_RDONLY; // default to read if nothing specified
    
    // Creation flags
    if(ofor & (open_for_write|open_for_append))
    {
        flags |= O_CREAT;  // Create if not exists
        // Note: O_APPEND handled separately below
    }
    else
    {
        flags |= O_EXCL; // For read-only, ensure file exists
    }
    
    // Append mode
    if(ofor & open_for_append)
        flags |= O_APPEND;
    
    // Share modes - Linux doesn't have direct equivalent
    // We'll ignore oshare for now, but you could implement
    // using fcntl(F_SETLK) for advisory locking
    
    return flags;
}

filehandle_t 
file_open(const TCHAR *filepath, open_for_et ofor, open_share_et oshare)
{
    // Convert flags
    int flags = convert_open_flags(ofor, oshare);
    
    // Permissions for newly created files (rw-r--r--)
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    // Open the file
    int fd = open(filepath, flags, mode);
    
    if(fd >= 0)
    {
        // If opening for write/append and file was newly created,
        // but you wanted OPEN_ALWAYS behavior, it's already handled
        return (filehandle_t)fd; // success
    }
    else
    {
        int err = errno;
        return (filehandle_t)posix_xerr(err);
    }
}

fserror_et 
file_close(filehandle_t fh)
{
    int fd = (int)fh;
    int result = close(fd);
    
    if(result == 0)
        return E_success;
    else
        return posix_xerr(errno);
}

int // mux fserror_et
file_read(filehandle_t fh, void *rbuf, int bytes_to_read)
{
    if(bytes_to_read == 0)
        return 0;
    else if(bytes_to_read < 0)
        return E_bad_param;
    
    int fd = (int)fh;
    ssize_t bytesRead = read(fd, rbuf, bytes_to_read);
    
    if(bytesRead >= 0)
        return (int)bytesRead;
    else
        return posix_xerr(errno);
}

int // mux fserror_et
file_write(filehandle_t fh, const void *wbuf, int bytes_to_write)
{
    if(bytes_to_write == 0)
        return 0;
    else if(bytes_to_write < 0)
        return E_bad_param;
    
    int fd = (int)fh;
    ssize_t bytesWritten = write(fd, wbuf, bytes_to_write);
    
    if(bytesWritten >= 0)
        return (int)bytesWritten;
    else
        return posix_xerr(errno);
}

__int64 file_seek(filehandle_t fh, __int64 offset, whence_et whence)
{
    int fd = (int)fh;
    
    // Convert your whence to POSIX
    int posix_whence;
    switch(whence)
    {
    case seek_set: posix_whence = SEEK_SET; break;
    case seek_cur: posix_whence = SEEK_CUR; break;
    case seek_end: posix_whence = SEEK_END; break;
    default: return E_bad_param;
    }
    
    off_t result = lseek(fd, (off_t)offset, posix_whence);
    
    if(result != (off_t)-1)
        return (__int64)result;
    else
        return posix_xerr(errno);
}

fserror_et 
file_setsize(filehandle_t fh, __int64 newsize)
{
    int fd = (int)fh;
    
    // Seek to new size
    __int64 seekpos = file_seek(fh, newsize, seek_set);
    if(seekpos < 0)
        return (fserror_et)seekpos;
    
    // Truncate at current position
    int result = ftruncate(fd, (off_t)newsize);
    
    if(result == 0)
        return E_success;
    else
        return posix_xerr(errno);
}

__int64 file_getsize(filehandle_t fh)
{
    int fd = (int)fh;
    struct stat st;
    
    int result = fstat(fd, &st);
    
    if(result == 0)
        return (__int64)st.st_size;
    else
        return posix_xerr(errno);
}


////////////////////////////////////////////////////////////////////////////
} // namespace fsapi
////////////////////////////////////////////////////////////////////////////
