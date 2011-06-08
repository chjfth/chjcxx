#ifndef __mm_psfunc_20061002_h_
#define __mm_psfunc_20061002_h_

#ifdef __cplusplus
extern"C"{
#endif


int mmps_i64_type_prefix(char *szPrefix);
/*!<
	The implementer of this function returns in \e szfmt the type prefix 
 for formatting a 64-bit integer. For example:
	- For Microsoft Visual C++: "I64"
	- For GNU glibc: "ll"

  @return Returns the length of the type prefix string stored in \e szfmt,
	in characters.
*/

#ifdef __cplusplus
}//extern"C"{
#endif

#endif
