package qct.mmrd.babyfacedetdmo;

import android.view.MotionEvent;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

public class TouchEventHelper {

	class Coor {
		float x;
		float y;
	}

	private Coor startCoor = new Coor();
	private Coor endCoor = new Coor();

	private View touchFrame;
	
	//鍒嗗埆鏄痻,y,width,height
	private int[] layoutArray = new int[4] ;

	public TouchEventHelper(View view) {
		touchFrame = view;
	}
	
	public int[] getLayoutArray(){
		return layoutArray ;
	}

	public void dispatchTouchEvent(MotionEvent ev) {

		switch (ev.getAction()) {
		case MotionEvent.ACTION_DOWN:
			clearView() ;
			
			startCoor.x = ev.getX();
			startCoor.y = ev.getY();
			break;

		case MotionEvent.ACTION_MOVE:
			int offx = (int) (ev.getX() - startCoor.x) ;
			int offy = (int) (ev.getY() - startCoor.y);
			if(offx % 10 == 0 || offy % 10 == 0 ){
				endCoor.x = ev.getX() ;
				endCoor.y = ev.getY() ;
				calculateViewPosition() ;
			}
			break;

		case MotionEvent.ACTION_UP:
			if(BFDMainActivity.isInit == 0){
				endCoor.x = ev.getX() ;
				endCoor.y = ev.getY() ;
				calculateViewPosition() ;
				BFDMainActivity.isInit = 1 ;
			}
			break;

		default:
			break;
		}

	}
	
	private void calculateViewPosition(){
		RelativeLayout.LayoutParams rl = (LayoutParams) touchFrame.getLayoutParams() ;
		rl.leftMargin = (int) Math.min(startCoor.x, endCoor.x)  ;
		rl.topMargin = (int) Math.min(startCoor.y, endCoor.y) ;
		rl.width =  Math.abs((int)(startCoor.x - endCoor.x)) ;
		rl.height = Math.abs((int)(startCoor.y - endCoor.y))  ;
		
		copyToArray(rl) ;
		touchFrame.setLayoutParams(rl) ;
	}
	
	private void clearView(){
		RelativeLayout.LayoutParams rl = (LayoutParams) touchFrame.getLayoutParams() ;
		rl.leftMargin = 0 ;
		rl.topMargin = 0 ;
		rl.width = 0 ;
		rl.height = 0 ;
		
		copyToArray(rl) ;
		touchFrame.setLayoutParams(rl) ;
	}
	
	private void copyToArray(RelativeLayout.LayoutParams rl){
		layoutArray[0] = rl.leftMargin ;
		layoutArray[1] = rl.topMargin ;
		layoutArray[2] = rl.width ;
		layoutArray[3] = rl.height ;
	}
}
