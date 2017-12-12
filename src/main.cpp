#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ros/ros.h>

#define FRAME_RATE 10

#define NORMAL_MODE    //データの収集モードのときコメントアウト

#ifndef NORMAL_MODE
int main(int argc, char **argv) {
  ros::Rate rate(FRAME_RATE);
  while(ros::ok()){
    ros::spinOnece();
    rate.sleep();
    //
  }
}

#else
//技、ポケモンリストのインターネット経由で取得し保存
//データ元：https://yakkun.com/sm/zukan/
int main(int argc, char **argv) {
  //Unimplemented
  return 0;
}
#endif
