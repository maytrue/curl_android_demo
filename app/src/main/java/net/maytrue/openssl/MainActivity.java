package net.maytrue.openssl;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private Thread mThread;
    private Button mButton;
    private HandlerThread mHandlerThread;
    private Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mHandlerThread = new HandlerThread("DownloadThread");
        mHandlerThread.start();

        mHandler = new Handler(mHandlerThread.getLooper(), new Handler.Callback() {
            @Override
            public boolean handleMessage(Message message) {
                testHttpsRequest();
                return false;
            }
        });

        // Example of a call to a native method
        mButton = (Button) findViewById(R.id.sample_btn);
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                mHandler.sendEmptyMessage(0);
//                if (mThread != null) {
//                    return;
//                }
//
//                mThread = new Thread() {
//                    @Override
//                    public void run() {
//                        super.run();
//                        testHttpsRequest();
//                    }
//                };
//
//                mThread.start();
            }
        });

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
}
