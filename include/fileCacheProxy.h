#ifndef _FILE_CACHE_PROXY_H
#define _FILE_CACHE_PROXY_H

#include <vector>
#include <future>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>

#include "event.h"
#include "http.h"
#include "buffer.h"

#include "json.hpp"

#include "fastdfs/client_func.h"
#include "fastdfs/fdfs_global.h"
#include "fastdfs/fdfs_client.h"

#include "configReader.h"
#include "utilities.h"
#include "logger.h"
#include "common.h"
#include "thread_pool.h"

#define INVOEKE_FUNC( NAME ) \
    static void invoke_##NAME(struct evhttp_request *req, void *arg) \
    { \
        getInstance()->NAME(req, arg); \
    }
#define INVOEKED_FUNC(NAME) (invoke_##NAME)

typedef void (*cb)(struct evhttp_request *, void *);

class fileCacheProxy
{
public:

    static fileCacheProxy *getInstance();

    class Garbo{
    public:
        ~Garbo(){
            delete fileCacheProxy::_Instance;
            fileCacheProxy::_Instance = nullptr;
        }
    };
    static Garbo m_garbo;

    ~fileCacheProxy();

    int init();
    int startService(void);
    int signal_handle(unsigned int signum);

    INVOEKE_FUNC(httpd_handler);
    INVOEKE_FUNC(fileinfo_handler);
    INVOEKE_FUNC(upload_handler);
    INVOEKE_FUNC(delete_handler);

private:

    struct threadParam
    {
        int threadID;
        std::future<int> retval ;

        event_base * ev;
        evhttp * ev_listen;

        TrackerServerInfo * info;
        ConnectionInfo * connectResult;

        threadParam(int _threadID, event_base * _ev, evhttp * _ev_listen, TrackerServerInfo * _info, ConnectionInfo * _connectResult );
        ~threadParam();
    };

    fileCacheProxy();
    static fileCacheProxy *_Instance;

    static constexpr int PRIVILEAGE_644 = (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    static constexpr int logFileSize                 = 4;
    static constexpr int logFileBkupNum              = 4;
    static constexpr const char* configFileName      = "fileCacheProxy.conf" ;
    static constexpr const char* FC_LOGGER_NAME      = "fileCacheProxy";
    static constexpr const char* PROCESS_LOGGER_NAME = "process";

    static constexpr int MAX_CONNECTION_TEST = 16;
    static constexpr int MAX_FDFSMetaData_CNT = 32;
    
    // using CB = std::function<void(struct evhttp_request *, void *)>; 
    std::map<std::string, cb> mPath2Handle;
    void httpd_handler(struct evhttp_request * req, void * arg);
    void upload_handler(struct evhttp_request * req, void * arg);
    void delete_handler(struct evhttp_request * req, void * arg);
    void fileinfo_handler(struct evhttp_request * req, void * arg);

    int isReady;
    pid_t pid;
    char mWorkDir[PATH_LEN];

    int listenfd;

    std::threadpool *mThreadPool;
    std::vector<threadParam *> threadParams;

    // Logger
    std::string mlogDir;
    std::string mFclogFile;
    std::shared_ptr<spdlog::logger> m_fc_rotating_logger;

    // FileID recoder
    std::string mRecordDir;
    std::string mProclogFile;
    std::ofstream fFileID;

    // Load from config
    std::string localhost;
    uint16_t port;
    int ThreadCount;
    int expiredays;
    char mClientConfPath[PATH_LEN];

};


#endif