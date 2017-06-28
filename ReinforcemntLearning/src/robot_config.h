/*
 *  Author:         Erhard Wieser
 *
    Description:    Robot-specific parameters: NAO robot.
*/

// NAO robot-specific h-files for the joint msgs.
#ifndef robot_config
#define robot_config
#include <robot_specific_msgs/JointState.h>
#include <robot_specific_msgs/JointAnglesWithSpeed.h>
#include <robot_specific_msgs/TactileTouch.h>
#include <robot_specific_msgs/Bumper.h>

#define ROBOT_DOF       26	// robot-specific; NAO robot has 26 DOF in total
#define HEAD_DOF        2
#define L_ARM_DOF 	6
#define L_LEG_DOF       6
#define R_LEG_DOF	6
#define R_ARM_DOF	6

extern double hip_roll;
extern double* dp_hip_roll;
#endif


