#ifndef __CHHI__utf8utils_h_
#define __CHHI__utf8utils_h_
#define __CHHI__utf8utils_h_created_ 20260418
#define __CHHI__utf8utils_h_updated_ 20260418


////////////////////////////////////////////////////////////////////////////
namespace utf8 { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


int find_invalid_utf8seq();


////////////////////////////////////////////////////////////////////////////
} // namespace utf8
////////////////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++


#if defined(utf8utils_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_utf8utils) // [IMPL]


// >>> Include headers required by this lib's implementation

//#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <stdint.h> // for uint16_t etc

// <<< Include headers required by this lib's implementation




#ifndef utf8utils_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace utf8 {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.

/* 
// find_invalid_utf8seq(), produced by DeekSeek.

This function validates UTF-8 sequences according to the UTF-8 standard, checking for:

Valid sequence lengths: Determines if the leading byte correctly indicates 1-4 byte sequences

Continuation bytes: Ensures all continuation bytes have the 10xxxxxx pattern

Buffer boundaries: Prevents reading past the buffer end

Overlong sequences: Detects sequences that could be encoded in fewer bytes

Surrogate halves: Invalid in UTF-8 (U+D800 to U+DFFF)

Code point range: Ensures code points don't exceed the Unicode maximum (0x10FFFF)

The function returns the byte offset (0-based) of the first invalid UTF-8 sequence, 
or -1 if the entire buffer contains valid UTF-8 sequences.

*/

/**
* Find the first invalid UTF-8 sequence in a buffer
* @param buffer The input buffer to check
* @param length The length of the buffer in bytes
* @return Offset of first invalid sequence, or -1 if all sequences are valid
*/
__int64 find_invalid_utf8seq(const char* buffer, size_t length) 
{
	if (buffer == NULL || length == 0) {
		return -1;
	}

	size_t offset = 0;
	while (offset < length) 
	{
		unsigned char c = (unsigned char)buffer[offset];

		// ASCII character (0x00-0x7F)
		if (c <= 0x7F) {
			offset++;
			continue;
		}

		// Determine expected length of UTF-8 sequence
		int expected_len = 0;
		if ((c & 0xE0) == 0xC0) {
			// 2-byte character: 110xxxxx
			expected_len = 2;
		}
		else if ((c & 0xF0) == 0xE0) {
			// 3-byte character: 1110xxxx
			expected_len = 3;
		}
		else if ((c & 0xF8) == 0xF0) {
			// 4-byte character: 11110xxx
			expected_len = 4;
		}
		else {
			// Invalid leading byte
			return (__int64)offset;
		}

		// Check if we have enough bytes remaining
		if (offset + expected_len > length) {
			return (__int64)offset;
		}

		// Validate continuation bytes
		for (int i = 1; i < expected_len; i++) {
			unsigned char cont = (unsigned char)buffer[offset + i];
			// Continuation bytes must be 10xxxxxx
			if ((cont & 0xC0) != 0x80) {
				return (__int64)offset;
			}
		}

		// Additional validation for overlong sequences and invalid code points
		if (expected_len == 2) {
			// Check for overlong 2-byte sequence (should be at least 0x80)
			unsigned char byte1 = c;
			unsigned char byte2 = (unsigned char)buffer[offset + 1];
			uint16_t code_point = ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
			if (code_point < 0x80) {
				return (__int64)offset; // Overlong sequence
			}
		}
		else if (expected_len == 3) {
			// Check for overlong 3-byte sequence
			unsigned char byte1 = c;
			unsigned char byte2 = (unsigned char)buffer[offset + 1];
			unsigned char byte3 = (unsigned char)buffer[offset + 2];
			uint16_t code_point = ((byte1 & 0x0F) << 12) |
				((byte2 & 0x3F) << 6) |
				(byte3 & 0x3F);

			// Check for overlong (should be at least 0x800)
			if (code_point < 0x800) {
				return (__int64)offset;
			}

			// Check for surrogate halves (U+D800 to U+DFFF) which are invalid in UTF-8
			if (code_point >= 0xD800 && code_point <= 0xDFFF) {
				return (__int64)offset;
			}
		}
		else if (expected_len == 4) {
			// Check for overlong 4-byte sequence
			unsigned char byte1 = c;
			unsigned char byte2 = (unsigned char)buffer[offset + 1];
			unsigned char byte3 = (unsigned char)buffer[offset + 2];
			unsigned char byte4 = (unsigned char)buffer[offset + 3];
			uint32_t code_point = ((byte1 & 0x07) << 18) |
				((byte2 & 0x3F) << 12) |
				((byte3 & 0x3F) << 6) |
				(byte4 & 0x3F);

			// Check for overlong (should be at least 0x10000)
			if (code_point < 0x10000) {
				return (__int64)offset;
			}

			// Check for code points beyond Unicode maximum (0x10FFFF)
			if (code_point > 0x10FFFF) {
				return (__int64)offset;
			}
		}

		offset += expected_len;

	} // while (offset < length) 

	return -1;
}



////////////////////////////////////////////////////////////////////////////
} // namespace utf8
////////////////////////////////////////////////////////////////////////////



#ifndef utf8utils_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
