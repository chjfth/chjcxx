#ifndef __errno2name_h_
#define __errno2name_h_

#include <errno.h>

#ifndef __cplusplus
extern"C"{
#endif

const char *errno2name(int errnum);

#ifndef __cplusplus
} // extern"C"
#endif


#ifdef ERRNO2NAME_IMPL // user should define this in one of his .c/.cpp to get implementation

struct errno2name_map_st
{
	int errnum;
	const char *errname;
};

#define E2N(e) { e, #e },

static errno2name_map_st s_map[] =
{ 
	E2N(EPERM)    /* Operation not permitted */
	E2N(ENOENT)    /* No such file or directory */
	E2N(ESRCH)    /* No such process */
	E2N(EINTR)    /* Interrupted system call */
	E2N(EIO)    /* I/O error */
	E2N(ENXIO)    /* No such device or address */
	E2N(E2BIG)    /* Argument list too long */
	E2N(ENOEXEC)    /* Exec format error */
	E2N(EBADF)    /* Bad file number */
	E2N(ECHILD)    /* No child processes */
	E2N(EAGAIN)    /* Try again */
	E2N(ENOMEM)    /* Out of memory */
	E2N(EACCES)    /* Permission denied */
	E2N(EFAULT)    /* Bad address */
	E2N(ENOTBLK)    /* Block device required */
	E2N(EBUSY)    /* Device or resource busy */
	E2N(EEXIST)    /* File exists */
	E2N(EXDEV)    /* Cross-device link */
	E2N(ENODEV)    /* No such device */
	E2N(ENOTDIR)    /* Not a directory */
	E2N(EISDIR)    /* Is a directory */
	E2N(EINVAL)    /* Invalid argument */
	E2N(ENFILE)    /* File table overflow */
	E2N(EMFILE)    /* Too many open files */
	E2N(ENOTTY)    /* Not a typewriter */
	E2N(ETXTBSY)    /* Text file busy */
	E2N(EFBIG)    /* File too large */
	E2N(ENOSPC)    /* No space left on device */
	E2N(ESPIPE)    /* Illegal seek */
	E2N(EROFS)    /* Read-only file system */
	E2N(EMLINK)    /* Too many links */
	E2N(EPIPE)    /* Broken pipe */
	E2N(EDOM)    /* Math argument out of domain of func */
	E2N(ERANGE)    /* Math result not representable */

	E2N(EDEADLK)    /* Resource deadlock would occur */
	E2N(ENAMETOOLONG)    /* File name too long */
	E2N(ENOLCK)    /* No record locks available */
	E2N(ENOSYS)    /* Function not implemented */
	E2N(ENOTEMPTY)    /* Directory not empty */
	E2N(ELOOP)    /* Too many symbolic links encountered */
	E2N(EAGAIN)    /* Operation would block */
	E2N(ENOMSG)    /* No message of desired type */
	E2N(EIDRM)    /* Identifier removed */
	E2N(ECHRNG)    /* Channel number out of range */
	E2N(EL2NSYNC)    /* Level 2 not synchronized */
	E2N(EL3HLT)    /* Level 3 halted */
	E2N(EL3RST)    /* Level 3 reset */
	E2N(ELNRNG)    /* Link number out of range */
	E2N(EUNATCH)    /* Protocol driver not attached */
	E2N(ENOCSI)    /* No CSI structure available */
	E2N(EL2HLT)    /* Level 2 halted */
	E2N(EBADE)    /* Invalid exchange */
	E2N(EBADR)    /* Invalid request descriptor */
	E2N(EXFULL)    /* Exchange full */
	E2N(ENOANO)    /* No anode */
	E2N(EBADRQC)    /* Invalid request code */
	E2N(EBADSLT)    /* Invalid slot */

	E2N(EDEADLOCK)

	E2N(EBFONT)    /* Bad font file format */
	E2N(ENOSTR)    /* Device not a stream */
	E2N(ENODATA)    /* No data available */
	E2N(ETIME)    /* Timer expired */
	E2N(ENOSR)    /* Out of streams resources */
	E2N(ENONET)    /* Machine is not on the network */
	E2N(ENOPKG)    /* Package not installed */
	E2N(EREMOTE)    /* Object is remote */
	E2N(ENOLINK)    /* Link has been severed */
	E2N(EADV)    /* Advertise error */
	E2N(ESRMNT)    /* Srmount error */
	E2N(ECOMM)    /* Communication error on send */
	E2N(EPROTO)    /* Protocol error */
	E2N(EMULTIHOP)    /* Multihop attempted */
	E2N(EDOTDOT)    /* RFS specific error */
	E2N(EBADMSG)    /* Not a data message */
	E2N(EOVERFLOW)    /* Value too large for defined data type */
	E2N(ENOTUNIQ)    /* Name not unique on network */
	E2N(EBADFD)    /* File descriptor in bad state */
	E2N(EREMCHG)    /* Remote address changed */
	E2N(ELIBACC)    /* Can not access a needed shared library */
	E2N(ELIBBAD)    /* Accessing a corrupted shared library */
	E2N(ELIBSCN)    /* .lib section in a.out corrupted */
	E2N(ELIBMAX)    /* Attempting to link in too many shared libraries */
	E2N(ELIBEXEC)    /* Cannot exec a shared library directly */
	E2N(EILSEQ)    /* Illegal byte sequence */
	E2N(ERESTART)    /* Interrupted system call should be restarted */
	E2N(ESTRPIPE)    /* Streams pipe error */
	E2N(EUSERS)    /* Too many users */
	E2N(ENOTSOCK)    /* Socket operation on non-socket */
	E2N(EDESTADDRREQ)    /* Destination address required */
	E2N(EMSGSIZE)    /* Message too long */
	E2N(EPROTOTYPE)    /* Protocol wrong type for socket */
	E2N(ENOPROTOOPT)    /* Protocol not available */
	E2N(EPROTONOSUPPORT)    /* Protocol not supported */
	E2N(ESOCKTNOSUPPORT)    /* Socket type not supported */
	E2N(EOPNOTSUPP)    /* Operation not supported on transport endpoint */
	E2N(EPFNOSUPPORT)    /* Protocol family not supported */
	E2N(EAFNOSUPPORT)    /* Address family not supported by protocol */
	E2N(EADDRINUSE)    /* Address already in use */
	E2N(EADDRNOTAVAIL)    /* Cannot assign requested address */
	E2N(ENETDOWN)    /* Network is down */
	E2N(ENETUNREACH)    /* Network is unreachable */
	E2N(ENETRESET)    /* Network dropped connection because of reset */
	E2N(ECONNABORTED)    /* Software caused connection abort */
	E2N(ECONNRESET)    /* Connection reset by peer */
	E2N(ENOBUFS)    /* No buffer space available */
	E2N(EISCONN)    /* Transport endpoint is already connected */
	E2N(ENOTCONN)    /* Transport endpoint is not connected */
	E2N(ESHUTDOWN)    /* Cannot send after transport endpoint shutdown */
	E2N(ETOOMANYREFS)    /* Too many references: cannot splice */
	E2N(ETIMEDOUT)    /* Connection timed out */
	E2N(ECONNREFUSED)    /* Connection refused */
	E2N(EHOSTDOWN)    /* Host is down */
	E2N(EHOSTUNREACH)    /* No route to host */
	E2N(EALREADY)    /* Operation already in progress */
	E2N(EINPROGRESS)    /* Operation now in progress */
	E2N(ESTALE)    /* Stale NFS file handle */
	E2N(EUCLEAN)    /* Structure needs cleaning */
	E2N(ENOTNAM)    /* Not a XENIX named type file */
	E2N(ENAVAIL)    /* No XENIX semaphores available */
	E2N(EISNAM)    /* Is a named type file */
	E2N(EREMOTEIO)    /* Remote I/O error */
	E2N(EDQUOT)    /* Quota exceeded */

	E2N(ENOMEDIUM)    /* No medium found */
	E2N(EMEDIUMTYPE)    /* Wrong medium type */
	E2N(ECANCELED)    /* Operation Canceled */
	E2N(ENOKEY)    /* Required key not available */
	E2N(EKEYEXPIRED)    /* Key has expired */
	E2N(EKEYREVOKED)    /* Key has been revoked */
	E2N(EKEYREJECTED)    /* Key was rejected by service */

	/* for robust mutexes */
	E2N(EOWNERDEAD)    /* Owner died */
	E2N(ENOTRECOVERABLE)    /* State not recoverable */		
};

const char *errno2name(int errnum)
{
	int names = sizeof(s_map) / sizeof(s_map[0]);
	for (int i = 0; i < names; i++)
	{
		if (s_map[i].errnum == errnum)
			return s_map[i].errname;
	}
	
	return "UnknownErrno";
}


#endif // ERRNO2NAME_IMPL
#endif
