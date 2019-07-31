//
// Created by guowei on 2019-07-31.
//

#ifndef CURL_ANDROID_DEMO_CURLMUTIL_HPP
#define CURL_ANDROID_DEMO_CURLMUTIL_HPP

#include <memory>
#include <string>

class CurlMulti {
public:
    CurlMulti();

    ~CurlMulti();

    void startLoop();

    void addTask(std::string& url, std::string& header, int64_t start, int64_t end);

    void stopLoop();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};


#endif //CURL_ANDROID_DEMO_CURLMUTIL_HPP
