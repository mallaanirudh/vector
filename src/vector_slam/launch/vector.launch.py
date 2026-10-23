import os
import launch
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from webots_ros2_driver.webots_launcher import WebotsLauncher
from webots_ros2_driver.webots_controller import WebotsController
from webots_ros2_driver.urdf_spawner import URDFSpawner

def generate_launch_description():
    package_dir = get_package_share_directory('vector_slam')
    world_path = os.path.join(package_dir, 'worlds', 'vector3.wbt')
    urdf_path = os.path.join(package_dir, 'urdf', 'vector.urdf')
    controllers_path = os.path.join(package_dir, 'config', 'ros2_controllers.yaml')

    # 1. Start the Webots simulation and enable the ROS 2 Supervisor
    webots = WebotsLauncher(
        world=world_path,
        ros2_supervisor=True
    )

    # 2. Webots Controller node (must load ros2_controllers.yaml so controller_manager starts)
    vector_driver = WebotsController(
        robot_name='vector',
        parameters=[
            {'robot_description': urdf_path},
            {'use_sim_time': True},
            controllers_path
        ],
        output='screen'
    )

    # 3. Spawn the URDF model inside Webots via the supervisor service
    urdf_spawner = URDFSpawner(
        name='vector',
        urdf_path=urdf_path,
        relative_path_prefix=os.path.join(package_dir, 'urdf'),
        translation='0 0 0.1',
        rotation='0 0 1 0'
    )

    # 4. Robot State Publisher for handling TF/transforms
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[
            {'robot_description': open(urdf_path).read()},
            {'use_sim_time': True}
        ]
    )

    # 5. Controller Manager Spawners
    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster', '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}]
    )

    diff_drive_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['diff_drive_controller', '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}]
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    return LaunchDescription([
        webots,
        webots._supervisor,
        vector_driver,
        urdf_spawner,
        robot_state_publisher,
        rviz_node,
        # Some deadlock error am delaying this by 5 sec
        launch.actions.TimerAction(
            period=5.0,
            actions=[
                joint_state_broadcaster_spawner,
                diff_drive_spawner
            ]
        ),
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=webots,
                on_exit=[launch.actions.EmitEvent(event=launch.events.Shutdown())],
            )
        )
    ])