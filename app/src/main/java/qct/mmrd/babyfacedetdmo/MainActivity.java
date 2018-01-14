package qct.mmrd.babyfacedetdmo;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

/**
 * Created by 59732 on 2018/1/13.
 */

public class MainActivity extends Activity {

    public static BlueToothService mBTService;
    public static boolean isConnected;
    private boolean startSend=true;
    private TextView txconnect,txPowerStatus,txStatus;
    private Button Btup, Btdown, Btnishizheng, Btshunshizheng, Btright, Btleft;
    private String sendString;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButtonListener buttonListener = new ButtonListener();
        txconnect= (TextView) findViewById(R.id.Txconnect);
        if (isConnected==true)
        {
            txconnect.setText(R.string.Text_is_connect);
        }else {
            txconnect.setText(R.string.Text_not_connect);
        }
        Btup= (Button) findViewById(R.id.Bt_up);
        Btdown= (Button) findViewById(R.id.Bt_down);
        Btnishizheng= (Button) findViewById(R.id.Bt_nishizheng);
        Btshunshizheng= (Button) findViewById(R.id.Bt_shunshizheng);
        Btright= (Button) findViewById(R.id.Bt_right);
        Btleft= (Button) findViewById(R.id.Bt_left);
        Btup.setOnTouchListener(buttonListener);
        Btdown.setOnTouchListener(buttonListener);
        Btnishizheng.setOnTouchListener(buttonListener);
        Btshunshizheng.setOnTouchListener(buttonListener);
        Btleft.setOnTouchListener(buttonListener);
        Btright.setOnTouchListener(buttonListener);
    }

    class ButtonListener implements View.OnClickListener, View.OnTouchListener {

        public void onClick(View v) {

        }

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch (v.getId()) {
                case R.id.Bt_up:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "A";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                case R.id.Bt_down:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "E";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                case R.id.Bt_right:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "JC";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                case R.id.Bt_left:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "JG";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                case R.id.Bt_shunshizheng:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "KG";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                case R.id.Bt_nishizheng:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        sendString= "KC";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        sendString= "Z";
                        if(isConnected && startSend){
                            mBTService.write(sendString.getBytes());
                        }
                    }
                    break;
                default:
                    break;
            }
            return false;
        }
    }

}
