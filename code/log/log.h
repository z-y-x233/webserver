/*
 * @Author       : mark
 * @Date         : 2020-06-16
 * @copyleft Apache 2.0
 */ 
#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "blockqueue.h"
#include "../buffer/buffer.h"


class Log {
public:
    enum LEVEL{
        DEBUG, INFO, WARN, ERROR
    };
    void init(int level, const char* path = "./log", 
                const char* suffix =".log",
                int maxQueueCapacity = 1024);

    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char *format,...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }
    
private:
    Log();                                  //构造函数
    void AppendLogLevelTitle_(int level);   //根据日志级别添加日志头部信息
    virtual ~Log();                         //析构函数
    void AsyncWrite_();                     //异步写入

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* path_;          //日志文件前缀目录
    const char* suffix_;        //日志文件后缀

    int MAX_LINES_;             //

    int lineCount_;             //行号
    int toDay_;                 //今天日期

    bool isOpen_;               //
 
    Buffer buff_;               //读写缓冲
    int level_;                 //日志级别
    bool isAsync_;              //是否异步
    FILE* fp_;                  //指向当前日志的指针

    std::unique_ptr<BlockDeque<std::string>> deque_;    //
    std::unique_ptr<std::thread> writeThread_;          //
    std::mutex mtx_;                                    //
};

#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif //LOG_H