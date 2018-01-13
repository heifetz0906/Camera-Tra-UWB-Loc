package qct.mmrd.babyfacedetdmo;

import android.app.Dialog;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.Set;

/**
 *
 */
public class ConnectBlueToothDialog extends Dialog{

	private BlueToothService mBTService ;
	
	private ListView deviceList ;

	private BFDMainActivity bfdactivity ;

	private ConnectBluet activity ;

	private Set<BluetoothDevice> devices;
	
	private ArrayAdapter<String> mPairedDevicesArrayAdapter = null;//
	private ArrayAdapter<String> mNewDevicesArrayAdapter = null;//
	
	
	public ConnectBlueToothDialog(Context context, int theme) {
		super(context, theme);
		activity = (ConnectBluet) context;
	}

	public BlueToothService getmBTService() {
		return mBTService;
	}

	public void setmBTService(BlueToothService mBTService) {
		this.mBTService = mBTService;
	}
	
	protected void onCreate(android.os.Bundle savedInstanceState) {
		super.onCreate(savedInstanceState) ;
		setContentView(R.layout.device_dialog) ;
		deviceList = (ListView) findViewById(R.id.connect_device_dialog_listview) ;
		mPairedDevicesArrayAdapter = new ArrayAdapter<String>(activity,
				R.layout.device_name);
		mNewDevicesArrayAdapter = new ArrayAdapter<String>(activity,
				R.layout.device_name);
		deviceList.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
				// 鑾峰彇钃濈墮鐗╃悊鍦板潃
				if (!mBTService.IsOpen()) {// 鍒ゆ柇钃濈墮鏄惁鎵撳紑
					mBTService.OpenDevice();
					return;
				}
				if (mBTService.GetScanState() == mBTService.STATE_SCANING) {
					Message msg = new Message();
					msg.what = 2;
					activity.getHandler().sendMessage(msg);
				}
				if (mBTService.getState() == mBTService.STATE_CONNECTING) {
					return;
				}

				String info = ((TextView) arg1).getText().toString();
				String address = info.substring(info.length() - 17);
				mBTService.DisConnected();
				mBTService.ConnectToDevice(address);// 杩炴帴钃濈墮
//				SharedPreferences spf = activity.getSharedPreferences("address", 0) ;
//				Editor editor = spf.edit() ;
//				editor.putString("lastConnectAdress", address) ;
//				editor.commit() ;
			}
		}) ;
		scanDevice() ;
	};
	
   


	public void scanDevice(){
		// 开始扫描设备

		if (!mBTService.IsOpen()) {//如果蓝牙未被打开
			mBTService.OpenDevice();
			return;
		}
		if (mBTService.GetScanState() == mBTService.STATE_SCANING)
			return;
		
		mNewDevicesArrayAdapter.clear();
		//鑾峰彇宸查厤瀵圭殑璁惧
		devices = mBTService.GetBondedDevice();

		if (devices.size() > 0) {

			for (BluetoothDevice device : devices) {
				mNewDevicesArrayAdapter.add(device.getName() + "\n"
						+ device.getAddress());
			}
		}
		deviceList.setAdapter(mNewDevicesArrayAdapter);
		//寮�濮嬫壂鎻忛檮杩戣摑鐗欒澶�
		new Thread() {
			public void run() {
				mBTService.ScanDevice();
			}
		}.start();
	}
	
	public void addDeviceForAdapter(String deviceInfo){
		mNewDevicesArrayAdapter.add(deviceInfo) ;
	}
}
