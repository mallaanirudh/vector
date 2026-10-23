cmake_minimum_required(VERSION 3.8)
project(vector_slam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pluginlib REQUIRED)
find_package(webots_ros2_driver REQUIRED)

find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# Webots driver plugin library
add_library(vector_driver SHARED
  src/vector_drive.cpp
)

target_include_directories(vector_driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(vector_driver
  rclcpp
  pluginlib
  webots_ros2_driver
  geometry_msgs
  sensor_msgs
  nav_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
)

# Export the Webots plugin description
pluginlib_export_plugin_description_file(
  webots_ros2_driver
  vector_driver.xml
)

# Install headers
install(
  DIRECTORY include/
  DESTINATION include
)

# Install plugin library
install(
  TARGETS vector_driver
  LIBRARY DESTINATION lib/${PROJECT_NAME}
)

# Install robot resources
install(
  DIRECTORY urdf launch worlds config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()