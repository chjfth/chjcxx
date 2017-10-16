#ifndef __logtofile_h_
#define __logtofile_h_

int logfile_create(const char *filename); // create a new file

void logfile_append(int hfile, const void *pcontent, int bytes);

void logfile_close(int hfile);

#endif
