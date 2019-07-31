#include <jni.h>
#include <string>

//#include <openssl/md4.h>
#include <curl/curl.h>
#include <android/log.h>

#include "CurlMulti.hpp"


extern "C" JNIEXPORT jstring JNICALL
Java_net_maytrue_openssl_CurlMulti_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

//    MD4_CTX context;
//
//    MD4_Init(&context);

    return env->NewStringUTF(hello.c_str());

}

static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
    int length = size * nmemb;
    __android_log_print(ANDROID_LOG_DEBUG, "curl", "length:%d", length);
    ((std::string*)userp)->append((char*)dest, size * nmemb);
    return size * nmemb;
}


static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    __android_log_print(ANDROID_LOG_DEBUG, "curl", "dltotal:%f, dlnow:%f", dltotal, dlnow);
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_net_maytrue_openssl_CurlMulti_testHttpsRequest(
        JNIEnv* env,
        jobject thiz) {

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
//        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
        curl_easy_setopt(curl, CURLOPT_URL, "http://img.momocdn.com/feedvideo/A4/79/A4796DEB-34B3-B0C8-5F78-7061A711E5EE20170730.mp4");
//        curl_easy_setopt(curl, CURLOPT_URL, "https://baidu.com/");
        /* Now specify the POST data */
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)123);

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);


        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        __android_log_print(ANDROID_LOG_DEBUG, "curl", "curl_easy_perform");
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        __android_log_print(ANDROID_LOG_DEBUG, "curl", "curl_easy_perform:%d", res);

        if (res == CURLE_OK) {
            __android_log_print(ANDROID_LOG_DEBUG, "curl", "%s", readBuffer.c_str());
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

extern "C" JNIEXPORT jlong JNICALL
Java_net_maytrue_openssl_CurlMulti_nativeCreateCurlMulti(
        JNIEnv* env,
        jobject thiz) {

    CurlMulti *instance = new CurlMulti();
    return (jlong)instance;
}

extern "C" JNIEXPORT void JNICALL
Java_net_maytrue_openssl_CurlMulti_nativeStartLoop(
        JNIEnv* env,
        jobject thiz,
        jlong instance) {
    CurlMulti *multi = (CurlMulti *)instance;
    multi->startLoop();
}

extern "C" JNIEXPORT void JNICALL
Java_net_maytrue_openssl_CurlMulti_nativeAddTask(
        JNIEnv* env,
        jobject thiz,
        jlong instance,
        jstring url,
        jstring header,
        jlong start,
        jlong end) {
    CurlMulti *multi = (CurlMulti *)instance;

    const char *urlCString = nullptr;
    if (url) {
        urlCString = env->GetStringUTFChars(url, NULL);
    }

    const char *headerCString = nullptr;
    if (header) {
        headerCString = env->GetStringUTFChars(header, NULL);
    }

    std::string urlString;
    if (urlCString) {
        urlString = std::string(urlCString);
    }

    std::string headerString;
    if (headerCString) {
        headerString = std::string(headerCString);
    }

    multi->addTask(urlString, headerString, start, end);

    if (url && urlCString) {
        env->ReleaseStringUTFChars(url, urlCString);
    }

    if (header && headerCString) {
        env->ReleaseStringUTFChars(header, headerCString);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_net_maytrue_openssl_CurlMulti_nativeDestroyCurlMulti(
        JNIEnv* env,
        jobject thiz,
        jlong instance) {
    CurlMulti *multi = (CurlMulti *)instance;
    multi->stopLoop();
    delete multi;
    multi = nullptr;
}

