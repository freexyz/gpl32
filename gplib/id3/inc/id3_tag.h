#ifndef __ID3_TAG_H__
#define __ID3_TAG_H__

#include "gplib.h"

//=============================================================================
// Definition
//=============================================================================
// +++++++++
// + ID3v1 +
// +++++++++
#define C_ID3_V1_ID				"TAG"
#define C_ID3_V1_ID_LEN		3			// Len of C_ID3_V1_ID
#define C_ID3_V1_TAG_LEN	128		// Whole len of ID3 tag v1

#define C_ID3_V1_FT_ID			0	// Offset from the beginning of v1 tag
#define C_ID3_V1_FT_TITLE		1
#define C_ID3_V1_FT_ARTIST	2
#define C_ID3_V1_FT_ALBUM		3
#define C_ID3_V1_FT_YEAR		4
#define C_ID3_V1_FT_COMMENT	5
#define C_ID3_V1_FT_TRACK		6
#define C_ID3_V1_FT_GENRE		7
#define C_ID3_V1_FT_MAX			C_ID3_V1_FT_GENRE
#define C_ID3_V1_FT_INVALID	0xFF

#define C_ID3_V1_OFFSET_ID			0	// Offset from the beginning of v1 tag
#define C_ID3_V1_OFFSET_TITLE		3
#define C_ID3_V1_OFFSET_ARTIST	33
#define C_ID3_V1_OFFSET_ALBUM		63
#define C_ID3_V1_OFFSET_YEAR		93
#define C_ID3_V1_OFFSET_COMMENT	97
#define C_ID3_V1_OFFSET_TRACK		126
#define C_ID3_V1_OFFSET_GENRE		127

#define C_ID3_V1_FILED_SIZE_ID			C_ID3_V1_ID_LEN
#define C_ID3_V1_FILED_SIZE_TITLE		30
#define C_ID3_V1_FILED_SIZE_ARTIST	30
#define C_ID3_V1_FILED_SIZE_ALBUM		30
#define C_ID3_V1_FILED_SIZE_YEAR		4
#define C_ID3_V1_FILED_SIZE_COMMENT	28	// ID3v1.1, not v1.0
#define C_ID3_V1_FILED_SIZE_TRACK		1		// Only in ID3v1.1
#define C_ID3_V1_FILED_SIZE_GENRE		1

// +++++++++
// + ID3v2 +
// +++++++++
#define C_ID3_V2_ID									"ID3"
#define C_ID3_V2_ID_LEN							3			// Len of C_ID3_V2_ID
#define C_ID3_V2_HEADER_LEN					10		// Total len of ID3v2 header
#define C_ID3_V2_EXTENDED_HDR_LEN		6			// Total len of ID3v2 extended hdr
#define C_ID3_V2_FOOTER_LEN					10		// Total len of ID3v2 footer
#define C_ID3_V2_INVALID_FRAME_ID		"ZZZZ"
#define C_ID3_V2_PADDING_FRAME_ID		"\0\0\0\0"

#define C_ID3_V2_HEADER_OFFSET_VERSION	3
#define C_ID3_V2_HEADER_OFFSET_FLAGS		5
#define C_ID3_V2_HEADER_OFFSET_SIZE			6

#define C_ID3_V2_FLAGS_FOOTER_PRESENT_MASK	0x10
#define C_ID3_V2_FLAGS_EXTENDED_HDR_MASK		0x40

#define C_ID3_V2_FRAME_HEADER_SIZE	10

#define C_ID3_V2_FOOTER_ID			"3DI"
#define C_ID3_V2_FOOTER_ID_LEN	3
#define C_ID3_V2_FOOTER_LEN			10

#endif