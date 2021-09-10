#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

struct stBox {
  uint32_t boxLen;
  uint64_t boxLargeLen;
  uint8_t boxType[4];
  uint8_t* boxData;
};

int main() {
  cout << "this is a mp4 parser" << endl;
  std::string strInput = "../../../test.mp4";
  FILE* fp = fopen(strInput.c_str(), "rb");
  if (fp) {
    const uint nReadOnce = 2048 * 1000;
    uint8_t* szReadBuf = new uint8_t[nReadOnce];
    fseek(fp, 0, SEEK_END);
    uint fileSize = ftell(fp);
    cout << "file size is " << fileSize << endl;
    fseek(fp, 0, SEEK_SET);
    int nOffSet = 0;
    int nBoxDatalen = 0;
    uint64_t nMaxDataSize = 400*1000;
    stBox box;
    box.boxData = new uint8_t[nMaxDataSize];
    int nFileOffset = 0;
    int nReadUnit = nReadOnce;
    int nLeftToUse = 0;
    while(nFileOffset < fileSize) {
      //循环读取直到文件尾
      nReadUnit = std::min(fileSize - nFileOffset, nReadOnce);
      nLeftToUse = nReadOnce - nOffSet;
      nReadUnit -= nLeftToUse;
      memmove(szReadBuf, szReadBuf+nOffSet, nLeftToUse);
      fread(szReadBuf+nLeftToUse, 1, nReadUnit, fp);
      nOffSet = 0;
      //size 一下把4字节长度和可能的largeSize都读取了以及type
      if(nReadOnce - nOffSet >= 16) {
      memcpy((void*)&box.boxLen, szReadBuf, 4);
      box.boxLen = szReadBuf[nOffSet+0] | szReadBuf[nOffSet+1] | szReadBuf[nOffSet+2] | szReadBuf[nOffSet+3];
      nOffSet += 4;
      if(box.boxLen == 1) {
        memcpy((void*)&box.boxLargeLen, szReadBuf, 8);
        nOffSet += 8;
      }
      //type
      memcpy(box.boxType, szReadBuf+nOffSet, 4);
      nOffSet += 4;
      }
      //data
      nBoxDatalen = box.boxLen - 4 - (box.boxLen==1 ? 4 : 8);
      if(box.boxLen > nMaxDataSize || box.boxLargeLen > nMaxDataSize)
      {
        nMaxDataSize = std::max((uint64_t)box.boxLen, box.boxLargeLen);
        delete[] box.boxData;
        box.boxData = new uint8_t[nMaxDataSize];
      }
      memcpy(box.boxData, szReadBuf+nOffSet, nBoxDatalen);
      printf("box type: %s, size:%lld, dataLen:%d\n", box.boxType, box.boxLen, nBoxDatalen);
#if 0 //暂时不显示
      for(int i = 0;i < nBoxDatalen; i++) {
        printf("%02x ", box.boxData[i]);
      }
#endif
      printf("\n");
    }
    fclose(fp);
    delete[] szReadBuf;
  }
  else {
    fp = fopen(strInput.c_str(), "wb");
    fclose(fp);
    cout << strInput << " open fail" << endl;
  }
  return 0;
}
