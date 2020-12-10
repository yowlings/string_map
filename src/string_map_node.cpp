#include "nav_msgs/OccupancyGrid.h"
#include "robot_msg/StringOccupancyGrid.h"
#include "ros/ros.h"
#include "std_msgs/String.h"

ros::Publisher string_map_pub_;
ros::WallTimer pub_timer;
int rate;
bool pub_with_rate;
robot_msg::StringOccupancyGrid string_msg;
void pubTimerCallback(const ros::WallTimerEvent& wall_timer) {
  string_msg.header.stamp = ros::Time::now();
  string_map_pub_.publish(string_msg);
}
void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg) {
  string_msg.header = msg->header;
  string_msg.info = msg->info;
  std::vector<uint8_t> v;
  for (unsigned int i = 1; i < msg->data.size(); i++) {
    if (msg->data[i] == -1) {
      v.push_back(42);
    } else if (msg->data[i] == 0) {
      v.push_back(97);
    } else {
      v.push_back(msg->data[i]);
    }
  }
  string_msg.data.clear();
  string_msg.data.assign(v.begin(), v.end());
  if (!pub_with_rate) {
    string_map_pub_.publish(string_msg);
  }
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "string_map");
  ros::NodeHandle nh("~");
  nh.param("pub_with_rate", pub_with_rate, false);
  nh.param("rate", rate, 1);
//  ROS_INFO_STREAM("pub with rate: "<<pub_with_rate<<"\n Rate: "<<rate<<" Hz!");
  string_map_pub_ =
      nh.advertise<robot_msg::StringOccupancyGrid>("/string_map", 1, true);
  ros::Subscriber map_sub = nh.subscribe("/map", 1000, mapCallback);
  if (pub_with_rate) {
    pub_timer =
        nh.createWallTimer(ros::WallDuration(1.0 / rate), &pubTimerCallback);
    ROS_INFO_STREAM("Publish string map topic with rate: "<<rate<<" Hz!");
  }
  ros::spin();

  return 0;
}
