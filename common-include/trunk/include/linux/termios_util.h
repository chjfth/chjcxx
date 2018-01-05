#ifndef __termios_util_h_20171231_
#define __termios_util_h_20171231_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include <commdefs.h>
//#include <linux/errno2name.h>

#ifdef __cplusplus
extern"C" {
#endif

enum RawTermError_ret
{
	RTE_Success = 0,
	RTE_Unknown = -1, // maybe termios save but verify fail
	RTE_BadParam = -2,
	RTE_NotTTY = -3,
	RTE_SetLock = -4,
	RTE_SetBaudrate = -5,
	RTE_SetRaw = -6,
};
	
RawTermError_ret termios_SetRawTerm(int fd, int Baudrate, termios *pTioOrig=NULL);

const char *RTE2Name(RawTermError_ret val);
		
	
#ifdef __cplusplus
} //extern"C"{
#endif

///////////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION CODE BELOW ////////////////////////
///////////////////////////////////////////////////////////////////////////

#ifdef termios_util_IMPL

#include <sys/ioctl.h> // for TIOCEXCL

struct RTE_E2N_map_st
{
	int errnum;
	const char *errname;
};

#define RTE_E2N(e) { e, #e },

const char *
RTE2Name(RawTermError_ret errnum)
{
	static RTE_E2N_map_st s_map [] =
	{
		RTE_E2N(RTE_Success)
		RTE_E2N(RTE_Unknown)
		RTE_E2N(RTE_BadParam)
		RTE_E2N(RTE_NotTTY)
		RTE_E2N(RTE_SetLock)
		RTE_E2N(RTE_SetBaudrate)
		RTE_E2N(RTE_SetRaw)
	};

	int names = sizeof(s_map) / sizeof(s_map[0]);
	for (int i = 0; i < names; i++)
	{
		if (s_map[i].errnum == errnum)
			return s_map[i].errname;
	}
	
	return "UnknownRTE";
}

static bool  
tio_SetRaw(int fd)
{
	termios tioa = {};
	int err = tcgetattr(fd, &tioa);
	if (err)
		return false;

	// Set non-canonical(raw) mode 
	tioa.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Set raw input and output

	tioa.c_cflag |= CLOCAL | CREAD;
	tioa.c_cflag &= ~CRTSCTS; // Disable hardware flow control
	
	tioa.c_iflag |= IGNBRK; // ignore break condition (-BRKINT)
	tioa.c_iflag &= ~(IXON | IXOFF); // Disable software flow control (-IXANY)
	tioa.c_iflag |= INPCK; // enable parity checking
	tioa.c_iflag &= ~(IGNPAR | PARMRK); //Read parity error byte as 0x00
#ifdef IUCLC
	tioa.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IUCLC);
#else // MAC OS X 10.4/10.5 don't have IUCLC
	tioa.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR);
#endif

	tioa.c_oflag &= ~OPOST; // Disable all output processing
		// OPOST will cause an LF to become CR LF .
	
	err = tcsetattr(fd, TCSANOW, &tioa);
	if (err)
		return false;

	// Check whether my termios are really set
	termios tio_verify = {};
	err = tcgetattr(fd, &tio_verify);
	if ( err || memcmp(&tioa, &tio_verify, sizeof(termios))!=0 )
		return false;
	
	return true;
}


struct SBaudrateMap
{
	int baudrate;
	int bconst;
};
#define DEF_BAUD(n) { n, B##n }
static SBaudrateMap s_arBmap[] = 
{
	DEF_BAUD(921600),
	DEF_BAUD(460800),
	DEF_BAUD(230400),
	DEF_BAUD(115200),
	DEF_BAUD(57600),
	DEF_BAUD(38400),
	DEF_BAUD(19200),
	DEF_BAUD(9600),
	DEF_BAUD(4800),
	DEF_BAUD(2400),
	DEF_BAUD(1800),
	DEF_BAUD(1200),
	DEF_BAUD(600),
	DEF_BAUD(300),
	DEF_BAUD(200),
	DEF_BAUD(150),
	DEF_BAUD(75),
	DEF_BAUD(50),
};

static RawTermError_ret
tio_SetLineParams(int fd, int Baudrate, int *perrno)
{
	int i;
	termios tio = {}, tio_verify = {};
	int err = tcgetattr(fd, &tio);
	if (err)
		return RTE_Unknown;
	
	// Deal with Baudrate
	for(i=0; i<GetEleQuan_i(s_arBmap); i++)
	{
		if(s_arBmap[i].baudrate==Baudrate)
		{
			int baud_const = s_arBmap[i].bconst;
			err = cfsetispeed(&tio, baud_const);
			if (err)
				return RTE_SetBaudrate;

			err = cfsetospeed(&tio, baud_const);
			if(err)
				return RTE_SetBaudrate;

			break;
		}
	}
	//
	if(i==GetEleQuan(s_arBmap)) // no matching baudrate const found
		return RTE_SetBaudrate;

	err = tcsetattr(fd, TCSANOW, &tio);
	if (err)
	{
		*perrno = errno;
		return RTE_SetBaudrate;
	}		
	
	// Check whether the baudrate is really accepted by system. A MUST!
	// [2006-01-12] On PC, if you request to set baudrate 230400 in termios struct, 
	// but the hardware does not support it, then tcsetattr will succeed, but
	// subsequent tcgetattr shows that the baudrate 230400 was not set(remains the old baudrate).
	//
	err = tcgetattr(fd, &tio_verify);
	if( err || memcmp(&tio, &tio_verify, sizeof(termios))!=0 )
		return RTE_SetBaudrate; // not equal
	
	// Deal with DataBits, always set 8 bit
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CS8;

	// Deal with Parity, always set no-parity
	tio.c_cflag &= ~PARENB;

	// Deal with StopBits, always 1 stop bit
	tio.c_cflag &= ~CSTOPB;

	err = tcsetattr(fd, TCSANOW, &tio);
	if (err)
	{
		*perrno = errno;
		return RTE_Unknown;
	}

	// Verify again.
	err = tcgetattr(fd, &tio_verify);
	if( err || memcmp(&tio, &tio_verify, sizeof(termios))!=0 ) 
		return RTE_SetBaudrate; // not equal
	
	return RTE_Success;
}

RawTermError_ret 
termios_SetRawTerm(int fd, int Baudrate, termios *pTioOrig)
{
	RawTermError_ret ret = RTE_Unknown;
	int ret_errno = 0;
	bool succ = false;
	termios tio_orig = {};
	if (!pTioOrig)
		pTioOrig = &tio_orig;
	memset(pTioOrig, 0, sizeof(termios));
	
	if (!isatty(fd))
		return RTE_NotTTY;
	
	int err = tcgetattr(fd, pTioOrig);
	if (err)
		return RTE_Unknown;
	
	///
	
	// Set exclusive lock, but no "exclusion" effect on root user.
	// https://stackoverflow.com/a/17983721/151453 
	err = ioctl(fd, TIOCEXCL);
	if (err)
	{
		ret = RTE_SetLock;
		goto ERROR_END;
	}
	
	succ = tio_SetRaw(fd);
	if (!succ)
	{
		ret = RTE_SetRaw;
		goto ERROR_END;
	}

	ret = tio_SetLineParams(fd, Baudrate, &ret_errno);
	if (ret != RTE_Success)
		goto ERROR_END;
	
	return RTE_Success;
	
ERROR_END:
	// restore original termios params
	err = tcsetattr(fd, TCSANOW, &tio_orig);
	return ret;
}

#endif // termios_util_IMPL

#endif
