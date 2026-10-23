#ifndef VECTOR_SLAM__VECTOR_DRIVER_HPP_
#define VECTOR_SLAM__VECTOR_DRIVER_HPP_

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <webots_ros2_driver/PluginInterface.hpp>
#include <webots_ros2_driver/WebotsNode.hpp>

// Include Webots C API headers
#include <webots/robot.h>
#include <webots/lidar.h>

namespace vector_slam
{

class VectorDriver : public webots_ros2_driver::PluginInterface
{
public:
  void init(webots_ros2_driver::WebotsNode *node, std::unordered_map<std::string, std::string> &parameters) override;
  void step() override;

private:
  WbDeviceTag lidar_;
  int time_step_;
  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr lidar_publisher_;
  webots_ros2_driver::WebotsNode *node_;
};

}  // namespace vector_slam

#endif  // VECTOR_SLAM__VECTOR_DRIVER_HPP_