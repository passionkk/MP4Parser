#include <iostream>
#include <string>
using namespace std;

struct stBox {
  uint64_t boxLen;
  uint8_t boxType[4];
  uint8_t* boxData;
};

int main() {
  cout << "this is a mp4 parser" << endl;
  std::string strInput = "../../../test.mp4";
  FILE* fp = fopen(strInput.c_str(), "rb");
  if (fp) {
    uint nReadUnit = 2048 * 1000;
    uint8_t* szReadBuf = new uint8_t[nReadUnit];
    fseek(fp, 0, SEEK_END);
    uint fileSize = ftell(fp);
    cout << "file size is " << fileSize << endl;
    fseek(fp, 0, SEEK_SET);
    
    int nOffSet = 0;
    int nBoxDatalen = 0;
    stBox box;
    
    while(fileSize > 0) {
      fread(szReadBuf, 1, nReadUnit, fp);
      
      box.boxLen = szReadBuf[nOffSet+0] | szReadBuf[nOffSet+1] |
      szReadBuf[nOffSet+2] | szReadBuf[nOffSet+3];
      nOffSet+=4;
      memcpy(box.boxType, szReadBuf+nOffSet, 4);
      nOffSet += 4;
      nBoxDatalen = box.boxLen - 8;
      box.boxData = new uint8_t[nBoxDatalen];
      memcpy(box.boxData, szReadBuf+nOffSet, nBoxDatalen);
      
      printf("box type: %s, size:%lld, dataLen:%d\n", box.boxType, box.boxLen, nBoxDatalen);
      for(int i = 0;i < nBoxDatalen; i++) {
        printf("%02x ", box.boxData[i]);
      }
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
