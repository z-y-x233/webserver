/*
 * @Author       : mark
 * @Date         : 2020-06-26
 * @copyleft Apache 2.0
 */ 

#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
class Buffer {
public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;           //可写字节数
    size_t ReadableBytes() const;           //可读字节数
    size_t PrependableBytes() const;        //已用字节数

    const char* Peek() const;               //返回可读位置底层指针
    char* BeginWrite();                     //返回可写位置底层指针
    const char* BeginWriteConst() const;    

    void EnsureWriteable(size_t len);       //确保可写字节数大于len

    void HasWritten(size_t len);            //写指针向后移动len个字节
    void Retrieve(size_t len);              //读指针往后移动len个字节
    void RetrieveUntil(const char* end);    //读指针移动到end

    void RetrieveAll() ;                    //将整个buffer置零, 初始化读写指针为0
    std::string RetrieveAllToStr();         //讲可读部分作为字符串返回, 然后初始化置零


    void Append(const char* str, size_t len);       //将从char* 指向的地址开始长度为len字节的数据追加到缓冲区
    void Append(const void* data, size_t len);      //同上
    void Append(const std::string& str);            //同上
    void Append(const Buffer& buff);                //同上

    ssize_t ReadFd(int fd, int* Errno);             //将fd的数据读到缓冲区中
    ssize_t WriteFd(int fd, int* Errno);            //将缓冲区中的文件写到fd中

private:
    char* BeginPtr_();                  //返回底层数据结构开始指针
    
    const char* BeginPtr_() const;      //返回底层数据结构开始指针

    void MakeSpace_(size_t len);        //若可用空间小于len, 则扩大一倍空间

    std::vector<char> buffer_;          //buffer底层数据结构
    
    std::atomic<std::size_t> readPos_;  //当前可读数据指针

    std::atomic<std::size_t> writePos_; //当前可写数据指针
};

#endif //BUFFER_H