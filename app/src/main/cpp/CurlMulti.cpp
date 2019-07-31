//
// Created by guowei on 2019-07-31.
//

#include "CurlMulti.hpp"
#include "CommonLog.hpp"

#include <curl/curl.h>
#include <thread>
#include <deque>
#include <curl/multi.h>


class Task {
public:
    Task(std::string& url, std::string& header, int64_t start, int64_t end) {
        mURL = url;
        mHeader = header;
        mStart = start;
        mEnd = end;
    }

    ~Task() {

    }

    const std::string &getURL() const {
        return mURL;
    }

    const std::string &getHeader() const {
        return mHeader;
    }

    int64_t getStart() const {
        return mStart;
    }

    int64_t getEnd() const {
        return mEnd;
    }

private:

    std::string mURL;
    std::string mHeader;
    int64_t  mStart;
    int64_t  mEnd;
};

class CurlMulti::Impl
{
public:
    Impl() {
        mCurlMulti = curl_multi_init();
        mRunning = true;
    }

    ~Impl() {
        curl_multi_cleanup(mCurlMulti);
    }

    static int progressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        std::string *url = (std::string*)clientp;
        LOGD("dltotal:%f, dlnow:%f, url:%s", dltotal, dlnow, url->c_str());
        return 0;
    }

    static size_t writeCallback(void *dest, size_t size, size_t nmemb, void *userp)
    {
        int length = size * nmemb;
        std::string *url = (std::string*)userp;
        LOGD("length:%d, url:%s", length, url->c_str());
        return size * nmemb;
    }

    void startLoop()
    {
        LOGD("start loop");
        mThread = std::thread([&](){
            while (mRunning) {
                CURLMcode curlMultiCode;
                CURL *handle = nullptr;
                {
                    std::unique_lock<std::mutex> lock(mMutex);
                    if (!mTaskVector.empty()) {
                        auto task = mTaskVector.front();
                        mTaskVector.pop_front();

                        handle = curl_easy_init();
                        curl_easy_setopt(handle, CURLOPT_URL, task->getURL().c_str());

                        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
                        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);

                        curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
                        std::string url = task->getURL();

                        curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, progressCallback);
                        curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, &url);

                        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback);
                        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &url);

                    }
                }
                // add to multi handle
                if (handle) {
                    curl_multi_add_handle(mCurlMulti, handle);
                }

                int waitResult = 0;
                curlMultiCode = curl_multi_wait(mCurlMulti, NULL, 0, 10, &waitResult);
                LOGD("curl_multi_wait:%d, waitResult:%d", curlMultiCode, waitResult);
                if (curlMultiCode != CURLM_OK) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // perform
                int stillRunning = 0;
                curlMultiCode = curl_multi_perform(mCurlMulti, &stillRunning);
                LOGD("curl_multi_perform:%d, stillRunning:%d", curlMultiCode, stillRunning);


                // read info
                int messageLeft = 0;
                CURLMsg *curlMessage = nullptr;
                do {
                    curlMessage = curl_multi_info_read(mCurlMulti, &messageLeft);
                    if (!curlMessage) {
                        break;
                    }

                    if (curlMessage->msg == CURLMSG_DONE) {
                        CURL *handle = curlMessage->easy_handle;
                        CURLcode code = curlMessage->data.result;
                        const char *errorMessage = curl_easy_strerror(code);
                        int httpCode = 0;
                        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpCode);

                        char *ipAddress;
                        curl_easy_getinfo(handle, CURLINFO_PRIMARY_IP, &ipAddress);

                        LOGD("code:%d, errorMessage:%s, httpCode:%d, ipAddress:%s", code, errorMessage, httpCode, ipAddress);
                        curl_multi_remove_handle(mCurlMulti, handle);
                    }
                } while (curlMessage);
            }
        });

        LOGD("start loop finish");
    }

    void stopLoop() {
        LOGD("stop loop");
        mRunning = false;
        if (mThread.joinable()) {
            mThread.join();
        }
        LOGD("stop loop finish");
    }

    void addTask(std::string& url, std::string& header, int64_t start, int64_t end)
    {
        LOGD("add url:%s", url.c_str());
        std::unique_lock<std::mutex> lock(mMutex);
        auto task = std::make_shared<Task>(url, header, start, end);
        mTaskVector.push_back(task);
        mCondition.notify_all();
    }

private:
    CURLM *mCurlMulti;
    std::thread mThread;

    std::deque<std::shared_ptr<Task>> mTaskVector;
    std::mutex mMutex;
    std::condition_variable mCondition;

    bool mRunning;
};

CurlMulti::CurlMulti() : mImpl(new Impl()) {

}

CurlMulti::~CurlMulti() {

}

void CurlMulti::startLoop()
{
    mImpl->startLoop();
}

void CurlMulti::addTask(std::string& url, std::string& header, int64_t start, int64_t end)
{
    mImpl->addTask(url, header, start, end);
}

void CurlMulti::stopLoop() {
    mImpl->stopLoop();
}
