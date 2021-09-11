//
//  Fourcc.h
//  mp4Parser
//
//  Created by hekun on 2021/9/11.
//

#ifndef Fourcc_h
#define Fourcc_h

#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

#define MP4_FTYP MKTAG('f', 't', 'y', 'p')
#define MP4_FREE MKTAG('f', 'r', 'e', 'e')
#define MP4_MOOV MKTAG('m', 'o', 'o', 'v')
#define MP4_MVHD MKTAG('m', 'v', 'h', 'd')
#define MP4_TRAK MKTAG('t', 'r', 'a', 'k')
#define MP4_TKHD MKTAG('t', 'k', 'h', 'd')
#define MP4_TREF MKTAG('t', 'r', 'e', 'f')//非必须
#define MP4_TRGR MKTAG('t', 'r', 'g', 'r')
#define MP4_EDTS MKTAG('e', 'd', 't', 's')
#define MP4_ELTS MKTAG('e', 'l', 's', 't')
#define MP4_MDIA MKTAG('m', 'd', 'i', 'a')

//#define MP4_

#endif /* Fourcc_h */
