package qct.mmrd.babyfacedetdmo;

import android.app.Activity;
import android.os.Bundle;

/**
 * Created by 59732 on 2018/1/13.
 */

public class MainActivity extends Activity {

    public static BlueToothService mBTService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        String sendString="hello world";
        mBTService.write(sendString.getBytes());
    }
}
