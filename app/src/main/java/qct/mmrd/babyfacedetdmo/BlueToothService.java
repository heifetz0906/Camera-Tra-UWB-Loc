package qct.mmrd.babyfacedetdmo;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.os.Handler;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Set;
import java.util.UUID;

//钃濈墮宸ュ叿绫�
public class BlueToothService {

	private BluetoothAdapter adapter;
	private Context context;
	private int mState;
	private int scanState = 1;
	private Boolean D = true;
	private String TAG = "BlueToothService";
	private AcceptThread mAcceptThread;
	private ConnectThread mConnectThread;
	private ConnectedThread mConnectedThread;

	// Constants that indicate the current connection state
	public static final int STATE_NONE = 0; // we're doing nothing
	public static final int STATE_LISTEN = 1; // now listening for incoming connections
	public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
	public static final int STATE_CONNECTED = 3; // now connected to a remote device
	public static final int LOSE_CONNECT = 4;
	public static final int FAILED_CONNECT = 5;
	public static final int SUCCESS_CONNECT = 6; // now connected to a remote

	public static final int MESSAGE_STATE_CHANGE = 1;
	public static final int MESSAGE_READ = 2;
	public static final int MESSAGE_WRITE = 3;
	public static final int STATE_SCANING = 0;// 鎵弿态
	public static final int STATE_SCAN_STOP = 1;

	private static final int WRITE_READ = 2;
	private static final int WRITE_WAIT = 3;
	private static int writeState = 2;
	public static int times = 0;

	private void SetWriteState(int state) {
		synchronized (this) {
			writeState = state;
		}
	}

	private static final UUID MY_UUID = UUID
			.fromString("00001101-0000-1000-8000-00805F9B34FB");
	private static final String NAME = "BlueToothService";

	private Handler mHandler;

	public BlueToothService(Context context, Handler handler) {
		this.context = context;
		this.mHandler = handler;
		mState = STATE_NONE;
		adapter = BluetoothAdapter.getDefaultAdapter();
	}

	public boolean HasDevice() {
		if (adapter != null) {
			return true;
		}
		return false;

	}

	public boolean IsOpen() {
		synchronized (this) {
			if (adapter.isEnabled()) {
				return true;
			}
			return false;
		}
	}

	public void OpenDevice() {
		Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		context.startActivity(intent);

	}

	public void CloseDevice() {
		adapter.disable();
	}

	public Set<BluetoothDevice> GetBondedDevice() {

		Set<BluetoothDevice> devices = adapter.getBondedDevices();
		return devices;
	}

	//鎵弿璁惧
	public void ScanDevice() {
		IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		context.registerReceiver(mReceiver, filter);

		filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		context.registerReceiver(mReceiver, filter);
		if (adapter.isDiscovering()) {
			adapter.cancelDiscovery();
		}
		SetScanState(BlueToothService.STATE_SCANING);
		// Request discover from BluetoothAdapter
		adapter.startDiscovery();
	}

	//鍋滄鎵弿
	public void StopScan() {

	
		adapter.cancelDiscovery();
		SetScanState(BlueToothService.STATE_SCAN_STOP);
		
		try{
			context.unregisterReceiver(mReceiver);
		}catch(Throwable e){
              e.printStackTrace() ;			
		}
	}

	public OnReceiveDataHandleEvent OnReceive = null;

	public interface OnReceiveDataHandleEvent {
		public void OnReceive(BluetoothDevice device);
	}

	public OnReceiveDataHandleEvent getOnReceive() {
		return OnReceive;
	}

	public void setOnReceive(OnReceiveDataHandleEvent onReceive) {
		OnReceive = onReceive;
	}

	private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();

			// When discovery finds a device
			if (BluetoothDevice.ACTION_FOUND.equals(action)) {
				// Get the BluetoothDevice object from the Intent
				BluetoothDevice device = intent
						.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
				// If it's already paired, skip it, because it's been listed
				// already
				if (device.getBondState() != BluetoothDevice.BOND_BONDED) {
					SetScanState(BlueToothService.STATE_SCANING);
					OnReceive.OnReceive(device);
				}
				// When discovery is finished, change the Activity title
			} else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED
					.equals(action)) {
				SetScanState(BlueToothService.STATE_SCAN_STOP);

				OnReceive.OnReceive(null);
			}
		}

		private void OnFinished() {
			// TODO Auto-generated method stub

		}
	};
	public static String EXTRA_DEVICE_ADDRESS = "device_address";

	public void ConnectToDevice(String address) {
		if (BluetoothAdapter.checkBluetoothAddress(address)) {
			BluetoothDevice device = adapter.getRemoteDevice(address);
			connect(device);
		}
	}

	public void write(byte[] out) {
		// Create temporary object
		ConnectedThread r;
		// Synchronize a copy of the ConnectedThread
		synchronized (this) {
			if (mState != STATE_CONNECTED)
				return;
			r = mConnectedThread;
		}
		if (r != null) {
			r.write(out);
		} else {
			DisConnected();
			Nopointstart();

		}
	}

	public synchronized void start() {
		if (D)
			Log.d(TAG, "start");

		// Cancel any thread attempting to make a connection
		if (mConnectThread != null) {
			mConnectThread.cancel();
			mConnectThread = null;
		}

		// Cancel any thread currently running a connection
		if (mConnectedThread != null) {
			mConnectedThread.cancel();
			mConnectedThread = null;
		}

		// Start the thread to listen on a BluetoothServerSocket
		if (mAcceptThread == null) {
			mAcceptThread = new AcceptThread();
			mAcceptThread.start();
		}
		setState(STATE_LISTEN);
	}

	private synchronized void setState(int state) {
		mState = state;
	}

	public synchronized int getState() {
		return mState;

	}

	private synchronized void SetScanState(int state) {
		scanState = state;
	}

	public synchronized int GetScanState() {
		return scanState;

	}

	public synchronized void connect(BluetoothDevice device) {

		if (mState == STATE_CONNECTING) {
			if (mConnectThread != null) {
				mConnectThread.cancel();
				mConnectThread = null;
			}
		}
		if (mConnectedThread != null) {
			mConnectedThread.cancel();
			mConnectedThread = null;
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		mConnectThread = new ConnectThread(device);
		mConnectThread.start();
		setState(STATE_CONNECTING);
	}

	public synchronized void DisConnected() {

		if (mConnectThread != null) {
			mConnectThread.cancel();
			mConnectThread = null;
		}

		if (mConnectedThread != null) {
			mConnectedThread.cancel();
			mConnectedThread = null;
		}

		if (mAcceptThread != null) {
			mAcceptThread.cancel();
			mAcceptThread = null;
		}

		setState(STATE_NONE);
	}

	public synchronized void connected(BluetoothSocket socket,
			BluetoothDevice device) {

		if (mConnectThread != null) {
			mConnectThread.cancel();
			mConnectThread = null;
		}

		if (mConnectedThread != null) {
			mConnectedThread.cancel();
			mConnectedThread = null;
		}

		if (mAcceptThread != null) {
			mAcceptThread.cancel();
			mAcceptThread = null;
		}

		mConnectedThread = new ConnectedThread(socket);
		mConnectedThread.start();

		setState(STATE_CONNECTED);
	}

	/**
	 * Stop all threads
	 */
	public synchronized void stop() {
		if (D)
			Log.d(TAG, "stop");
		setState(STATE_NONE);
		if (mConnectThread != null) {
			mConnectThread.cancel();
			mConnectThread = null;
		}
		if (mConnectedThread != null) {
			mConnectedThread.cancel();
			mConnectedThread = null;
		}
		if (mAcceptThread != null) {
			mAcceptThread.cancel();
			mAcceptThread = null;
		}
	}

	private void connectionSuccess() {
		setState(STATE_CONNECTED);
		mHandler.obtainMessage(MESSAGE_STATE_CHANGE, SUCCESS_CONNECT, -1)
				.sendToTarget();
	}

	private void connectionFailed() {
		setState(STATE_LISTEN);
		mHandler.obtainMessage(MESSAGE_STATE_CHANGE, FAILED_CONNECT, -1)
				.sendToTarget();
	}

	/**
	 * Indicate that the connection was lost and notify the UI Activity.
	 */
	private void connectionLost() {
		setState(STATE_LISTEN);
		mHandler.obtainMessage(MESSAGE_STATE_CHANGE, LOSE_CONNECT, -1)
				.sendToTarget();

	}

	private void Nopointstart() {
		setState(STATE_LISTEN);
		mHandler.obtainMessage(MESSAGE_STATE_CHANGE, LOSE_CONNECT, 0)
				.sendToTarget();

	}

	/**
	 * This thread runs while listening for incoming connections. It behaves
	 * like a server-side client. It runs until a connection is accepted (or
	 * until cancelled).
	 */
	private class AcceptThread extends Thread {
		// The local server socket
		private final BluetoothServerSocket mmServerSocket;

		public AcceptThread() {
			BluetoothServerSocket tmp = null;

			// Create a new listening server socket
			try {
				tmp = adapter.listenUsingRfcommWithServiceRecord(NAME, MY_UUID);
			} catch (IOException e) {
				Log.e(TAG, "listen() failed", e);
			}
			mmServerSocket = tmp;
		}

		@Override
		public void run() {
			if (D)
				Log.d(TAG, "BEGIN mAcceptThread" + this);
			setName("AcceptThread");
			BluetoothSocket socket = null;

			// Listen to the server socket if we're not connected
			while (mState != STATE_CONNECTED) {
				try {
					socket = mmServerSocket.accept();
				} catch (IOException e) {
					break;
				}

				// If a connection was accepted
				if (socket != null) {
					synchronized (BlueToothService.this) {
						switch (mState) {
						case STATE_LISTEN:
						case STATE_CONNECTING:
							// Situation normal. Start the connected thread.
							connected(socket, socket.getRemoteDevice());
							break;
						case STATE_NONE:
						case STATE_CONNECTED:
							try {
								socket.close();
							} catch (IOException e) {

							}
							break;
						}
					}
				}
			}

		}

		public void cancel() {
			if (D)
				Log.d(TAG, "cancel " + this);
			try {
				mmServerSocket.close();
			} catch (IOException e) {
				Log.e(TAG, "close() of server failed", e);
			}
		}
	}

	/**
	 * This thread runs while attempting to make an outgoing connection with a
	 * device. It runs straight through; the connection either succeeds or
	 * fails.
	 */
	private class ConnectThread extends Thread {
		private final BluetoothSocket mmSocket;
		private final BluetoothDevice mmDevice;

		public ConnectThread(BluetoothDevice device) {
			mmDevice = device;
			BluetoothSocket tmp = null;

			// Get a BluetoothSocket for a connection with the
			// given BluetoothDevice
			try {
				tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
			} catch (IOException e) {
				Log.e(TAG, "create() failed", e);
			}
			mmSocket = tmp;
		}

		@Override
		public void run() {
			Log.i(TAG, "BEGIN mConnectThread");
			setName("ConnectThread");

			// Always cancel discovery because it will slow down a connection
			adapter.cancelDiscovery();
			SetScanState(STATE_SCAN_STOP);

			// Make a connection to the BluetoothSocket
			try {
				// This is a blocking call and will only return on a
				// successful connection or an exception
				mmSocket.connect();
				connectionSuccess();
			} catch (IOException e) {
				e.printStackTrace() ;
				connectionFailed();
				// Close the socket
				try {
					mmSocket.close();
				} catch (IOException e2) {
					Log.e(TAG,
							"unable to close() socket during connection failure",
							e2);
				}
				// Start the service over to restart listening mode
				BlueToothService.this.start();
				return;
			}

			// Reset the ConnectThread because we're done
			synchronized (BlueToothService.this) {
				mConnectThread = null;
			}

			// Start the connected thread
			connected(mmSocket, mmDevice);
		}

		public void cancel() {
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e(TAG, "close() of connect socket failed", e);
			}
		}
	}


	private class ConnectedThread extends Thread {
		private final BluetoothSocket mmSocket;
		private final InputStream mmInStream;
		private final OutputStream mmOutStream;
		private boolean isCancle = false;

		public ConnectedThread(BluetoothSocket socket) {
			Log.d(TAG, "create ConnectedThread");
			mmSocket = socket;
			InputStream tmpIn = null;
			OutputStream tmpOut = null;
			isCancle = false;
			// Get the BluetoothSocket input and output streams
			try {
				tmpIn = socket.getInputStream();
				tmpOut = socket.getOutputStream();
			} catch (IOException e) {
				Log.e(TAG, "temp sockets not created", e);
			}

			mmInStream = tmpIn;
			mmOutStream = tmpOut;
		}

		@Override
		public void run() {
			Log.i(TAG, "BEGIN mConnectedThread");
			int bytes;

			// Keep listening to the InputStream while connected
			while (!isCancle) {
				try {
					// Send the obtained bytes to the UI Activity
					BufferedReader br = new BufferedReader(new InputStreamReader(mmInStream)) ;
//					String line;  
					 char[] b = new char[20] ;
//	                 br.read(b) ;
//	                 String msg = String.valueOf(b) ;
					String msg = br.readLine() ;
	                 if(!msg.trim().equals("")){
	                	 SetWriteState(WRITE_READ);
							mHandler.obtainMessage(MESSAGE_READ, 0, -1,
									msg.trim())
									 .sendToTarget();
	                 }
//					while((line=br.readLine()) != null)  
//					{  
//						Log.e("bufferreader", line+"<<<<") ;
//						SetWriteState(WRITE_READ);
//						mHandler.obtainMessage(MESSAGE_READ, 0, -1,
//								line)
//								 .sendToTarget();
//					} 
					
//					byte[] buffer = new byte[10];
//					// Read from the InputStream
//					bytes = mmInStream.read(buffer);
//					if (bytes > 0) {
//					
//
//					} else {
//						Log.e(TAG, "disconnected1");
//						connectionLost();
//						isCancle = true;
//						// if (mState != STATE_NONE) {
//						// Log.e(TAG, "disconnected");
//						// // Start the service over to restart listening mode
//						// BlueToothService.this.start();
//						// }
//						break;
//					}
				} catch (IOException e) {
					Log.e(TAG, "disconnected2", e);
					connectionLost();
					isCancle = true;
					// add by chongqing jinou
					// if (mState != STATE_NONE) {
					// // Start the service over to restart listening mode
					// BlueToothService.this.start();
					// }
					break;
				}
			}
		}

		public void write(byte[] buffer) {
			try {
				mmOutStream.write(buffer);
				Log.i("BTPWRITE", new String(buffer, "GBK"));
				// Share the sent message back to the UI Activity
				mHandler.obtainMessage(MESSAGE_WRITE, -1, -1, buffer)
						.sendToTarget();
			} catch (IOException e) {

			}
		}

		public void cancel() {
			try {
				isCancle = true;
				mmSocket.close();
				Log.d(TAG, "562cancel suc");
				setState(STATE_LISTEN);
			} catch (IOException e) {
				Log.d(TAG, "565cancel failed");
			}
		}
	}

	
}
