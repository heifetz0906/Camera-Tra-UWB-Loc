package qct.mmrd.babyfacedetdmo;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Toast;


/**
 * Created by 59732 on 2018/1/13.
 */

public class ConnectBluet extends Activity {

    private final String TAG = ConnectBluet.this.getClass().getSimpleName();

    private BlueToothService mBTService = null;
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    public boolean isConnected = false;

    private final static String BLUETOOTH_ADDRESS = "6C:40:08:B4:3B:E5";

    public Handler getHandler() {
        return handler;
    }

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 0:

                    break;
                case 1:// 扫描完毕
                    // progressDialog.cancel();
                    mBTService.StopScan();
                    break;
                case 2:// 停止扫描
                    break;
            }
        }
    };


    ConnectBlueToothDialog dialog;

    // 弹出蓝牙列表
    public void showDialog() {
        dialog = new ConnectBlueToothDialog(ConnectBluet.this, R.style.confirmDialog1);
        dialog.setmBTService(mBTService);

        dialog.show();
    }
    // 连接蓝牙的各种状况
    @SuppressWarnings("unused")
    private Handler mhandler = new Handler() {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MESSAGE_STATE_CHANGE:// è“�ç‰™è¿žæŽ¥çŠ¶æ€�
                    switch (msg.arg1) {
                        case BlueToothService.STATE_CONNECTED:// å·²ç»�è¿žæŽ¥
                            break;
                        case BlueToothService.STATE_CONNECTING:// æ­£åœ¨è¿žæŽ¥
                            break;
                        case BlueToothService.STATE_LISTEN:
                        case BlueToothService.STATE_NONE:
                            break;
                        case BlueToothService.SUCCESS_CONNECT:
                            isConnected = true;
                            Toast.makeText(ConnectBluet.this, "连接成功", Toast.LENGTH_SHORT).show();//连接蓝牙成功，要进入下一个activity了
                            Intent intent=new Intent();
                            intent.setClass(ConnectBluet.this, MainActivity.class); //设置跳转的Activity
                            MainActivity.mBTService=mBTService;
                            MainActivity.isConnected=isConnected;
                            ConnectBluet.this.startActivity(intent);
                            dialog.dismiss() ;
                            break;
                        case BlueToothService.FAILED_CONNECT:
                            Toast.makeText(ConnectBluet.this, "连接失败", Toast.LENGTH_SHORT).show();
                            break;
                        case BlueToothService.LOSE_CONNECT:
                            isConnected = false;
                            Toast.makeText(ConnectBluet.this, "失去连接", Toast.LENGTH_SHORT).show();
                            switch (msg.arg2) {
                                case -1:
                                    isConnected = false;
                                    break;
                                case 0:
                                    break;
                            }
                    }
                    break;
                case MESSAGE_READ:
                    try {
                        String info = (String) msg.obj;
                        Log.e(TAG, "info:======" + info);
                    } catch (Exception e) {
                        Log.e("MESSAGE_READ", e.getMessage());
                    }
                    break;
                case MESSAGE_WRITE:
                    break;

            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connectbt);
        //首先先创建一个BTservice，作为蓝牙通信服务
        mBTService = new BlueToothService(this, mhandler);// 创建对象的时候必须有一个是Handler类型

        if (!mBTService.IsOpen()) {
            mBTService.OpenDevice();
        }

        //接受到蓝牙的时候
        mBTService.setOnReceive(new BlueToothService.OnReceiveDataHandleEvent() {

            @Override
            public void OnReceive(BluetoothDevice device) {
                // TODO Auto-generated method stub
                if (device != null) {
                    if (dialog != null) {
                        dialog.addDeviceForAdapter(device.getName() + "\n" + device.getAddress());
                    }
                } else {
                    Message msg = new Message();
                    msg.what = 1;
                    handler.sendMessage(msg);
                }
            }
        });
        //按下搜索设备按钮开始进行
        findViewById(R.id.btnSearch).setOnClickListener(
                new View.OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        // TODO Auto-generated method stub
                        showDialog() ;
                    }
                });
    }

}
