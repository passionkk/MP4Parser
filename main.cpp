#include <iostream>
#include <string>
#include <algorithm>
#include "FileReader.h"
#include "Fourcc.h"
#include <vector>

using namespace std;

#define BOX_SIZE_LENGTH 4
#define BOX_TYPE_LENGTH 4
#define BOX_DATA_INVALID_LENTHT (BOX_SIZE_LENGTH + BOX_TYPE_LENGTH)

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
int ParseEdts(FileReader& reader, int nDataLen);
int ParseEtls(FileReader& reader, int nDataLen);
int ParseMdia(FileReader& reader, int nDataLen);
int ParseMdhd(FileReader& reader, int nDataLen);
int ParseHdlr(FileReader& reader, int nDataLen);
int ParseMinf(FileReader& reader, int nDataLen);
int ParseVmhd(FileReader& reader, int nDataLen);
/*
给你一个包含 n 个整数的数组 nums，判断 nums 中是否存在三个元素 a，b，c ，使得 a + b + c = 0 ？请你找出所有和为 0 且不重复的三元组。
注意：答案中不可以包含重复的三元组。
示例 1：
输入：nums = [-1,0,1,2,-1,-4]
输出：[[-1,-1,2],[-1,0,1]]
示例 2：
输入：nums = []
输出：[]
示例 3：
输入：nums = [0]
输出：[]
*/
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
      std::vector<vector<int>> result = {};
      if(nums.size() < 3)
        return result;
      std::sort(nums.begin(), nums.end());
      size_t left=0;
      size_t rigth= nums.size()-1;
      int sum = 0;
      while(left+1 < rigth) {
        sum =nums[left] + nums[left+1] + nums[rigth];
        if(sum == 0) {
          std::vector<int> aa= {nums[left], nums[left+1], nums[rigth]};
          //result.push_back({nums[left], nums[left+1], nums[right]});
        }
        else if(sum){
          
        }
      }
      
    }
};

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
        ParseMvhd(reader, box.boxLen - 4 - 4);//减去dataLen和headType
        break;
      case MP4_TRAK:
        ParseTrak(reader, box.boxLen);
        break;
      case MP4_TKHD:
        ParseTkhd(reader, box.boxLen);
        break;
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
    cout << "box type: " << box.boxType << ", size:" << box.boxLen << endl;
    switch (subType) {
      case MP4_TKHD:
        ParseTkhd(reader, box.boxLen - 4 - 4);
        break;
      case MP4_TREF:
      case MP4_TRGR:
        reader.Skip(box.boxLen);
        break;
      case MP4_EDTS:
        cout << "box type: edts, size:" << box.boxLen << endl;
        ParseEdts(reader, box.boxLen-BOX_DATA_INVALID_LENTHT);
        break;
      case MP4_MDIA:
        ParseMdia(reader, box.boxLen);
        break;
      case MP4_MDHD:
        ParseMdhd(reader, box.boxLen-BOX_DATA_INVALID_LENTHT);
        break;
      case MP4_HDLR:
        ParseHdlr(reader, box.boxLen-BOX_DATA_INVALID_LENTHT);
        break;
      case MP4_MINF:
        ParseMinf(reader, box.boxLen - BOX_DATA_INVALID_LENTHT);
        break;
      case MP4_VMHD:
        ParseVmhd(reader, box.boxLen - BOX_DATA_INVALID_LENTHT);
        break;
      case MP4_STBL:
        break;
      case MP4_STSD:
        //ParseStsd(reader, box.boxLen-BOX_DATA_INVALID_LENTHT);
        break;
      default:
        reader.Skip(box.boxLen - BOX_DATA_INVALID_LENTHT);
        break;
    }
  }
  return 0;
}

int ParseTkhd(FileReader& reader, int nDataLen) {
  int nStartPos = reader.GetCurPos();
  cout << "box type: tkhd, size:" << nDataLen << endl;
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
  } else {
    // uint32_t
    creation_time = reader.Read32();
    modification_time = reader.Read32();
    track_id = reader.Read32();
    reserved = reader.Read32();
    duration = reader.Read32();
  }
  cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
  ", track_id: " << track_id << ", duartion: " << duration << endl;

  reader.Skip(4*2);//const unsigned int(32)[2] reserved = 0;
  reader.Skip(2);//template int(16) layer = 0;
  reader.Skip(2);//template int(16) alternate_group = 0;
  reader.Skip(2);//template int(16) volume = {if track_is_audio 0x0100 else 0};
  reader.Skip(2);//const unsigned int(16) reserved = 0;
  reader.Skip(4*9);//template int(32)[9] matrix= { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
  // unity matrix

  uint32_t width = reader.Read32() >> 16;
  uint32_t height = reader.Read32() >> 16;
  cout << "width: " << width << ", height: " << height << endl;
  reader.SetCurPos(nStartPos+nDataLen);
  return 0;
}

int ParseEdts(FileReader& reader, int nDataLen) {
  //Zero or one
  //如果有的话，下一个是Elst
  stBox subBox;
  subBox.boxLen = reader.Read32();
  reader.ReadBuf(subBox.boxType, 4);
  cout << "box type: " << subBox.boxType << ", len: " << subBox.boxLen << endl;
  reader.Skip(subBox.boxLen - BOX_DATA_INVALID_LENTHT);
  return 0;
}

int ParseEtls(FileReader& reader, int nDataLen) {
  //Zero or one
  //ParseEdts内简单输出
  return 0;
}

int ParseMdia(FileReader& reader, int nDatalen)
{
  //4字节的mdia长度+ 4字节类型(mdia)
  //接着就是 子box

  return 0;
}

int ParseMdhd(FileReader& reader, int nDataLen)
{
  cout << "box type: mdhd, size:" << nDataLen << endl;
  int nStartPos = reader.GetCurPos();
  int version = reader.Read8();
  int flag = reader.Read24();
  uint64_t creation_time, modification_time, duration;
  uint32_t timescale;
  if(version == 1) {
    creation_time = reader.Read64();
    modification_time = reader.Read64();
    timescale = reader.Read32();
    duration = reader.Read64();
  } else {
    creation_time = reader.Read32();
    modification_time = reader.Read32();
    timescale = reader.Read32();
    duration = reader.Read32();
  }
  //uint8_t pad = reader.Read8();
  //bit(1) pad = 0;
  //unsigned int(5)[3] language; // ISO-639-2/T language code
  //unsigned int(16) pre_defined = 0;
  //一共 1+5*3+16 = 32bit
  reader.Skip(32/8);
  cout << "create_time: " << creation_time << ", modify_time: " << modification_time <<
  ", timescale: " << timescale << ", duartion: " << duration << endl;
  cout<< "real time is: " << duration*1.0 /timescale <<endl;
  
  return 0;
}

int ParseHdlr(FileReader& reader, int nDataLen)
{
  /*
  aligned(8) class HandlerBox extends FullBox(‘hdlr’, version = 0, 0) {
   unsigned int(32) pre_defined = 0;
   unsigned int(32) handler_type;
   const unsigned int(32)[3] reserved = 0;
   string name;
  }
  */
  cout << "box type: hdlr, size:" << nDataLen << endl;
  int version = reader.Read8();
  int flag = reader.Read24();
  reader.Skip(4);
  reader.Skip(4);
  reader.Skip(4*3);
  uint8_t* buf = new uint8_t[nDataLen-4*5];
  cout << "string: " << buf;
  delete[] buf;
  return 0;
}

int ParseMinf(FileReader& reader, int nDataLen)
{
  cout << "box type: minf, size:" << nDataLen << endl;
  return 0;
}

int ParseVmhd(FileReader& reader, int nDataLen)
{
  cout << "box type: vmhd, size:" << nDataLen << endl;
  reader.Skip(nDataLen);
  return 0;
}
