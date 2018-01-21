package qct.mmrd.babyfacedetdmo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
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
    private ImageView ivguihua;
    private byte[] sendBufeer={(byte)0xFF,(byte)0xFE,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
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
        ivguihua= (ImageView) findViewById(R.id.Iv_guihua);
        Btup.setOnTouchListener(buttonListener);
        Btdown.setOnTouchListener(buttonListener);
        Btnishizheng.setOnTouchListener(buttonListener);
        Btshunshizheng.setOnTouchListener(buttonListener);
        Btleft.setOnTouchListener(buttonListener);
        Btright.setOnTouchListener(buttonListener);
        ivguihua.setOnClickListener(buttonListener);
        if(isConnected && startSend){
            mBTService.write(sendBufeer);
        }
    }

    class ButtonListener implements View.OnClickListener, View.OnTouchListener {

        public void onClick(View v) {
            switch (v.getId())
            {
                case R.id.Iv_guihua:
                    Intent intent=new Intent();
                    intent.setClass(MainActivity.this, BFDMainActivity.class); //设置跳转的Activity
                    BFDMainActivity.mBTService=mBTService;
                    BFDMainActivity.isConnected=isConnected;
                    MainActivity.this.startActivity(intent);
                    break;
                default:
                    break;
            }
        }

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch (v.getId()) {
                case R.id.Bt_up:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,0,50,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                case R.id.Bt_down:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,0,-50,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                case R.id.Bt_right:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,50,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                case R.id.Bt_left:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,-50,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                case R.id.Bt_shunshizheng:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,0,0,-50);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                case R.id.Bt_nishizheng:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        int2byte(sendBufeer,0,0,50);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        int2byte(sendBufeer,0,0,0);
                        if(isConnected && startSend){
                            mBTService.write(sendBufeer);
                        }
                    }
                    break;
                default:
                    break;
            }
            return false;
        }
    }
    @Override
    public void onBackPressed() {
        super.onBackPressed();
        finish();
    }

    private void int2byte (byte[] in,int x,int y,int z)
    {
        if(x>=0)
        {
            in[9]&=0xFB;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFB;
            in[9]|=0x04;
            x=-x;
        }
        in[3]=(byte) ((x&0xFF00)>>8);
        in[4]=(byte)((x&0x00FF));
        if(y>=0)
        {
            in[9]&=0xFD;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFB;
            in[9]|=0x02;
            y=-y;
        }
        in[5]=(byte) ((y&0xFF00)>>8);
        in[6]=(byte)((y&0x00FF));
        if(z>=0)
        {
            in[9]&=0xFE;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFE;
            in[9]|=0x01;
            z=-z;
        }
        in[7]=(byte) ((z&0xFF00)>>8);
        in[8]=(byte)((z&0x00FF));
    }
}
