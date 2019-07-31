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

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private Thread mThread;
    private Button mButton;
    private HandlerThread mHandlerThread;
    private Handler mHandler;
    private CurlMulti mCurlMulti;
    private List<String> mList = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        mList.add("http://img.momocdn.com/feedvideo/A4/79/A4796DEB-34B3-B0C8-5F78-7061A711E5EE20170730.mp4");
//        mList.add("http://img.momocdn.com/feedvideo/D4/72/D472713F-B023-91DC-31CB-780C86896CE120171022.mp4");
//        mList.add("https://img.momocdn.com/feedvideo/D4/72/D472713F-B023-91DC-31CB-780C86896CE120171022.mp4");
        mList.add("http://img.momocdn.com/homepagevideo/B9/20/B920C864-FE74-4F42-ADD6-4EC671E981D820190219.mp4");

        mCurlMulti = new CurlMulti();
        mHandlerThread = new HandlerThread("DownloadThread");
        mHandlerThread.start();

        mHandler = new Handler(mHandlerThread.getLooper(), new Handler.Callback() {
            @Override
            public boolean handleMessage(Message message) {

                switch (message.what) {
                    case 0:
                        mCurlMulti.testHttpsRequest();
                        break;

                    case 1:
                        for (String url : mList) {
                           mCurlMulti.addTask(url, null, 0, -1);
                        }
                        break;
                }
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

        Button button = (Button) findViewById(R.id.multi_btn);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                mCurlMulti.createCurlMulti();
                mCurlMulti.startLoop();

                mHandler.sendEmptyMessage(1);
            }
        });

    }

}
