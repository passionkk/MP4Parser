#include <iostream>
#include <string>
#include <algorithm>
#include "FileReader.h"
#include "Fourcc.h"

using namespace std;

struct stBox {
  uint boxLen;
  uint64_t boxLargeLen;
  uint8_t boxType[4];
  uint8_t* boxData;
};

int ParseMoov(FileReader& reader, int nDataLen);
int ParseMvhd(FileReader& reader, int nDataLen);
int ParseTrak(FileReader& reader, int nDataLen);
int ParseTkhd(FileReader& reader, int nDataLen);

int main() {
  FileReader reader;
  cout << "this is a mp4 parser" << endl;
  std::string strInput = "../../../test.mp4";
  if(reader.Open(strInput.c_str()) == 0)
  {
    uint64_t fileSize = reader.GetFileSize();
    cout << "file size is " << fileSize << endl;
    reader.SetCurPos(0);
    uint64_t nBoxDatalen = 0;
    uint64_t nMaxDataSize = 400*1000;
    uint64_t nFileOffset = 0;
    stBox box;
    box.boxData = new uint8_t[nMaxDataSize];
    uint8_t boxType[4] = {0};
    while (nFileOffset < fileSize) {
      //size 一下把4字节长度和可能的largeSize都读取了以及type
      box.boxLen = reader.Read32();
      nFileOffset += 4;
      if(box.boxLen == 1) {
        box.boxLargeLen = reader.Read64();
        nFileOffset += 8;
      }
      //type
      reader.ReadBuf(box.boxType, 4);
      nFileOffset += 4;
      //data
      nBoxDatalen = std::max((uint64_t)box.boxLen, box.boxLargeLen) - 4 - (box.boxLen==1 ? 8 : 4);
      if(box.boxLen > nMaxDataSize || box.boxLargeLen > nMaxDataSize)
      {
        nMaxDataSize = std::max((uint64_t)box.boxLen, box.boxLargeLen);
        delete[] box.boxData;
        box.boxData = new uint8_t[nMaxDataSize];
      }
      if(MKTAG(box.boxType[0], box.boxType[1], box.boxType[2], box.boxType[3]) == MP4_MOOV) {
        ParseMoov(reader, nBoxDatalen);
      } else {
        reader.ReadBuf(box.boxData, nBoxDatalen);
        nFileOffset += nBoxDatalen;
      }
      printf("box type: %s, size:%lu, largeSize:%llu, dataLen:%llu\n", box.boxType, box.boxLen, box.boxLargeLen, nBoxDatalen);
      
#if 0 //暂时不显示
      for(int i = 0;i < nBoxDatalen; i++) {
        printf("%02x ", box.boxData[i]);
      }
#endif
    }
    printf("\n");
    reader.Close();
  } else {
    cout << strInput << " open fail" << endl;
  }
  return 0;
}

int ParseMoov(FileReader& reader, int nDataLen)
{
  int nStartPos = reader.GetCurPos();
  cout << "moov Header, size: " << nDataLen << endl;
  stBox box;
  while (reader.GetCurPos() - nStartPos < nDataLen) {
    box.boxLen = reader.Read32();
    reader.ReadBuf(box.boxType, 4);
    uint32_t subType = MKTAG(box.boxType[0], box.boxType[1], box.boxType[2], box.boxType[3]);
    switch (subType) {
      case MP4_MVHD:
        ParseMvhd(reader, box.boxLen);
        break;
      case MP4_TRAK:
        ParseTrak(reader, box.boxLen);
        break;
      case MP4_TKHD:
        ParseTkhd(reader, box.boxLen);
      default:
        break;
    }
  }
  return 0;
}

int ParseMvhd(FileReader& reader, int nDataLen) {
  int nStartPos = reader.GetCurPos();
  cout << "box type: mvhd, size:" << nDataLen << endl;
  //version unsigned int(8)
  //flags bit(24)
  int version = reader.Read8();
  int flag = reader.Read24();
  uint64_t creation_time, modification_time, duration;
  uint32_t timescale;
  if(version == 1){
    // uint64_t
    creation_time = reader.Read64();
    modification_time = reader.Read64();
    timescale = reader.Read32();
    duration = reader.Read64();
    cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
    ", timescale: " << timescale << ", duartion: " << duration << endl;
  } else {
    // uint32_t
    creation_time = reader.Read32();
    modification_time = reader.Read32();
    timescale = reader.Read32();
    duration = reader.Read32();
    cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
    ", timescale: " << timescale << ", duartion: " << duration << endl;
    cout << "duration as seconds: " << duration*1.0/timescale << endl;
  }
//  uint32_t rate = reader.Read32();    //typically 1.0
//  uint16_t volume = reader.Read16();  //typically, full volume
//  uint16_t reserved = reader.Read16();
  reader.SetCurPos(nStartPos+nDataLen);
  return 0;
}

int ParseTrak(FileReader& reader, int nDataLen) {
  int nStartPos = reader.GetCurPos();
  cout << "box type: trak, size:" << nDataLen << endl;
  stBox box;
  while (reader.GetCurPos() - nStartPos < nDataLen) {
    box.boxLen = reader.Read32();
    reader.ReadBuf(box.boxType, 4);
    uint32_t subType = MKTAG(box.boxType[0], box.boxType[1], box.boxType[2], box.boxType[3]);
    switch (subType) {
      case MP4_TKHD:
        ParseTkhd(reader, nDataLen);
        break;
      case MP4_TREF:
      case MP4_TRGR:
        reader.Skip(nDataLen);
        break;
      case MP4_EDTS:
        cout << "box type: edts, size:" << box.boxLen << endl;
        reader.Skip(nDataLen);
        //ParseEDTS();
        break;
      case MP4_MDIA:
        break;
      default:
        break;
    }
  }
  return 0;
}

int ParseTkhd(FileReader& reader, int nDataLen) {
  int nStartPos = reader.GetCurPos();
  cout << "box type: tkhd, size:" << nDataLen;
  //version unsigned int(8)
  //flags bit(24)
  int version = reader.Read8();
  int flag = reader.Read24();
  uint64_t creation_time, modification_time;
  uint32_t track_id, reserved, duration;  //reserved 保留字 0
  if(version == 1){
    // uint64_t
    creation_time = reader.Read64();
    modification_time = reader.Read64();
    track_id = reader.Read32();
    reserved = reader.Read32();
    duration = reader.Read32();
    cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
    ", track_id: " << track_id << ", duartion: " << duration << endl;
  } else {
    // uint32_t
    creation_time = reader.Read32();
    modification_time = reader.Read32();
    track_id = reader.Read32();
    reserved = reader.Read32();
    duration = reader.Read32();
    cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
    ", track_id: " << track_id << ", duartion: " << duration << endl;
  }
  
  reader.Skip(4*2);//const unsigned int(32)[2] reserved = 0;
  reader.Skip(2);//template int(16) layer = 0;
  reader.Skip(2);//template int(16) alternate_group = 0;
  reader.Skip(2);//template int(16) volume = {if track_is_audio 0x0100 else 0};
  reader.Skip(2);//const unsigned int(16) reserved = 0;
  reader.Skip(4*9);//template int(32)[9] matrix= { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
  // unity matrix

  uint32_t width = reader.Read32();
  uint32_t height = reader.Read32();
  cout << "widht: " << width << ", height: " << height << endl;
  reader.SetCurPos(nStartPos+nDataLen);
  return 0;
}
