//
//  FileReader.cpp
//  mp4Parser
//
//  Created by hekun on 2021/9/11.
//

#include "FileReader.h"

FileReader::FileReader() {
  m_fp = nullptr;
}

FileReader::~FileReader() {
  if(m_fp) {
    fclose(m_fp);
  }
  m_fp = nullptr;
}

int FileReader::Open(const char* input) {
  int nRet = 0;
  if(m_fp) {
    fclose(m_fp);
    m_fp = nullptr;
  }
  if((m_fp = fopen(input, "rb")) == nullptr) {
    nRet = -1;
  }
  return nRet;
}

int FileReader::Close() {
  if(m_fp) {
    fclose(m_fp);
  }
  m_fp = nullptr;
  return 0;
}

uint64_t FileReader::GetFileSize() {
  uint64_t ret = -1;
  if(m_fp) {
    uint64_t cur = ftell(m_fp);
    fseek(m_fp, 0, SEEK_END);
    ret = ftell(m_fp);
    fseek(m_fp, 0, cur);
  }
  return ret;
}

uint64_t FileReader::GetCurPos() {
  uint64_t ret = -1;
  if(m_fp) {
    ret = ftell(m_fp);
  }
  return ret;
}

int FileReader::SetCurPos(int pos) {
  int ret = -1;
  if(m_fp) {
    ret = fseek(m_fp, pos, SEEK_SET);
  }
  return ret;
}

uint8_t FileReader::Read8()
{
  uint8_t buf;
  fread(&buf, 1, 1, m_fp);
  return buf;
}

uint16_t FileReader::Read16()
{
  uint8_t buf[2];
  fread(buf, 1, 2, m_fp);
  return buf[1] | buf[0] << 8;
}

uint32_t FileReader::Read24()
{
  uint8_t buf[3];
  fread(buf, 1, 3, m_fp);
  return buf[2] | buf[1] << 8 | buf[0] << 16;
}

uint32_t FileReader::Read32()
{
  uint8_t buf[4];
  fread(buf, 1, 4, m_fp);
  return buf[3] | buf[2] << 8 | buf[1] << 16 | buf[0] << 24;
}

uint64_t FileReader::Read64()
{
  uint64_t high = Read32();
  uint64_t low = Read32();
  return high << 32 | low;
}

int FileReader::ReadBuf(uint8_t* buf, int len) {
  int ret = 0;
  if (m_fp) {
    ret = fread(buf, len, 1, m_fp);
  }
  return ret;
}

int FileReader::Skip(int len){
  int ret = 0;
  if(m_fp) {
    ret = fseek(m_fp, len, SEEK_CUR);
  }
  return ret;
}
