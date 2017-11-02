#include "stdafx.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <_MINMAX_.h>
#include <gadgetlib/timefuncs.h>
#include <gadgetlib/getch.h>

enum TermioErr_et
{
	ETIO_Succ = 0,
	ETIO_Unknown = -1,
	ETIO_CheckFail = -2,
};

static TermioErr_et // put terminal into a raw mode
tty_set_input_raw(int fd, const termios &tios_orig, int vmin_val, int vtime_val)
{
	int             err;
	struct termios  tios = tios_orig;

	// Turn OFF these *local* flags:
	tcflag_t local_flags =
		ICANON  // non-canonical mode
		| ECHO  // (human)input char does not get echo back (to human)
		| ISIG;  // Special input char will not generate signals like INTR, QUIT, SUSP.
	tios.c_lflag &= ~local_flags;
	// Chj: Not need to care about these canonical-mode flags:
	//	* ECHONL: echo the NL(new-line) character even if ECHO is not set.
	//	* IEXTEN: Enable implementation-defined input processing
	
	
	// Turn off these *input* flags:
	tcflag_t input_flags = 
		BRKINT   // No generating SIGINT on serial-line BREAK (hardware)signal.
		| INPCK  // No input parity checking
		| ISTRIP // No Strip off eighth bit
		| ICRNL  // No convertion CR to LF
		| IXON;  // No XON/XOFF flow control on output
	tios.c_iflag &= ~input_flags;
	// note: IGNBRK is determined by caller input, and I don't touch it here.
	// No need to care: PARMRK, IGNCR
	
	// Turn off these *control* flags:
	tcflag_t control_flags = 
		CSIZE     // Clear byte-size mask(couple with ``tios.c_cflag|=CS8;`` later)
		| PARENB; // No input parity check. Dup of INPCK, explicity mention it for safety.
	tios.c_cflag &= ~control_flags;
	tios.c_cflag |= CS8; // Set 8 bits/char.

	// Nothing to do with output processing, we care for input-raw here.
//	tios.c_oflag &= ~(OPOST);

	tios.c_cc[VMIN] = vmin_val;
	tios.c_cc[VTIME] = vtime_val;
	
	if (tcsetattr(fd, TCSANOW, &tios) < 0) // I don't want TCSAFLUSH
	{
//		printf("(tcsetattr(fd, TCSANOW, &buf) < 0)");
	
		return ETIO_Unknown;
	}

	// Verify that the changes stuck. tcsetattr can return 0 on partial success.
	//
	termios tios_chk = {};
	if (tcgetattr(fd, &tios_chk) < 0) 
	{
//		err = errno;
//		tcsetattr(fd, TCSAFLUSH, &save_termios);
//		errno = err;
//		printf("(tcgetattr(fd, &buf) < 0)");
		return ETIO_Unknown;
	}
	
	if (tios_chk.c_lflag & local_flags)
		return ETIO_CheckFail;
	if (tios.c_iflag & input_flags)
		return ETIO_CheckFail;
	if ( (tios.c_cflag & (control_flags | CS8)) != CS8 )
		return ETIO_CheckFail;
//	if (tios.c_oflag & OPOST)
//		return ETIO_CheckFail;
	if( tios.c_cc[VMIN]!=vmin_val || tios.c_cc[VTIME]!=vtime_val)
	{
		return ETIO_CheckFail;
	}

	return ETIO_Succ;
}

static int 
in_ggt_getch(const termios &tios_orig, int timeout_millisec)
{
	int fd = STDIN_FILENO;
	int nrd = 0;
	char c = 0;

	TermioErr_et terr = ETIO_Succ;

	if (timeout_millisec <= 0)
	{
		if (timeout_millisec < 0) // wait infinitely, blocking read
		{
			terr = tty_set_input_raw(fd, tios_orig, 1, 0);		
		}
		else if (timeout_millisec == ggt_getch_peek) // ==0, polling read
		{
			terr = tty_set_input_raw(fd, tios_orig, 0, 0);
		}
		
		if (terr)
			return -1;
		
		nrd = read(fd, &c, 1);
		if (nrd == 1)
			return c;
		else if (nrd == 0)
			return 0;
		else
		{
			assert(nrd < 0);
			return -1;
		}
	}
	else
	{
		__int64 msec_start = ggt_GetOsMillisec();
		__int64 msec_end = msec_start + timeout_millisec;
		int msec_remain = (int)(msec_end - msec_start);
		// break wait time into fractions bcz VTIME can only represent 25.5 seconds

		for(; ;)
		{
			int vtime = _MID_(1, msec_remain/100, 250);
			
			int vmin = 0; // cannot be 1, which would cause infinite wait
			terr = tty_set_input_raw(fd, tios_orig, vmin, vtime);
			if (terr)
				return -1;

			nrd = read(fd, &c, 1);
			if (nrd < 0)
				return -1;
			else if (nrd == 1)
				return c;

			// -- timeout, try again
			
			msec_remain = (int)(msec_end - ggt_GetOsMillisec());
			if (msec_remain <= 0)
				return 0;
		}
	}
}

int 
ggt_getch(int timeout_millisec)
{
	termios tios_orig = { };
	if (tcgetattr(STDIN_FILENO, &tios_orig) < 0)
		return -1;
	
	int chret = in_ggt_getch(tios_orig, timeout_millisec);
	
	tcsetattr(STDIN_FILENO, TCSANOW, &tios_orig);
	
	return chret;
}


/*
 *http://man7.org/linux/man-pages/man3/termios.3.html

In noncanonical mode input is available immediately (without the user
       having to type a line-delimiter character), no input processing is
       performed, and line editing is disabled.  The read buffer will only
       accept 4095 chars; this provides the necessary space for a newline
       char if the input mode is switched to canonical.  The settings of MIN
       (c_cc[VMIN]) and TIME (c_cc[VTIME]) determine the circumstances in
       which a read(2) completes; there are four distinct cases:

       MIN == 0, TIME == 0 (polling read)
              If data is available, read(2) returns immediately, with the
              lesser of the number of bytes available, or the number of
              bytes requested.  If no data is available, read(2) returns 0.

       MIN > 0, TIME == 0 (blocking read)
              read(2) blocks until MIN bytes are available, and returns up
              to the number of bytes requested.

       MIN == 0, TIME > 0 (read with timeout)
              TIME specifies the limit for a timer in tenths of a second.
              The timer is started when read(2) is called.  read(2) returns
              either when at least one byte of data is available, or when
              the timer expires.  If the timer expires without any input
              becoming available, read(2) returns 0.  If data is already
              available at the time of the call to read(2), the call behaves
              as though the data was received immediately after the call.

       MIN > 0, TIME > 0 (read with interbyte timeout)
              TIME specifies the limit for a timer in tenths of a second.
              Once an initial byte of input becomes available, the timer is
              restarted after each further byte is received.  read(2)
              returns when any of the following conditions is met:

              *  MIN bytes have been received.

              *  The interbyte timer expires.

              *  The number of bytes requested by read(2) has been received.
                 (POSIX does not specify this termination condition, and on
                 some other implementations read(2) does not return in this
                 case.)

              Because the timer is started only after the initial byte
              becomes available, at least one byte will be read.  If data is
              already available at the time of the call to read(2), the call
              behaves as though the data was received immediately after the
              call.
 
*/