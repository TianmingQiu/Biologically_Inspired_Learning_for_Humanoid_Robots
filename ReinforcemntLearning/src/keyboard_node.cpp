/*
    Author:         Erhard Wieser
    Description:    This node serves as an interface node, it publishes input from the keyboard.
*/

#include "ros/ros.h"
#include "std_msgs/String.h"

#include <stdio.h>
#include <sstream>


int main(int argc, char **argv)
{
	ros::init(argc, argv, "keyboard_node");

	ros::NodeHandle n;
	ros::Publisher pub;

	pub = n.advertise<std_msgs::String>("key", 10);

	ros::Rate loop_rate(10);     // publish with 10 Hz

	while (ros::ok())
	{
		// check keyboard	
		std_msgs::String ch;
		ch.data = getchar();		

		pub.publish(ch); 	  // publish the keyboard input
		
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}

