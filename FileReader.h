//
//  FileReader.hpp
//  mp4Parser
//
//  Created by hekun on 2021/9/11.
//

#ifndef FileReader_hpp
#define FileReader_hpp

#include <stdio.h>
#include <stdlib.h>

class FileReader {
public:
  FileReader();
  ~FileReader();
  
  int Open(const char* input);
  int Close();
  
  uint64_t GetFileSize();
  uint64_t GetCurPos();
  int SetCurPos(int pos);
  
  uint8_t  Read8();
  uint16_t Read16();
  uint32_t Read24();
  uint32_t Read32();
  uint64_t Read64();
  
  int ReadBuf(uint8_t* buf, int len);
  int Skip(int len);
private:
  FILE* m_fp;
};

#endif /* FileReader_hpp */
