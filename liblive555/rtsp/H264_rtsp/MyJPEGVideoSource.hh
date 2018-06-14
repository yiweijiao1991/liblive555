/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2010 Live Networks, Inc.  All rights reserved.
// JPEG video sources
// C++ header

#ifndef _MY_JPEG_VIDEO_SOURCE_HH
#define _MY_JPEG_VIDEO_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif


class MyJPEGVideoSource: public FramedSource {
public:
	//inherited
	void doGetNextFrame();
	typedef struct {
	int	stream_id;
	unsigned int	session_id;	//use 32-bit session id
	int	frame_num;
	int	pic_type;
	unsigned int	PTS;
	int  stream_end;  // 0: normal stream frames,  1: stream end null frame
} bs_info_t;


/* any frame stored in bsreader must has continuous memory */
typedef struct bsreader_frame_info_s {
	bs_info_t bs_info;	/* orignal information got from dsp */
	char *frame_addr; /* the actual address in stream buffer */
	int	frame_size;	 /* the actual output frame size , including CAVLC */
} bsreader_frame_info_t;
	virtual u_int8_t type();
	virtual u_int8_t qFactor();
	virtual u_int8_t width(); // # pixels/8 (or 0 for 2048 pixels)
	virtual u_int8_t height(); // # pixels/8 (or 0 for 2048 pixels)
	virtual u_int8_t const* quantizationTables(u_int8_t& precision,
					     u_int16_t& length);
	virtual u_int32_t fragmentOffset();
	virtual Boolean isFrameEnd();
	virtual void setMaxOutputPacketSize(unsigned OutputPacketSize);
	int bsreader_get_one_frame(int stream, bsreader_frame_info_t * info);
    // If "qFactor()" returns a value >= 128, then this function is called
    // to tell us the quantization tables that are being used.
    // (The default implementation of this function just returns NULL.)
    // "precision" and "length" are as defined in RFC 2435, section 3.1.8.
	static MyJPEGVideoSource* createNew(UsageEnvironment& env, int streamID, int Quality);
protected:
  MyJPEGVideoSource(UsageEnvironment& env, int streamID, int Quality); // abstract base class
  virtual ~MyJPEGVideoSource();

private:
  // redefined virtual functions:
  virtual Boolean isJPEGVideoSource() const;
  virtual Boolean Parse(unsigned char* begin_addr, u_int32_t size);

private:
  int fStreamID;
  unsigned fMaxOutputPacketSize;
  u_int32_t fPTS;
  u_int32_t fSessionID;
  u_int8_t fType;
  u_int8_t fQ;
  u_int8_t* fQuantizationTables;
  u_int8_t fPrecision;
  u_int16_t fQtableLength;
  u_int16_t fWidth;
  u_int16_t fHeight;
  u_int32_t fFragmentOffset;
  unsigned char* fFragmentBeginAddr;
  u_int32_t fFragmentLeftSize;
};

#endif
