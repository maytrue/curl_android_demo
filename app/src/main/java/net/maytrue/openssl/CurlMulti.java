package net.maytrue.openssl;

import android.util.Pair;

public class CurlMulti {

    private long mCurlMulti = 0;

    public void createCurlMulti() {
        if (mCurlMulti > 0) {
            return;
        }
        mCurlMulti = nativeCreateCurlMulti();
    }

    public void startLoop() {
        if (mCurlMulti > 0) {
            nativeStartLoop(mCurlMulti);
        }
    }

    public void addTask(String url, String httpHeader, long start, long end) {
        if (mCurlMulti > 0) {
            nativeAddTask(mCurlMulti, url, httpHeader, start, end);
        }
    }

    public void destoryCurlMulti() {
        if (mCurlMulti > 0) {
            nativeDestroyCurlMulti(mCurlMulti);
            mCurlMulti = 0;
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void testHttpsRequest();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("mmcrypto");
        System.loadLibrary("mmssl");
        System.loadLibrary("curl");
        System.loadLibrary("native-lib");
    }

    private native long nativeCreateCurlMulti();

    private native void nativeStartLoop(long instance);

    private native void nativeAddTask(long instance, String url, String httpHeader, long start, long end);

    private native void nativeDestroyCurlMulti(long instance);
}
