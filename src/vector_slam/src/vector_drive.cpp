#include "vector_slam/vector_drive.hpp"
#include <pluginlib/class_list_macros.hpp>

namespace vector_slam
{

void VectorDriver::init(webots_ros2_driver::WebotsNode *node, std::unordered_map<std::string, std::string> &parameters)
{
  node_ = node;
  time_step_ = int(wb_robot_get_basic_time_step());

  // Get and enable the LiDAR device using the Webots C API
  lidar_ = wb_robot_get_device("laser_frame");
  if (lidar_) {
    wb_lidar_enable(lidar_, time_step_);
    wb_lidar_enable_point_cloud(lidar_);
  } else {
    RCLCPP_WARN(node_->get_logger(), "LiDAR device 'laser_frame' not found in Webots model!");
  }

  // Create publisher for /scan
  lidar_publisher_ = node_->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);
}

void VectorDriver::step()
{
  if (!lidar_) {
    return;
  }

  auto scan_msg = sensor_msgs::msg::LaserScan();
  scan_msg.header.stamp = node_->now();
  scan_msg.header.frame_id = "laser_frame";

  // Correct Webots C API functions for FoV and Range
  double fov = wb_lidar_get_fov(lidar_); // Total horizontal Field of View
  scan_msg.angle_min = -fov / 2.0;
  scan_msg.angle_max = fov / 2.0;

  int horizontal_resolution = wb_lidar_get_horizontal_resolution(lidar_);
  scan_msg.angle_increment = horizontal_resolution > 0 ? fov / horizontal_resolution : 0.0;
  scan_msg.time_increment = 0.0;
  scan_msg.scan_time = (double)time_step_ / 1000.0;
  scan_msg.range_min = wb_lidar_get_min_range(lidar_);
  scan_msg.range_max = wb_lidar_get_max_range(lidar_);

  const float *range_image = wb_lidar_get_range_image(lidar_);
  if (range_image && horizontal_resolution > 0) {
    scan_msg.ranges.assign(range_image, range_image + horizontal_resolution);
    lidar_publisher_->publish(scan_msg);
  }
}

}  // namespace vector_slam

PLUGINLIB_EXPORT_CLASS(vector_slam::VectorDriver, webots_ros2_driver::PluginInterface)