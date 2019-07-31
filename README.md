# curl_android_demo

CURL Download test with android

## libcurl中curl_easy_perform卡死问题

The following two article are about this problem

https://www.freehacker.cn/tools/libcurl-curl_easy_perform-blocking-problem/

https://www.cnblogs.com/bigben0123/p/3192978.html

But I cannot produce it 

Turn off network, and click button to download file
```
curl_easy_perform:6
```

```
CURLE_COULDNT_RESOLVE_HOST,    /* 6 */
```

Click button to download file, then turn off network 

```
2019-07-31 15:39:46.171 21701-23585/net.maytrue.openssl D/curl: length:1436
2019-07-31 15:39:46.171 21701-23585/net.maytrue.openssl D/curl: dltotal:34412430.000000, dlnow:824798.000000
2019-07-31 15:39:46.171 21701-23585/net.maytrue.openssl D/curl: dltotal:34412430.000000, dlnow:824798.000000
2019-07-31 15:39:46.560 21701-23585/net.maytrue.openssl D/curl: dltotal:34412430.000000, dlnow:824798.000000
2019-07-31 15:39:46.560 21701-23585/net.maytrue.openssl D/curl: curl_easy_perform:56
```

```
CURLE_RECV_ERROR,              /* 56 - failure in receiving network data */
```
curl exist download loop successfully 


