#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <visualization_msgs/Marker.h>

void dataCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
	// Get the range of the laser scanner
    std::vector<float> laser_scan_range;
    laser_scan_range = msg->ranges;
    float angle_increment = msg->angle_increment;
    float angle_min = msg->angle_min;
    float radian_min;
    float x_min_pos, y_min_pos, z_min_pos;

    float min_distance = 10000;
    float min_index = 0;

    float p_gain,zangle,gain;

    ros::NodeHandle nh("~");

    if(!(nh.getParam("/exercise4/p_gain",p_gain))){
	ROS_ERROR("P_GAIN ERROR");
    }

    ros::Publisher publisher =
	nh.advertise<geometry_msgs::Twist>("/cmd_vel",10);

    ros::Publisher visual = 
	nh.advertise<visualization_msgs::Marker>("/visualization_marker", 0);

    // Get minimum distance
    for(int i = 0; i < laser_scan_range.size(); i++){
			if(min_distance > laser_scan_range[i]) {
					min_distance = laser_scan_range[i];
					min_index = i;
			}
		}

		//ROS_INFO_STREAM("Minimum distance: " + std::to_string(min_distance));
    //start here
    radian_min = angle_min + angle_increment*min_index;
    x_min_pos = min_distance*cos(radian_min);
    y_min_pos = min_distance*sin(radian_min);
    z_min_pos = 1.0;

//    ROS_INFO("x: %f, y: %f\n",x_min_pos, y_min_pos);

    zangle = -radian_min;
    if(x_min_pos > 1){
	gain = p_gain*min_distance;
    }else{
	gain = 1;
    }

    geometry_msgs::Twist velo_comd;
    velo_comd.linear.x = 1.0*gain;
    velo_comd.angular.z = zangle;
    ROS_INFO("data: %f",gain);

    // Publish visualization marker
    visualization_msgs::Marker marker;
    marker.header.frame_id = "base_laser";
    marker.header.stamp = ros::Time();
    marker.ns = "base_laser";
    marker.id = 0;
    marker.type = visualization_msgs::Marker::CUBE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = x_min_pos;
    marker.pose.position.y = y_min_pos;
    marker.pose.position.z = z_min_pos;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 1;
    marker.scale.y = 1;
    marker.scale.z = 1;
    marker.color.a = 1.0; // Don't forget to set the alpha!
    marker.color.r = 0.0;
    marker.color.g = 1.0;
    marker.color.b = 0.0;

    //Only if using a MESH_RESOURCE marker type:
    marker.mesh_resource = "package:///exercise3/dae/base_link.dae";
    ros::Rate loop_rate(10);
    while(ros::ok()){
	visual.publish(marker);
	publisher.publish(velo_comd);
	ros::spin();
	loop_rate.sleep();
    }

}

int main(int argc, char** argv){
	ros::init(argc, argv, "exercise4");

	ros::NodeHandle nodeHandle("~");

	std::string topiq;
	int queque;

	if (!(nodeHandle.getParam("/exercise4/topic_name", topiq) & 
	      nodeHandle.getParam("/exercise4/queue_size", queque))) {
            ROS_ERROR("Could not load parameters.");
            ros::requestShutdown();
    }


	ros::Subscriber subcriber =
		nodeHandle.subscribe(topiq,queque,dataCallback);

	ros::spin();
	return 0;

}
