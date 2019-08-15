#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#define AVIF_HASINDEX  0x00000010

typedef unsigned short BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long FOURCC;

// MainAVIHeader
typedef struct {

    DWORD dwMicroSecPerFrame;		//显示每桢所需的时间ns，定义avi的显示速率
    DWORD dwMaxBytesPerSec;			//最大的数据传输率
    DWORD dwPaddingGranularity;		//记录块的长度需为此值的倍数，通常是2048
    DWORD dwFlages;				//AVI文件的特殊属性，如是否包含索引块，音视频数据是否交叉存储
    DWORD dwTotalFrame;			//文件中的总桢数
    DWORD dwInitialFrames;			//说明在开始播放前需要多少桢
    DWORD dwStreams;				//文件中包含的数据流种类
    DWORD dwSuggestedBufferSize;	       //建议使用的缓冲区的大小，通常为存储一桢图像以及同步声音所需要的数据之和
    DWORD dwWidth;				//图像宽
    DWORD dwHeight;				//图像高
    DWORD dwReserved[4];//保留值

}MainAVIHeader;

typedef struct{

    WORD wLeft;
    WORD wTop;
    WORD wRight;
    WORD wBottom;

}RECT;

// strh
typedef struct {

    FOURCC fccType;			//4字节，表示数据流的种类 vids 表示视频数据流, auds 音频数据流
    FOURCC fccHandler;		//4字节 ，表示数据流解压缩的驱动程序代号
    DWORD dwFlags;			//数据流属性
    WORD wPriority;			//此数据流的播放优先级
    WORD wLanguage;			//音频的语言代号
    DWORD dwInitalFrames;	 //说明在开始播放前需要多少桢
    DWORD dwScale;			//数据量，视频每桢的大小或者音频的采样大小
    DWORD dwRate;			//dwScale /dwRate = 每秒的采样数
    DWORD dwStart;			//数据流开始播放的位置，以dwScale为单位
    DWORD dwLength;			//数据流的数据量，以dwScale为单位
    DWORD dwSuggestedBufferSize;	//建议缓冲区的大小
    DWORD dwQuality;			//解压缩质量参数，值越大，质量越好
    DWORD dwSampleSize;		//音频的采样大小
    RECT rcFrame;			//视频图像所占的矩形

}AVIStreamHeader;

typedef struct {

    BYTE rgbBlue;		// 蓝色分量
    BYTE rgbGreen;		// 绿色分量
    BYTE rgbRed;		// 红色分量
    BYTE rgbReserved;	// 保留字节（用作Alpha通道或忽略）

} RGBQUAD;

typedef struct {

    DWORD dwSize;
    DWORD dwWidth;
    DWORD dwHeight;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwCompression;
    DWORD dwSizeImage;
    DWORD dwXPelsPerMeter;
    DWORD dwYPelsPerMeter;
    DWORD dwClrUsed;
    DWORD dwClrImportant;

}BITMAPINFOHEADER;

//视频流结构
typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1]; //颜色表
}BITMAPINFO;

//音频流结构
typedef struct
{
    WORD wFormatTag;
    WORD wChannels;		//声道数
    DWORD dwSamplesPerSec;	//采样率
    DWORD dwAvgBytesPerSec; //WAVE声音中每秒的数据量
    WORD wBlockAlign;	//数据块的对齐标志
    WORD wBitsPerSample;    //每次采样的数据量
    WORD wSize;		//此结构的大小

}WAVEFORMAT;

typedef struct{

    char chunk_id[4];
    DWORD is_key;
    DWORD pos;
    DWORD size;

}AVI_IDX;

typedef struct _avioldindex {
  FOURCC fcc;                  // 必须为‘idx1’
  DWORD cb;                   // 本数据结构的大小，不包括最初的8个字节（fcc和cb两个域）
  struct _avioldindex_entry {
  DWORD dwChunkId;            // 表征本数据块的四字符码
  DWORD dwFlags;              // 说明本数据块是不是关键帧、是不是‘rec ’列表等信息
  DWORD dwOffset;             // 本数据块在文件中的偏移量
  DWORD dwSize;               // 本数据块的大小
  } aIndex[];                // 这是一个数组！为每个媒体数据块都定义一个索引信息
} AVIOLDINDEX;

//解析avlist信息，重点为 StreamHeader结构体
void aviListInfo(char *pInfoBuf,int *readPos)
{
    char buf[5];
    DWORD dwValue=0;
    int flag=0;

    //list
    memset(buf,0,sizeof(buf));
    memcpy(buf,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"list: "<<buf<<endl;

    //list len
    dwValue = 0;
    memcpy((char *)&dwValue,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"list len: "<<dwValue<<endl;

    //strl
    memset(buf,0,5);
    memcpy(buf,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"strl: "<<buf<<endl;

    //strh
    memset(buf,0,5);
    memcpy(buf,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"strh: "<<buf<<endl;

    //strh len
    dwValue = 0;
    memcpy((char *)&dwValue,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"strh len: "<<dwValue<<endl;

    //strh info
    AVIStreamHeader streamHeader;
    memset(&streamHeader,0,sizeof(streamHeader));
    memcpy((char *)&streamHeader,pInfoBuf+*readPos,sizeof(streamHeader));
    *readPos+=sizeof(AVIStreamHeader);
    cout<<endl;
    cout<<"AVIStreamHeader len: "<<sizeof(streamHeader)<<endl;
    //cout<<"!!!!!!!!!!!!!!!!!!!!!!!    readPos:"<<*readPos<<endl;

    memset(buf,0,5);
    memcpy(buf,(char *)&streamHeader.fccType,4);
    cout<<"fccType: "<<buf<<endl;
    string fccType(buf);

    if(fccType.compare(0,4,"vids")==0) //文件类型
        flag = 0;
    else
        flag = 1;

    memset(buf,0,5);
    memcpy(buf,(char *)&streamHeader.fccHandler,4);
    cout<<"fccHandler: "<<buf<<endl;
    cout<<"Flags: "<<streamHeader.dwFlags<<endl;
    cout<<"Priority: "<<streamHeader.wPriority<<endl;
    cout<<" Language: "<<streamHeader.wLanguage<<endl;
    cout<<" InitalFrames: "<<streamHeader.dwInitalFrames<<endl;
    cout<<" Scale: "<<streamHeader.dwScale<<endl;
    cout<<" Rate: "<<streamHeader.dwRate<<endl;
    cout<<" Start: "<<streamHeader.dwStart<<endl;
    cout<<" Length: "<<streamHeader.dwLength<<endl;
    cout<<" SuggestedBufferSize: "<<streamHeader.dwSuggestedBufferSize<<endl;

    cout<<" Quality: "<<streamHeader.dwQuality<<endl;
    cout<<" SampleSize: "<<streamHeader.dwSampleSize<<endl;
    cout<<" left: "<<streamHeader.rcFrame.wLeft<<endl;
    cout<<" top: "<<streamHeader.rcFrame.wTop<<endl;
    cout<<" right: "<<streamHeader.rcFrame.wRight<<endl;
    cout<<" bottom: "<<streamHeader.rcFrame.wBottom<<endl<<endl;

    //strf
    memset(buf,0,sizeof(buf));
    memcpy(buf,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"strf: "<<buf<<endl;

    //strf len
    dwValue = 0;
    memcpy((char *)&dwValue,pInfoBuf+*readPos,4);
    *readPos+=4;
    cout<<"strf len: "<<dwValue<<endl;

    if(flag==0)//视频信息
    {
        BITMAPINFOHEADER bmiHeader;
        memset(&bmiHeader,0,sizeof(BITMAPINFOHEADER));
        memcpy((char *)&bmiHeader,pInfoBuf+*readPos,sizeof(BITMAPINFOHEADER));
        *readPos+=sizeof(BITMAPINFOHEADER);

        cout<<endl;
        cout<<" BITMAPINFOHEADER len: "<<sizeof(BITMAPINFOHEADER)<<endl;
        cout<<" Size: "<<bmiHeader.dwSize<<endl;
        cout<<" Width: "<<bmiHeader.dwWidth<<endl;
        cout<<" Height: "<<bmiHeader.dwHeight<<endl;
        cout<<" Planes: "<<bmiHeader.wPlanes<<endl;
        cout<<" BitCount: "<<bmiHeader.wBitCount<<endl;
        cout<<" Compression: "<<bmiHeader.dwCompression<<endl;
        cout<<" SizeImage: "<<bmiHeader.dwSizeImage<<endl;
        cout<<" XPelsPerMeter: "<<bmiHeader.dwXPelsPerMeter<<endl;
        cout<<" YPelsPerMeter: "<<bmiHeader.dwYPelsPerMeter<<endl;
        cout<<" ClrUsed: "<<bmiHeader.dwClrUsed<<endl;
        cout<<" ClrImportant: "<<bmiHeader.dwClrImportant<<endl;
    }
    else if(flag==1)  //音频流
    {
        WAVEFORMAT waveFormat;
        memset(&waveFormat,0,sizeof(WAVEFORMAT));
        memcpy((char *)&waveFormat,pInfoBuf+*readPos,sizeof(WAVEFORMAT));
        *readPos+=sizeof(WAVEFORMAT);

        cout<<endl;
        cout<<" WAVEFORMAT len: "<<sizeof(WAVEFORMAT)<<endl;
        cout<<" FormatTag: "<<waveFormat.wFormatTag<<endl;
        cout<<" Channels: "<<waveFormat.wChannels<<endl;
        cout<<" SamplesPerSec: "<<waveFormat.dwSamplesPerSec<<endl;
        cout<<" AvgBytesPerSec: "<<waveFormat.dwAvgBytesPerSec<<endl;
        cout<<" BlockAlign: "<<waveFormat.wBlockAlign<<endl;
        cout<<" BitsPerSample: "<<waveFormat.wBitsPerSample<<endl;
        cout<<" Size: "<<waveFormat.wSize<<endl;

        *readPos+=waveFormat.wSize;

    }

}

//整体解析
int AVI_INFO(char *inputpath)
{
    char buf[5];
    DWORD dwValue = 0;
    DWORD fileLen=0;
    DWORD frameCount=0;
    int hdrlListLen=0;
    char *pInfo=NULL;
    int readPos=0;
    DWORD moviPos=0;

    streampos pos;
    ifstream aviFile;
    aviFile.open(inputpath,ios::in|ios::binary);
    if(!aviFile)
    {
        cout<<inputpath<<" cann't open!"<<endl;
        return -1;
    }
    memset(buf,0,5);
    aviFile.read(buf,4);
    cout<<"riffHead: "<<buf<<endl;

    aviFile.read((char*)&dwValue,4);
    cout<<"file len: "<<dwValue<<endl;

    pos = aviFile.tellg();//current position
    aviFile.seekg(0,ios::end);
    fileLen = aviFile.tellg();
    aviFile.seekg(pos);

    if(fileLen!=(dwValue+8))
    {
        cout<<"file is damaged!"<<endl;
        return -1;
    }
    cout<<"file Len:::  "<<fileLen<<endl;

    //file type
    memset(buf,0,5);
    aviFile.read(buf,4);
    cout<<"file type: "<<buf<<endl;

    string aviFormat(buf);
    if(aviFormat.compare(0,3,"AVI")!=0)
    {
        cout<<"Not avi file!"<<endl;
        return -1;
    }

    //hdrl list
    memset(buf,0,5);
    aviFile.read(buf,4);
    cout<<"hdrl list: "<<buf<<endl;
//    cout<<"=========get there!"<<endl;

    //hdrl len
    dwValue = 0;
    aviFile.read((char*)&dwValue,4);
    cout<<"hdrl list len: "<<dwValue<<endl;
    hdrlListLen = dwValue;
    pInfo = new char[hdrlListLen+1];
    memset(pInfo,0,hdrlListLen+1);
    aviFile.read(pInfo,dwValue);

    //保存位置
    streampos nowpos = aviFile.tellg();

    //list type
    readPos = 0;
    memset(buf,0,5);
    memcpy(buf,pInfo+readPos,4);
    readPos+=4;
    cout<<"list type: "<<buf<<endl;

    //avih
    memset(buf,0,5);
    memcpy(buf,pInfo+readPos,4);
    readPos+=4;
    cout<<"avih: "<<buf<<endl;

    //avih len
    dwValue = 0;
    aviFile.read((char *)&dwValue,4);
    readPos+=4;
    cout<<"avih len: "<<dwValue<<endl;

    //avi header
    MainAVIHeader mainAVIHeader;
    memset(&mainAVIHeader,0,sizeof(MainAVIHeader));
    memcpy((char *)&mainAVIHeader,pInfo+readPos,sizeof(MainAVIHeader));
    readPos+=sizeof(MainAVIHeader);

    cout<<endl;

    cout<<" MainAVIHeader len: "<<sizeof(MainAVIHeader)<<endl;
    cout<<" MicroSecPerFrame: "<<mainAVIHeader.dwMicroSecPerFrame<<endl;
    cout<<" MaxBytesPerSec: "<<mainAVIHeader.dwMaxBytesPerSec<<endl;
    cout<<" PaddingGranularity: "<<mainAVIHeader.dwPaddingGranularity<<endl;
    cout<<" Flages: "<<mainAVIHeader.dwFlages<<endl;
    cout<<" TotalFrame: "<<mainAVIHeader.dwTotalFrame<<endl;
    frameCount=mainAVIHeader.dwTotalFrame;
    cout<<" InitialFrames: "<<mainAVIHeader.dwInitialFrames<<endl;
    cout<<" Streams: "<<mainAVIHeader.dwStreams<<endl;
    cout<<" SuggestedBufferSize: "<<mainAVIHeader.dwSuggestedBufferSize<<endl;
    cout<<" Width: "<<mainAVIHeader.dwWidth<<endl;
    cout<<" Height: "<<mainAVIHeader.dwHeight<<endl;

    if(!mainAVIHeader.dwFlages & AVIF_HASINDEX)
    {
        cout<<"Hasn't find idx info!"<<endl;
        return -1;
    }

    //list1
    cout<<endl<<"====================list1================"<<endl;

    aviListInfo(pInfo,&readPos);

    aviFile.seekg(nowpos);//不知道为什么
    //list 2
    if(mainAVIHeader.dwStreams>1)
    {
        int i;
        for(i=readPos;i<hdrlListLen;)
        {
            memset(buf,0,5);
            memcpy(buf,pInfo+readPos,4);
            cout<<" buf: "<<buf<<endl;
            string listFlag(buf);
            if(0==listFlag.compare(0,4,"LIST")) break;
            readPos+=4;
            i=readPos;
        }
        //list2
        cout<<endl<<"==============list2=============="<<endl;
        aviListInfo(pInfo,&readPos);
    }
    cout<<"readPos= "<<readPos<<endl;

    delete []pInfo;
    pInfo = NULL;

    //find list
    int i;
    string listFlag;
    for(i=readPos+8;i<fileLen;)
    {
        memset(buf,0,5);
        aviFile.read(buf, 4);
        cout<<" buf: "<<buf<<endl;
        listFlag=buf;
        if(0==listFlag.compare(0,4,"LIST"))
        {
            //len
            dwValue=0;
            aviFile.read((char *)&dwValue,4);
            i+=4;
            cout<<"LIST len: "<<dwValue<<endl;

            //type
            memset(buf,0,5);
            aviFile.read(buf,4);
            i+=4;
            cout<<"LIST type: "<<buf<<endl;
            listFlag=buf;
            aviFile.seekg(dwValue-4,ios::cur);

            if(0==listFlag.compare(0,4,"movi"))
            {
                moviPos=aviFile.tellg();
                moviPos=moviPos-dwValue+4;
                break;
            }
        }
        else if(0==listFlag.compare(0,4,"JUNK"))
        {
            //len
            dwValue = 0;
            aviFile.read((char *)&dwValue,4);
            cout<<"JUNK len: "<<dwValue<<endl;
            aviFile.seekg(dwValue,ios::cur);
        }
        i+=4;

    }
    cout<<"11111 moviPosition: "<<moviPos<<endl;
    if(moviPos==0)
        return -1;
    cout<<endl<<"================idxl info=================";

    //idxl
    memset(buf,0,5);
    aviFile.read(buf,4);
    cout<<"idxl: "<<buf<<endl;

    dwValue = 0;
    aviFile.read((char *)&dwValue,4);
    cout<<" idxl len: "<<dwValue<<endl;

    // avi file index
//    AVI_IDX *pIndx=new AVI_IDX[frameCount];
//    char *pszBuf = new char[1024*1024];
//    DWORD nBufSize = 1024*1024;

//    // open video file
//    ofstream videoFile;
//    videoFile.open("E:/test.avi",ios::out | ios::binary);

//    if(dwValue>=frameCount*sizeof(AVI_IDX))
//        dwValue=frameCount*sizeof(AVI_IDX);

//    aviFile.read((char *)pIndx,dwValue);
//    int idx;
//    for(idx=0;idx<frameCount;++idx)
//    {
//        aviFile.seekg(moviPos+pIndx[idx].pos,ios::beg);
//        memset(pszBuf,0,nBufSize);
//        aviFile.read(pszBuf,pIndx[idx].size);
//        if(!strncmp(pIndx[idx].chunk_id,"00dc",4))
//            continue;
//        videoFile.write(pszBuf,pIndx[idx].size);


//    }
//    delete []pIndx;
//    pIndx=NULL;
//    delete []pszBuf;
//    pszBuf=NULL;
//    videoFile.close();
    aviFile.close();
    return 0;


}

//破坏头结构(最简单方法)
void Destoryfile(char *path)
{
    ofstream videoFile;
    videoFile.open(path,ios::out | ios::binary|ios::in);

    if(!videoFile)
    {
        cout<<"cann't open "<<endl;
        return ;
    }

    char buf[5]="ffff";

    //修改 'RIFF' 识别
    videoFile.seekp(0,ios::beg);
    videoFile.write(buf,4);

    //修改 'AVI' 识别
    videoFile.seekp(8,ios::beg);
    videoFile.write(buf,4);

    //修改 hdrl标志
    videoFile.seekp(12,ios::beg);
    videoFile.write(buf,4);

    //修改 hdrl类型
    videoFile.seekp(20,ios::beg);
    videoFile.write(buf,4);

    //修改 avih标志
    videoFile.seekp(24,ios::beg);
    videoFile.write(buf,4);

    //修改stream 数量
    videoFile.seekp(56,ios::beg);
    videoFile.write(buf,4);

    //修改streamHeader type
    videoFile.seekp(108,ios::beg);
    videoFile.write(buf,4);

    //修改编码格式
    videoFile.seekp(112,ios::beg);
    videoFile.write(buf,4);

    videoFile.close();

}

//修复头结构
void RecoveryFile(char *path)
{
    ofstream videoFile;
    videoFile.open(path,ios::out | ios::binary|ios::in);

    if(!videoFile)
    {
        cout<<"cann't open "<<endl;
        return ;
    }

    char buf[5];
    //修复 'RIFF' 识别
    videoFile.seekp(0,ios::beg);
    memset(buf,0,5);
    buf[0]='R';
    buf[1]='I';
    buf[2]= 'F';
    buf[3]='F';
    videoFile.write(buf,4);

    //修复 'AVI'识别
    videoFile.seekp(8,ios::beg);
    memset(buf,0,5);
    buf[0]='A';
    buf[1]='V';
    buf[2]= 'I';
    buf[3]=0x20;
    videoFile.write(buf,4);

    //修复 hdrl标志
    videoFile.seekp(12,ios::beg);
    memset(buf,0,5);
    buf[0]='L';
    buf[1]='I';
    buf[2]= 'S';
    buf[3]='T';
    videoFile.write(buf,4);

    //修复 avih标志
    videoFile.seekp(24,ios::beg);
    memset(buf,0,5);
    buf[0]='a';
    buf[1]='v';
    buf[2]= 'i';
    buf[3]='h';
    videoFile.write(buf,4);

    //修复 hdrl类型
    memset(buf,0,5);
    videoFile.seekp(20,ios::beg);
    buf[0]='h';
    buf[1]='d';
    buf[2]= 'r';
    buf[3]='l';
    videoFile.write(buf,4);

    //修复stream 数量
    videoFile.seekp(56,ios::beg);
    memset(buf,0,5);
    buf[0]=0x01;
    buf[1]=0x00;
    buf[2]= 0x00;
    buf[3]=0x00;
    videoFile.write(buf,4);


    //修复streamHeader type
    memset(buf,0,5);
    buf[0]='v';
    buf[1]='i';
    buf[2]= 'd';
    buf[3]='s';
    videoFile.seekp(108,ios::beg);
    videoFile.write(buf,4);


    //修复编码类型
    memset(buf,0,5);
    buf[0]='m';
    buf[1]='j';
    buf[2]= 'p';
    buf[3]='g';
    videoFile.seekp(112,ios::beg);
    videoFile.write(buf,4);

    videoFile.close();

}

int main(int argc, char *argv[])
{
	if(argc<2)
    {
         cout<<"please type inputpath!"<<endl;
         return 0;
    }

//    AVI_INFO(argv[1]);
    //Destoryfile(argv[1]);
    RecoveryFile(argv[1]);

}
