package qct.mmrd.babyfacedetdmo;

import static android.R.attr.x;
import static java.lang.Math.abs;

/**
 * Created by 59732 on 2018/1/18.
 */

public class FollowOBJ {
    private int x;//距离左边边沿的位置
    private int y;//距离上边沿的位置
    private int width;//方框的宽度
    private int height;//方框的高度
    private int now_x;//距离左边边沿的位置
    private int now_y;//距离上边沿的位置
    private int now_width;//方框的宽度
    private int now_height;//方框的高度
    public int x_error;//距离左边边沿的位置
    public int y_error;//距离上边沿的位置
    public int w_error;//方框的宽度
    public int h_error;//方框的高度
    private boolean loose=false;
    public FollowOBJ(int x, int y, int width, int height) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    public String calculate()
    {
        StringBuilder send=new StringBuilder();
        if(now_x==0&&now_y==0&&now_width==0&&now_height==0)
        {
            if(loose==false) {
                loose = true;
            send.append("K");
            }
            send.append("G");//如果丢失目标，就顺时针旋转寻找目标
            return send.toString();
        }else {
            if(loose==true)
            {
                loose=false;
                send.append("J");
            }
            if((abs(x_error)<15)&&(abs(y_error)<15)&&(abs(w_error)<15)&&(abs(h_error)<15))
            {
                send.append("Z");//差距不大时停止
                return send.toString();
            }else
            {
               if(x_error<-15&&y_error<-15)//x误差小于-15 y误差小于-15，方块位于左上方，小车应该向左后方移动。
               {
                   send.append("F");
                   return send.toString();
               }else if(x_error<-15&&y_error>15)//x误差小于-15 y误差大于15，方块位于左下方，小车应该向左前方移动。
               {
                   send.append("H");
                   return send.toString();
               }else if (x_error>15&&y_error<-15)//x误差大于15 y误差小于-15，方块位于右上方，小车应该向右后方移动。
               {
                   send.append("D");
                   return send.toString();
               }else if (x_error>15&&y_error>15)//x误差大于15 y误差大于15，方块位于右下方，小车应该向右前方移动。
               {
                   send.append("B");
                   return send.toString();
               }
            }
        }
        return send.toString();
    }

    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public void setNow_x(int now_x) {
        this.now_x = now_x;
        this.x_error=now_x-x;
    }

    public void setNow_y(int now_y) {
        this.now_y = now_y;
        this.y_error=now_y-y;
    }

    public void setNow_width(int now_width) {
        this.now_width = now_width;
        this.w_error=now_width-width;
    }

    public void setNow_height(int now_height) {
        this.now_height = now_height;
        this.h_error=now_height-height;
    }
}

