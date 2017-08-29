 /*
 *   Author:      Erhard Wieser, Tianming Qiu, Jiachen Xu, Qiuhai Guo
 *
     Description: A node for sending and receiving sensorimotor data from the NAO robot.
 */


#include <ros/ros.h>

// ROS and OpenCV image processing
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32.h>
#include "std_msgs/String.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/Int16.h"

// own files
#include "robot_config.h"

using namespace std;
using namespace cv;
using namespace sensor_msgs;
using namespace message_filters;

namespace enc = sensor_msgs::image_encodings;
// send Joint initialization
void sendTargetJointStateHead(double dummy[HEAD_DOF]) ;
void sendTargetJointStateLArm(double dummy[L_ARM_DOF]) ;
void sendTargetJointStateRArm(double dummy[R_ARM_DOF]) ;
void sendTargetJointStateLLeg(double dummy[L_LEG_DOF]) ;
void sendTargetJointStateRLeg(double dummy[R_LEG_DOF]) ;
void setStiffness_RHipYawPitch(float value);
void setStiffness_RHipPitch(float value) ;
void setStiffness_RKneePitch(float value) ;
void setStiffness_RAnklePitch(float value) ;
void setStiffness_RAnkleRoll(float value) ;
float value = 0 ;

double HEAD_S1[HEAD_DOF] = {0.194776,0.477728} ;
double L_ARM_S1[L_ARM_DOF] = {1.93893,1.0124,-1.7442,-0.308292,-1.82387,0.6548} ;
double L_LEG_S1[L_LEG_DOF] = {-0.0889301,0.375872,0.135034,0.38806,-0.388144,0.0644701} ;
double R_ARM_S1[R_ARM_DOF] = {2.0464,0.184038,-0.211734,0.335988,1.38976,0.4296} ;
double R_LEG_S1[R_LEG_DOF] = {-0.0889301,-0.10427,0.082794,-0.0889301,0.0614018,0.0859461} ;

double R_LEG_S2[R_LEG_DOF] = {-0.032172,-0.0552659,0.082794,-0.0889301,0.0614018,0.0859461} ;
double R_LEG_S_Roll_ini[R_LEG_DOF] = {-0.032172,-0.0552659,0.082794,-0.0889301,0.0614018,0.0859461} ;
double R_LEG_S_Roll[R_LEG_DOF] = {-0.032172,-0.0552659,0.082794,-0.0889301,0.0614018,0.0859461} ;

double R_LEG_Kick_initial[R_LEG_DOF] = {-0.032172,-0.0552659,0.082794,-0.0889301,0.0614018,0.0859461}  ;
double R_LEG_S_Kick_1[R_LEG_DOF] = {-0.032172,-0.0552659,0.309826,0.4089301,-0.0904641,0.0752079} ;
double R_LEG_S_Kick_2[R_LEG_DOF] = {-0.032172,-0.0552659,-0.360532,-0.0889301,0.0644701,0.0859461} ;


// subscribers to tactile and touch sensors
ros::Subscriber tactile_sub;
ros::Subscriber bumper_sub;
ros::Subscriber robot_action_;

// publisher to robot joints for sending the target joint angles
ros::Publisher target_joint_state_pub;

// joint stiffnesses
ros::Publisher stiffness_pub;


// received motor state of the HEAD
double motor_head_in[HEAD_DOF];
double motor_l_leg_in[L_LEG_DOF];
double motor_r_leg_in[R_LEG_DOF];
double motor_l_arm_in[L_ARM_DOF];
double motor_r_arm_in[R_ARM_DOF];


// set the stiffness

// set the stiffness
void setStiffness_RHipYawPitch(float value)
{
    cout << "setting stiffnesses (RHipYawPitch) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;


    // set stiffnesses ofR HipYawPitch
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("RHipYawPitch");
    target_joint_stiffness.effort.clear();
    target_joint_stiffness.effort.push_back(value);
}

void setStiffness_RHipPitch(float value)
{
    cout << "setting stiffnesses (RHipPitch) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;


    // set stiffnesses of RHipPitch
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("RHipPitch");
    target_joint_stiffness.effort.clear();
    target_joint_stiffness.effort.push_back(value);
    stiffness_pub.publish(target_joint_stiffness);
}

void setStiffness_RKneePitch(float value)
{
    cout << "setting stiffnesses (RKneePitch) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;


    // set stiffnesses of RKneePitch
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("RKneePitch");
    target_joint_stiffness.effort.clear();
    target_joint_stiffness.effort.push_back(value);
    stiffness_pub.publish(target_joint_stiffness);
}

void setStiffness_RAnklePitch(float value)
{
    cout << "setting stiffnesses (RAnklePitch) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;


    // set stiffnesses of RAnklePitch
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("RAnklePitch");
    target_joint_stiffness.effort.clear();
    target_joint_stiffness.effort.push_back(value);
    stiffness_pub.publish(target_joint_stiffness);
}
void setStiffness_RAnkleRoll(float value)
{
    cout << "setting stiffnesses (RAnkleRoll) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;


    // set stiffnesses of RAnkleRoll
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("RAnkleRoll");
    target_joint_stiffness.effort.clear();
    target_joint_stiffness.effort.push_back(value);
    stiffness_pub.publish(target_joint_stiffness);
}



// callback function for tactile buttons (TBs) on the head

void tactileCB(const robot_specific_msgs::TactileTouch::ConstPtr& __tactile_touch)
{

    // check TB 3 (rear)   enable tracking object option
    if (((int)__tactile_touch->button == 3) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;
        R_LEG_S_Roll[1] = R_LEG_S_Roll_ini[1] ;
        sendTargetJointStateHead(HEAD_S1) ;
        sleep(1) ;
        sendTargetJointStateLArm(L_ARM_S1) ;
        sleep(1) ;
        sendTargetJointStateRArm(R_ARM_S1) ;
        sleep(1) ;
        //sendTargetJointStateLLeg(L_LEG_S1) ;
        sendTargetJointStateLLeg(L_LEG_S1) ;
        sleep(1) ;
        sendTargetJointStateRLeg(R_LEG_S1) ;
     }

    // check TB 2 (middle)  tranning neural network and save weights parameter
    if (((int)__tactile_touch->button == 2) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;


    }

    // check TB 1 (front)
    if (((int)__tactile_touch->button == 1) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;


    }

}


// callback function for the head joints

// callback function for the head joints
void jointStateCB(const robot_specific_msgs::JointState::ConstPtr& joint_state)
{
    // buffer for incoming message
    std_msgs::Float32MultiArray buffer;

    // index
    int idx;


    // extract the proprioceptive state of the HEAD
    buffer.data.clear();
        cout << endl << endl << endl << endl << "Head joints:  " << endl  ;
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "HeadYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "HeadPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
    }

    // write data into array
    idx = 0;
    for(vector<float>::const_iterator iter = buffer.data.begin(); iter != buffer.data.end(); ++iter)
    {
        // store into temporary target motor state buffer
        motor_head_in[idx] = *iter;
        idx++;
    }
    cout << endl ;

    // display data on terminal

    for (int i=0; i<HEAD_DOF; i++)
        cout << motor_head_in[i] << ",";
    cout << endl;

    // extract the proprioceptive state of the LEFT ARM
    buffer.data.clear();
    cout << endl << "Left arm joints:  " << endl  ;
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "LShoulderPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LShoulderRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LElbowYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LElbowRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LWristYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LHand")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }

    }

    // write data into array
    idx = 0;
    for(vector<float>::const_iterator iter = buffer.data.begin(); iter != buffer.data.end(); ++iter)
    {
        // store into temporary target motor state buffer
        motor_l_arm_in[idx] = *iter;
        idx++;
    }
    cout << endl ;
    for (int i=0; i<R_LEG_DOF; i++)
        cout << motor_l_arm_in[i] << ",";
    cout << endl;



    // extract the proprioceptive state of the LEFT LEG
    buffer.data.clear();
    cout << endl << "Left leg joints:  " << endl  ;
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "LHipYawPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LHipPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LHipRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LKneePitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LAnklePitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "LAnkleRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }

    }

    // write data into array
    idx = 0;
    for(vector<float>::const_iterator iter = buffer.data.begin(); iter != buffer.data.end(); ++iter)
    {
        // store into temporary target motor state buffer
        motor_l_leg_in[idx] = *iter;
        idx++;
    }

    // display data on terminal
    cout << endl ;

    for (int i=0; i<R_LEG_DOF; i++)
        cout << motor_l_leg_in[i] << ",";
    cout << endl;





    // extract the proprioceptive state of the RIGHT ARM
    buffer.data.clear();
    cout << endl << "Right arm joints:  " << endl  ;
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "RShoulderPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RShoulderRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RElbowYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RElbowRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RWristYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RHand")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }

    }

    // write data into array
    idx = 0;
    for(vector<float>::const_iterator iter = buffer.data.begin(); iter != buffer.data.end(); ++iter)
    {
        // store into temporary target motor state buffer
        motor_r_arm_in[idx] = *iter;
        idx++;
    }
    cout << endl ;
    for (int i=0; i<R_LEG_DOF; i++)
        cout << motor_r_arm_in[i] << ",";
    cout << endl;



    // extract the proprioceptive state of the RIGHT LEG
    buffer.data.clear();
    cout << endl << "Right leg joints:  " << endl  ;
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "RHipYawPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RHipPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RHipRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RKneePitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RAnklePitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }
        if (joint_state->name[i] == "RAnkleRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            cout << joint_state->name[i] <<": " << joint_state->position[i] << "    ";
        }

    }

    // write data into array
    idx = 0;
    for(vector<float>::const_iterator iter = buffer.data.begin(); iter != buffer.data.end(); ++iter)
    {
        // store into temporary target motor state buffer
        motor_r_leg_in[idx] = *iter;
        idx++;
    }

    // display data on terminal
    cout << endl ;

    for (int i=0; i<R_LEG_DOF; i++)
        cout << motor_r_leg_in[i] << ",";
    cout << endl;



}

// send commanded joint positions of the HEAD
void sendTargetJointStateHead(double dummy[HEAD_DOF] /* dummy representing the comanded joint state*/) /* maybe a result as function argument */
{

    robot_specific_msgs::JointAnglesWithSpeed target_joint_state;

    // specify the limb
    target_joint_state.joint_names.clear();
    target_joint_state.joint_names.push_back("Head");

    // specifiy the angle
    target_joint_state.joint_angles.clear();
    for (int i=0; i<HEAD_DOF; i++)
        target_joint_state.joint_angles.push_back(dummy[i] /* array containing result */);

    // set speed
    target_joint_state.speed = 0.2;

    // set the mode of joint change
    target_joint_state.relative = 0;

    // send to robot
    target_joint_state_pub.publish(target_joint_state);

}

// send commanded joint positions of the LEFT LEG
void sendTargetJointStateLLeg(double dummy[L_LEG_DOF]/* dummy representing the comanded joint state */)
{


    robot_specific_msgs::JointAnglesWithSpeed target_joint_state;

    // specify the limb
    target_joint_state.joint_names.clear();
    target_joint_state.joint_names.push_back("LLeg");

    // specifiy the angle
    target_joint_state.joint_angles.clear();
    for (int i=0; i<L_LEG_DOF; i++)
        target_joint_state.joint_angles.push_back(dummy[i] /* array containing result */);

    // set speed
    target_joint_state.speed = 0.2;

    // set the mode of joint change
    target_joint_state.relative = 0;

    // send to robot
    target_joint_state_pub.publish(target_joint_state);

}
// send commanded joint positions of the RIGHT LEG
void sendTargetJointStateRLeg(double dummy[R_LEG_DOF]/* dummy representing the comanded joint state */)
{


    robot_specific_msgs::JointAnglesWithSpeed target_joint_state;

    // specify the limb
    target_joint_state.joint_names.clear();
    target_joint_state.joint_names.push_back("RLeg");

    // specifiy the angle
    target_joint_state.joint_angles.clear();
    for (int i=0; i<R_LEG_DOF; i++)
        target_joint_state.joint_angles.push_back(dummy[i] /* array containing result */);

    // set speed
    target_joint_state.speed = 0.2;

    // set the mode of joint change
    target_joint_state.relative = 0;

    // send to robot
    target_joint_state_pub.publish(target_joint_state);

}

// send commanded joint positions of the LEFT ARM
void sendTargetJointStateLArm(double dummy[L_ARM_DOF]/* dummy representing the comanded joint state */)
{


    robot_specific_msgs::JointAnglesWithSpeed target_joint_state;

    // specify the limb
    target_joint_state.joint_names.clear();
    target_joint_state.joint_names.push_back("LArm");

    // specifiy the angle
    target_joint_state.joint_angles.clear();
    for (int i=0; i<L_ARM_DOF; i++)
        target_joint_state.joint_angles.push_back(dummy[i] /* array containing result */);

    // set speed
    target_joint_state.speed = 0.2;

    // set the mode of joint change
    target_joint_state.relative = 0;

    // send to robot
    target_joint_state_pub.publish(target_joint_state);

}


// send commanded joint positions of the RIGHT ARM
void sendTargetJointStateRArm(double dummy[R_ARM_DOF]) // dummy representing the comanded joint state)
{

    robot_specific_msgs::JointAnglesWithSpeed target_joint_state;

    // specify the limb
    target_joint_state.joint_names.clear();
    target_joint_state.joint_names.push_back("RArm");

    // specifiy the angle
    target_joint_state.joint_angles.clear();
    for (int i=0; i<R_ARM_DOF; i++)
        target_joint_state.joint_angles.push_back(dummy[i] /* array containing result */);

    // set speed
    target_joint_state.speed = 0.2;

    // set the mode of joint change
    target_joint_state.relative = 0;

    // send to robot
    target_joint_state_pub.publish(target_joint_state);

}

// callback function for key events
void keyCB(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO("key pushed: %s", msg->data.c_str());

    // start the robot behaviour
    if (*(msg->data.c_str()) == '0')
	{
		cout << "keyCB()" << endl;
		
    	
	}

}



// callback function for move leg
void robotActionCB(const std_msgs::Int16::ConstPtr& msg)
{
    /*cout << "Here is aruco call back function"<<endl;
    cout << "x1: " << msg->data[0] << " x2: " << msg->data[1] << " x3: " << msg->data[2] << endl;*/
    switch ( msg->data)
    {
    case 0:
        cout << "Kick! from call back" << endl;
        sendTargetJointStateRLeg(R_LEG_S_Roll) ;
        sleep(2);
        sendTargetJointStateRLeg(R_LEG_S_Kick_1) ;
        sleep(1);
        sendTargetJointStateRLeg(R_LEG_S_Kick_2) ;
        sleep(2);
        sendTargetJointStateRLeg(R_LEG_S_Roll) ;

        //return 0;
        break;
    case 1:
        cout << "Move in from call back" << endl;
        //return 1;
        R_LEG_S_Roll[1] += 0.05 ;
        sendTargetJointStateRLeg(R_LEG_S_Roll) ;
        R_LEG_S_Kick_1[1] = R_LEG_S_Roll[1] ;
        R_LEG_S_Kick_2[1] = R_LEG_S_Roll[1] ;

        break;
    case 2:
        cout << "Move out from call back" << endl;
        R_LEG_S_Roll[1] -= 0.05 ;
        sendTargetJointStateRLeg(R_LEG_S_Roll) ;
        R_LEG_S_Kick_1[1] = R_LEG_S_Roll[1] ;
        R_LEG_S_Kick_2[1] = R_LEG_S_Roll[1] ;
        //return 2;
        break;
    default :
        cout<<"Undefine movement!  form call back"<<endl;
    }

}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "central_node");
    ros::NodeHandle central_node_nh;

    // messaging with the NAO nodes

    // advertise joint stiffnesses
    stiffness_pub = central_node_nh.advertise<robot_specific_msgs::JointState>("joint_stiffness", 1);

    // subscribe to the joint states
    // the topic is the same as the one of the wrapper node of the NAO robot
    ros::Subscriber joint_state_sub;
    joint_state_sub = central_node_nh.subscribe("joint_states", 1, &jointStateCB);

    // advertise the target joint states
    target_joint_state_pub = central_node_nh.advertise<robot_specific_msgs::JointAnglesWithSpeed>("joint_angles", 1);    // to NAO robot

    // using image_transport to publish and subscribe to images
    image_transport::ImageTransport image_tran(central_node_nh);

    // subscribe to the raw camera image
    image_transport::Subscriber image_sub;
    //image_sub = image_tran.subscribe("/nao_robot/camera/top/camera/image_raw", 1, &visionCB);

    // subscribe to tactile and touch sensors
    tactile_sub = central_node_nh.subscribe("tactile_touch", 1, tactileCB);

    // set up the subscriber for the keyboard
    ros::Subscriber key_sub;
    key_sub = central_node_nh.subscribe("key", 5, keyCB);




    robot_action_=central_node_nh.subscribe("/robot_action",5,robotActionCB);

    ros::spin();

    return 0;
}
