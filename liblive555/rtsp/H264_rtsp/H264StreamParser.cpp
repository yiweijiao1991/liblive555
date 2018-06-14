#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "H264StreamParser.hh"
#include "MyH264VideoStreamFramer.hh"
#include "assert.h"
#include "../../typedef.h"
#include "../../VideoQueue/VTInterf.h"


#define USR_DEFINED_ERROR			2




#ifdef __cplusplus
extern "C"
{
#endif
int rtsp_getFrame(frame_info * info);
#ifdef __cplusplus
}
#endif


//+++base64encode+++
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static void base64encode(char *out,const unsigned char* in,int len)
{
	for(;len >= 3; len -= 3)
	{
		*out ++ = cb64[ in[0]>>2];
		*out ++ = cb64[ ((in[0]&0x03)<<4)|(in[1]>>4) ];
		*out ++ = cb64[ ((in[1]&0x0f) <<2)|((in[2] & 0xc0)>>6) ];
		*out ++ = cb64[ in[2]&0x3f];
		in++;
	}
	if(len > 0)
	{
		unsigned char fragment;
		*out ++ = cb64[ in[0]>>2];
		fragment =	(in[0] &0x03)<<4 ;
		if(len > 1)
			fragment |= in[1] >>4 ;
		*out ++ = cb64[ fragment ];
		*out ++ = (len <2) ? '=' : cb64[ ((in[1]&0x0f)<<2)];
		*out ++ = '=';
	}
	*out = '\0';
}
//---base64encode---

#if 0
static void findNalu (NALU * nalus, u_int32_t * count, u_int8_t *bitstream, u_int32_t streamSize)
{
    u_int32_t index = *count;
    u_int8_t * bs = bitstream;
    u_int32_t  head;
    while ((u_int32_t)bs <= ((u_int32_t)bitstream + streamSize - 4))
    {
        head = (bs[3] << 24) | (bs[2] << 16) | (bs[1] << 8) | bs[0];
//	printf("head 0x%x: 0x%x, 0x%x, 0x%x, 0x%x\n", head, bs[0], bs[1], bs[2], bs[3]);
#ifdef BIGENDIAN
        if (head == 0x00000001)
#else
        if (head == 0x01000000)
#endif
        {
      		// we find a nalu
		bs += 4;
		nalus[index].addr = bs;
		nalus[index].nalu_type = 0x1F & bs[0];

		if ((index - (*count)) > 0)/*Not the first NALU in this stream*/
		{
		    nalus[index - 1].size = (u_int32_t)(nalus[index].addr - nalus[index - 1].addr) - 4;	// cut off 4 bytes of delimiter
		}
//		printf("	nalu type %d\n", nalus[index].nalu_type);	//jay
		++ index;
        }
        else if (bs[3] != 0) {
	        bs += 4;
        } else if (bs[2] != 0) {
	        bs += 3;
        } else if (bs[1] != 0) {
	        bs += 2;
        } else {
	        bs += 1;
        }
    }
    /*Calculate the size of the last NALU in this stream*/
    if (index > *count)
    {
        nalus[index - 1].size = ((u_int32_t)bitstream + streamSize) - (u_int32_t)nalus[index - 1].addr;
        *count = index;
    }
    else
        printf ("Nothing found!\n");
}
#endif 

static int findNalu_amba (NALU * nalus, u_int32_t * count, u_int8_t *bitstream, u_int32_t streamSize)
{
	int index = -1;
	u_int8_t * bs = bitstream;
	u_int32_t head;
	u_int8_t nalu_type;

	u_int8_t *last_byte_bitstream = bitstream + streamSize - 1;
	
	while (bs <= last_byte_bitstream) 
	{
		
		head = (bs[3] << 24) | (bs[2] << 16) | (bs[1] << 8) | bs[0];
//		printf("head 0x%x: 0x%x, 0x%x, 0x%x, 0x%x\n", head, bs[0], bs[1], bs[2], bs[3]);
		
		if (head == 0x01000000) 
		{	// little ending
			index++;
			// we find a nalu
			bs += 4;		// jump to nalu type
			nalu_type = 0x1F & bs[0];
			nalus[index].nalu_type = nalu_type;
			nalus[index].addr = bs;

			if (index  > 0) 
			{	// Not the first NALU in this stream
				nalus[index -1].size = nalus[index].addr - nalus[index -1].addr - 4; // cut off 4 bytes of delimiter
			}
//			printf("	====================nalu type %d\n", nalus[index].nalu_type);	//jay
//			printf(" ------------- %d, %d %d", bs[0], bs[1], bs[2]);
			if (nalu_type == NALU_TYPE_NONIDR || nalu_type == NALU_TYPE_IDR) 
			{
				// Calculate the size of the last NALU in this stream
				nalus[index].size =  last_byte_bitstream - bs + 1;
				break;
			}
		}
		else if (bs[3] != 0) 
		{
			bs += 4;
		} 
		else if (bs[2] != 0) 
		{
			bs += 3;
		} 
		else if (bs[1] != 0) 
		{
			bs += 2;
		} 
		else 
		{
			bs += 1;
		}
	}

	*count = index + 1;
	if (*count == 0) 
	{
		printf("No nalu found in the bitstream!\n");
		return -1;
	}
	//printf("------------------------------------\n");
	return 0;
}

#if 0
static int show_streaming(void)
{
	#define DOT_MAX_COUNT 10
	static int dot_count = DOT_MAX_COUNT;
	int i;

	if (dot_count < DOT_MAX_COUNT) {
		fprintf(stderr, ".");	//print a dot to indicate it's alive
		dot_count++;
	} else{
		fprintf(stderr, "\r");
		for ( i = 0; i < DOT_MAX_COUNT ; i++)
			fprintf(stderr, " ");
		fprintf(stderr, "\r");
		dot_count = 0;
	}

	fflush(stderr);
	return 0;
}
#endif

//H264FileStreamParser


H264BitStreamParser::H264BitStreamParser(int streamID)
	:fsps(NULL),fpps(NULL), profileLevelID(0), fNaluCurrIndex(0), fNaluCount(0), 
	fStreamID(streamID), fFrameNum(0)
{
	return ;
}

H264BitStreamParser::~H264BitStreamParser()
{
//	if(m_data != NULL)
//		delete[] m_data;
	printf(" ------------H264BitStreamParser::~H264BitStreamParser  free----------------------\n");
	delete[] fsps;
	delete[] fpps;
}

void H264BitStreamParser:: registerReadInterest(unsigned char* to,unsigned maxSize)
{
	fTo = to;
	fLimit = to + maxSize;
}

extern struct timeval timeNow, timeNow2;
int frame_num = 0;

int H264BitStreamParser::parse()
{

	
	frame_info frame;
	
	while (fNaluCurrIndex >= fNaluCount) 
	{ 	
		 rtsp_getFrame(&frame);
		
		
#if 1	
		fFrameNum = frame.iFrameNub;
#endif
	//	printf("  %d   %x %x %x %x %x\n" , frame.length, frame.pData[0], frame.pData[1],frame.pData[2], frame.pData[3], frame.pData[4]);
		findNalu_amba(fNalus, (u_int32_t* )&fNaluCount, (u_int8_t*)(frame.pData), frame.length);
		fNaluCurrIndex = 0;
	}

	while(fNalus[fNaluCurrIndex].nalu_type == 16) 
	{		//filter out the SEI nalu
		assert(fNaluCurrIndex <= fNaluCount);
		fNaluCurrIndex++;
	}
//	printf("[%d] ------nalu_type %d, size %d, PTS %d\n", fStreamID, 
//		fNalus[fNaluCurrIndex].nalu_type, fNalus[fNaluCurrIndex].size, fPTS);		//jay
	//assert((fNaluSize = fNalus[fNaluCurrIndex].size) > 0);
	if((fNaluSize = fNalus[fNaluCurrIndex].size) <= 0)
	{
		fNaluCurrIndex++;
		return 0;
	}
	fNaluType = fNalus[fNaluCurrIndex].nalu_type;
	 
	
	if (fTo != NULL) {
		memcpy(fTo, fNalus[fNaluCurrIndex].addr, fNalus[fNaluCurrIndex].size);
#if 0		//optm
		if (fNaluType == 7) {
			sps = fTo + 1;
			if (fsps != NULL)
				delete[] fsps;
			fsps = new char[fNaluSize *4/3 +4];
			base64encode(fsps,sps, fNaluSize-1);
			memcpy(((char*)&profileLevelID),sps,1);
			memcpy(((char*)&profileLevelID)+1,sps+1,1);
			memcpy(((char*)&profileLevelID)+2,sps+2,1);
		}
		if (fNaluType == 8) {
			pps = fTo + 1;
			if (fpps != NULL)
				delete[] fpps;
			fpps = new char[fNaluSize*4/3 +4];
			base64encode(fpps,pps,fNaluSize-1);
		}
#endif 
		fNaluCurrIndex++;
	}
	return 0;
}

char* H264BitStreamParser::getParsersps()
{
	return fsps;
}

char* H264BitStreamParser::getParserpps()
{
	return fpps;
}
unsigned int H264BitStreamParser::getPreID()
{
	return profileLevelID;
}


//H264FileStreamParser
H264FileStreamParser::H264FileStreamParser(FramedSource * usingSource, FramedSource * inputSource)
:StreamParser(inputSource, FramedSource::handleClosure, usingSource, &MyH264VideoStreamFramer::continueReadProcessing, usingSource),
fUsingSource(usingSource),fsps(NULL),fpps(NULL), profileLevelID(0), fCurrentParseState(PARSING_START_SEQUENCE)
{
	
}

H264FileStreamParser::~H264FileStreamParser()
{
	delete[] fsps;
	delete[] fpps;
}

//reset saved the parser state
void H264FileStreamParser::restoreSavedParserState()
{
	StreamParser::restoreSavedParserState();
	fTo = fSavedTo;
	fNumTruncatedBytes = fSavedNumTruncatedBytes;
}

void H264FileStreamParser::setParseState(MyH264ParseState parseState)
{
	fSavedTo = fTo;
	fSavedNumTruncatedBytes = fNumTruncatedBytes;
	fCurrentParseState = parseState;
	saveParserState();
}

unsigned H264FileStreamParser::getParseState()
{
	return fCurrentParseState;
}

void H264FileStreamParser:: registerReadInterest(unsigned char* to,unsigned maxSize)
{
	fStartOfFrame = fTo = fSavedTo = to;
	fLimit = to + maxSize;
	fNumTruncatedBytes = fSavedNumTruncatedBytes = 0;
}

unsigned H264FileStreamParser::parse()
{
	while (1) {
		try {
			switch(fCurrentParseState)
			{
				case PARSING_START_SEQUENCE:
	//				printf("parseStartSequence\n");	//jay
					return parseStartSequence();
				case PARSING_NAL_UNIT:
	//				printf("parseNALUnit\n");	//jay
					return parseNALUnit();
				default:
					return 0;
			}
		} catch (int e)
		{
//			printf("catch %d!\n", e);		//jay
			if (e == USR_DEFINED_ERROR)
				continue;
			else
				return 0;
		}
	}
}

unsigned H264FileStreamParser::parseStartSequence()
{
//get 4 bytes
	u_int32_t test = test4Bytes();
//find the startcode
	while(test != 0x00000001)
	{
		skipBytes(1);
		test = test4Bytes();
	}
//set the state to PARSING_START_SEQUNCE
	setParseState(PARSING_START_SEQUENCE);
	return parseNALUnit();
}

unsigned H264FileStreamParser::parseNALUnit()
{
	unsigned char* sps = NULL;
	unsigned char* pps = NULL;
	skipBytes(1);
	u_int32_t head= test4Bytes();
	skipBytes(3);
	u_int8_t Type = 0;

	u_int32_t test = test4Bytes();
	switch(head&0xffffff1f)
	{
		case SPSHEAD:
	//		printf("SPS\n");		//jay
			Type = 7;
			sps = fTo + 1;
			break;
		case PPSHEAD:
	//		printf("PPS\n");		//jay
			Type = 8;
			pps = fTo + 1;
			break;
		case IDRHEAD:
	//		printf("IDR Frame\n");		//jay
			Type = 5;
			break;
		case NALUHEAD:
	//		printf("IP Frame\n");		//jay
			Type = 1;
			break;
		case AUDHEAD:
	//		printf("AUD\n");		//jay
			Type = 9;
	//		throw USR_DEFINED_ERROR;
			break;
		case SEIHEAD:
	//		printf("SEI\n");		//jay
			Type = 6;
	//		throw USR_DEFINED_ERROR;
			break;
		default:
			Type = 0;
			break;
	}

	while(test != 0x00000001)
	{
		saveByte(get1Byte());
		test = test4Bytes();
	}
	
	if(Type == 7)
	{
		if (fsps != NULL)
			delete[] fsps;
		fsps = new char[curFrameSize()*4/3 +4];
		base64encode(fsps,sps,curFrameSize()-1);
		memcpy(((char*)&profileLevelID),sps,1);
		memcpy(((char*)&profileLevelID)+1,sps+1,1);
		memcpy(((char*)&profileLevelID)+2,sps+2,1);
	}
	else if(Type == 8)
	{
		if (fpps != NULL)
			delete[] fpps;
		fpps = new char[curFrameSize()*4/3 +4];
		base64encode(fpps,pps,curFrameSize()-1);
	}
//	printf("curFrameSize %d\n", curFrameSize());
	fNaluType = Type;
	return curFrameSize();
}

void H264FileStreamParser::saveByte(u_int8_t sbyte)
{
//	printf("saveByte 0x%x\n", byte);	//jay
	if(fTo >= fLimit)
	{
		++fNumTruncatedBytes;
		return;
	}
	*fTo++ = sbyte;
}

void H264FileStreamParser::save4Bytes(u_int32_t word)
{
	if(fTo+4 > fLimit)
	{
		fNumTruncatedBytes += 4;
		return;
	}
	*fTo++ = (word>>24);
	*fTo++ = (word>>16);
	*fTo++ = (word>>8);
	*fTo++ = (word);
}

//save data until we see a sync word(0x00000001)
void H264FileStreamParser::saveToNextCode(u_int32_t& curWord)
{
	while(curWord != 0x00000001)
	{
		save4Bytes(curWord);
		curWord = get4Bytes();
	}
}

void H264FileStreamParser::skipToNextCode(u_int32_t& curWord)
{
	while(curWord != 0x00000001)
	{
		curWord = get4Bytes();
	}
}

unsigned H264FileStreamParser::curFrameSize()
{	
	return fTo - fStartOfFrame;
}
char* H264FileStreamParser::getParsersps()
{
	return fsps;
}

char* H264FileStreamParser::getParserpps()
{
	return fpps;
}
unsigned int H264FileStreamParser::getPreID()
{
	return profileLevelID;
}

