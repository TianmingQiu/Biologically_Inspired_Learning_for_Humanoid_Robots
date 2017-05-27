 /*
 *   Author:      Erhard Wieser
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

// own files
#include "robot_config.h"
//#include "nn.h"
#include "cmac.h"


using namespace std;
using namespace cv;
using namespace sensor_msgs;
using namespace message_filters;

namespace enc = sensor_msgs::image_encodings;


// subscribers to tactile and touch sensors
ros::Subscriber tactile_sub;
ros::Subscriber bumper_sub;

// publisher to robot joints for sending the target joint angles
ros::Publisher target_joint_state_pub;

// joint stiffnesses
ros::Publisher stiffness_pub;

// received motor state of the HEAD
double motor_head_in[HEAD_DOF];

// received motor state of the LEFT ARM
double motor_l_arm_in[L_ARM_DOF];

// received motor state of the RIGHT ARM
double motor_r_arm_in[R_ARM_DOF];

// label of the GUI window showing the raw image of NAO's camera
static const char cam_window[] = "NAO Camera (raw image)";
vector<Moments> mu(1);
vector<Point2f> mc(1);
vector<float>  Joint_Value;
float JointValue[6];
vector<float>  Center_Position;
int gloal_count=0;
int active_flag=0; //set a global flag to activate the robot tracking

void sendTargetJointStateLArm(double dummy[L_ARM_DOF]);    //declare functions
void sendTargetJointStateRArm(double dummy[R_ARM_DOF]);
void sendTargetJointStateHead(double dummy[HEAD_DOF]);


// set the stiffness
void setStiffness(float value)
{
    cout << "setting stiffnesses (Head) to " << value << endl;

    robot_specific_msgs::JointState target_joint_stiffness;

    // set stiffnesses of HEAD joints
    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("LShoulderRoll");
    target_joint_stiffness.effort.clear();
    //for (int i=0; i<L_ARM_DOF; i++)
    target_joint_stiffness.effort.push_back(value);

    stiffness_pub.publish(target_joint_stiffness);

    target_joint_stiffness.name.clear();
    target_joint_stiffness.name.push_back("LShoulderPitch");
    target_joint_stiffness.effort.clear();
    //for (int i=0; i<L_ARM_DOF; i++)
    target_joint_stiffness.effort.push_back(value);

    stiffness_pub.publish(target_joint_stiffness);

}


// callback function for vision
void visionCB(const sensor_msgs::ImageConstPtr& msg)
{
    // pointer on OpenCV image
    cv_bridge::CvImagePtr cv_ptr;

    try
    {
        // transform ROS image into OpenCV image
        cv_ptr = cv_bridge::toCvCopy(msg, enc::BGR8);
    }
    catch (cv_bridge::Exception& e)		// throw an error msg. if conversion fails
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    // show the raw camera image
    Mat raw_image=cv_ptr->image;
    Mat hsv_image;

    //convert RGB image to HSV
    cvtColor(raw_image,hsv_image,CV_BGR2HSV);
    Mat L_RED_BLOB;
    Mat U_RED_BLOB;
    //select the hue range for red
    inRange(hsv_image,Scalar(0,100,100),Scalar(10,255,255),L_RED_BLOB);
    inRange(hsv_image,Scalar(150,100,100),Scalar(179,255,255),U_RED_BLOB);
    Mat red_image;
    Mat red_image_bi;
    addWeighted(L_RED_BLOB,1.0,U_RED_BLOB,1.0,0.0,red_image);

    //smooth the image and Thresholiding
     GaussianBlur( red_image, red_image_bi, Size(9,9), 0, 0 );
     red_image_bi = red_image_bi > 150;

     //imshow the original image
     imshow(cam_window, raw_image);
     //imshow the image only with red target
     imshow("red binay image", red_image_bi);


    //find all the contours
    Canny(red_image_bi, red_image_bi, 100, 200, 3);
    //Find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    findContours( red_image_bi, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // Draw contours
    Mat drawing = Mat::zeros( red_image_bi.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }


    // iterate through each contour.
     Mat src = Mat::zeros( red_image_bi.size(), CV_8UC3 );
     int largest_area=0;
         int largest_contour_index=0;
         //Rect bounding_rect;
        for( int i = 0; i< contours.size(); i++ )
        {
            //  Find the area of contour
            double a=contourArea( contours[i],false);
            if(a>largest_area){
                largest_area=a;
                //cout<<i<<" area  "<<a<<endl;
                // Store the index of largest contour
                largest_contour_index=i;
                // Find the bounding rectangle for biggest contour
                //bounding_rect=boundingRect(contours[i]);
            }
        }
        Scalar color( 255,255,255);  // color of the contour in the
        //Draw the contour and rectangle
        drawContours( src, contours,largest_contour_index, color, CV_FILLED,8,hierarchy);
        //rectangle(src, bounding_rect,  Scalar(0,255,0),2, 8,0);


        if ( contours.size()){
           mu[0] = moments( contours[largest_contour_index], false );

           mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
           //cout<<"center of mass of the lagerst blob: "<<mc[0]<<endl;
         }


   imshow( "contour window", drawing );
   imshow( "lagest contour window", src );
   waitKey(3);

   // if active option, tracking the object
   if (active_flag==1){
       CMAC net;
       net.LoadPara();
       double joint_position_normalized[2];
       net.Predict((mc.at(0).x)/320,(mc.at(0).y)/240,joint_position_normalized);

       double joint_position[2];

       //cout<<"joint position norm "<<joint_position_normalized[0]<<" "<<joint_position_normalized[1]<<endl;

       joint_position[0]=joint_position_normalized[0]*4.1714-2.0857;
       joint_position[1]=joint_position_normalized[1]*1.6407-0.3142;
       joint_position[2]=JointValue[2];
       joint_position[3]=JointValue[3];
       joint_position[4]=JointValue[4];
       joint_position[5]=JointValue[5];

       cout<<"Joint should move to :"<<joint_position[0]<<"  "<<joint_position[1]<<endl;
       if (joint_position[0]<-2 || joint_position[0]>2 || joint_position[1]<-0.3 || joint_position[1]>1.3)
           gloal_count++;

       else{
          sendTargetJointStateLArm(joint_position);
       }
   }

}


// callback function for tactile buttons (TBs) on the head

void tactileCB(const robot_specific_msgs::TactileTouch::ConstPtr& __tactile_touch)
{

    // check TB 3 (rear)   enable tracking object option
    if (((int)__tactile_touch->button == 3) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;

        //active the tracking function
        active_flag=1;

        //collection training data
        /*Joint_Value.push_back(JointValue[0]);
        Joint_Value.push_back(JointValue[1]);
        Center_Position.push_back(mc.at(0).x);
        Center_Position.push_back(mc.at(0).y);
        ofstream inFile;

        inFile.open("data/Joint_value.txt", ios::trunc);
        for (int h = 0; h <(Joint_Value.size())/2; h++){
                inFile << Joint_Value.at(h*2) << "\t"<<Joint_Value.at(h*2+1)<<endl;
        }
        inFile.close();

        inFile.open("data/Object_Center.txt", ios::trunc);
        for (int i = 0; i < Center_Position.size()/2; i++){
            inFile << Center_Position.at(i*2) << "\t"<<Center_Position.at(i*2+1)<<endl;
        }
        inFile.close();
        gloal_count++;
        cout<<"THE "<<gloal_count<<" th collection!"<<endl;*/


    }

    // check TB 2 (middle)  tranning neural network and save weights parameter
    if (((int)__tactile_touch->button == 2) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;
        CMAC net;

        ifstream file;
        double INPUT[nsample][N];
        double OUTPUT[nsample][Q];

        file.open("data/Object_Center.txt");
        for (int k = 0; k < nsample; k++){
            for (int h = 0;h < N; h++){
                file >> INPUT[k][h];
                }
        }
        file.close();

        file.open("data/Joint_value.txt");
        for (int k = 0; k < nsample; k++){
            for (int j = 0; j < Q; j++){
                file >> OUTPUT[k][j];
            }
        }
        file.close();

        for (int k = 0; k < nsample; k++){
			
			//Normalize
            INPUT[k][0] /= 320;
            INPUT[k][1] /= 240;
            OUTPUT[k][0] = (OUTPUT[k][0] + 2.0857) / 4.1714;
            OUTPUT[k][1] = (OUTPUT[k][1] + 0.3142)/1.6407;
        }

        net.Train(INPUT, OUTPUT);
        net.SavePara();

    }

    // check TB 1 (front)
    if (((int)__tactile_touch->button == 1) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;
        //set home position for head	
        double head_joint[2];
        head_joint[0]=0.5252;
        head_joint[1]=-0.2712;
        sendTargetJointStateHead(head_joint);

        setStiffness(0);

    }

}


// callback function for bumpers
void bumperCB(const robot_specific_msgs::Bumper::ConstPtr& __bumper)
{
    // check each bumper

    cout << "bumper " << (int)__bumper->bumper << endl;

    static bool left_bumper_flag = false;
    static bool right_bumper_flag = false;

    // check left bumper
    if (((int)__bumper->bumper == 1) && ((int)__bumper->state == 1))
    {
        left_bumper_flag = !left_bumper_flag;   // toggle flag

        // do something, e.g.:
        // set / reset stiffness
        if (left_bumper_flag)
            setStiffness(0.005);
        /*
        else
            setStiffness(0.9);  */

    }

    // check right bumper
    if (((int)__bumper->bumper == 0) && ((int)__bumper->state == 1))
    {
        right_bumper_flag = !right_bumper_flag;     // toggle flag



    }

}


// callback function for the head joints
void jointStateCB(const robot_specific_msgs::JointState::ConstPtr& joint_state)
{
    // buffer for incoming message
    std_msgs::Float32MultiArray buffer;

    // index
    int idx;


    // extract the proprioceptive state of the HEAD
    buffer.data.clear();
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "HeadYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << ": " << joint_state->position[i] << endl;
        }
        if (joint_state->name[i] == "HeadPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << ": " << joint_state->position[i] << endl;
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


    // extract the proprioceptive state of the LEFT ARM
    buffer.data.clear();
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "LShoulderPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            JointValue[0]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "LShoulderRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            JointValue[1]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "LElbowYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
             JointValue[2]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "LElbowRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
             JointValue[3]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "LWristYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
             JointValue[4]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "LHand")
        {
            buffer.data.push_back(joint_state->position[i]);
             JointValue[5]=joint_state->position[i];
            // cout << joint_state->name[i] << endl;
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


    // extract the proprioceptive state of the RIGHT ARM
    buffer.data.clear();
    for (int i=0; i<ROBOT_DOF; i++)
    {
        if (joint_state->name[i] == "RShoulderPitch")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "RShoulderRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "RElbowYaw")
        {
           buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "RElbowRoll")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "RWristYaw")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
        }
        if (joint_state->name[i] == "RHand")
        {
            buffer.data.push_back(joint_state->position[i]);
            // cout << joint_state->name[i] << endl;
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


}


// send commanded joint positions of the HEAD
void sendTargetJointStateHead(double dummy[HEAD_DOF])
{
    //double dummy[HEAD_DOF];  // dummy representing the comanded joint state
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


// send commanded joint positions of the LEFT ARM
void sendTargetJointStateLArm(double dummy[L_ARM_DOF])
{
      // dummy representing the comanded joint state
    //dummy = LArmJoint;
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
void sendTargetJointStateRArm(double dummy[R_ARM_DOF])
{
    //double dummy[R_ARM_DOF];  // dummy representing the comanded joint state
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
    image_sub = image_tran.subscribe("/nao_robot/camera/top/camera/image_raw", 1, &visionCB);

    // subscribe to tactile and touch sensors
    tactile_sub = central_node_nh.subscribe("tactile_touch", 1, tactileCB);
    bumper_sub = central_node_nh.subscribe("bumper", 1, bumperCB);

    // set up the subscriber for the keyboard
    ros::Subscriber key_sub;
    key_sub = central_node_nh.subscribe("key", 5, keyCB);

    // create a GUI window for the raw camera image
    namedWindow(cam_window, 0);

    ros::spin();

    return 0;
}
