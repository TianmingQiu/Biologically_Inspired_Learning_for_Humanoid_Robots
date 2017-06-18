#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <std_msgs/Float64MultiArray.h>
#include "std_msgs/String.h"
using namespace std;
using namespace cv;
using namespace aruco;

/*vector<Marker> oldMarkers;
vector<float> markerid;*/

int counter=1;
class ImageConverter
{
    ros::NodeHandle nh_;
    ros::Publisher aruco_positon_x_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;
    CameraParameters TheCameraParameters;

public:
    ImageConverter()
    : it_(nh_)
    {
        // Subscrive to input video feed and publish output video feed
        image_sub_ = it_.subscribe("/nao_robot/camera/top/camera/image_raw", 1, &ImageConverter::imageCb, this);
        aruco_positon_x_ = nh_.advertise< std_msgs::Float64MultiArray > ("/aruco_positon_x", 1000);

        Mat dist(1,5,CV_32FC1);
        dist.at<float>(0,0)=-0.066494;
        dist.at<float>(0,1)=0.095481;
        dist.at<float>(0,2)=-0.000279;
        dist.at<float>(0,3)=0.002292;
        dist.at<float>(0,4)=0.000000;
        Mat cameraP(3,3,CV_32FC1);

        cameraP.at<float>(0,0)=551.543059;
        cameraP.at<float>(0,1)=0.000000;
        cameraP.at<float>(0,2)=327.382898;
        cameraP.at<float>(1,0)=0.000000;
        cameraP.at<float>(1,1)=553.736023;
        cameraP.at<float>(1,2)=225.026380;
        cameraP.at<float>(2,0)=0.000000;
        cameraP.at<float>(2,1)=0.000000;
        cameraP.at<float>(2,2)=1.000000;

        TheCameraParameters.setParams(cameraP,dist,Size(640,480));
        TheCameraParameters.resize( Size(640,480));
    }

    ~ImageConverter()
    {
    }

    string IntToStr(int a)
    {
        stringstream ss;
        ss << a;
        return ss.str();
    }

    void imageCb(const sensor_msgs::ImageConstPtr& msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }

        Mat InImage=cv_ptr->image;

        std_msgs::Float64MultiArray msg1;



        vector<Marker> Markers = detectMarker(InImage);


        for  (int i=0;i<Markers.size();i++)
        {

               Marker thisMarker = Markers[i];
               thisMarker.draw(InImage,Scalar(0,0,255),2);
               if (thisMarker.id==3)
                   thisMarker.calculateExtrinsics(0.064,TheCameraParameters,true);
               else
                   thisMarker.calculateExtrinsics(0.1,TheCameraParameters,true);

               /*markerid.push_back(thisMarker.id);
               markerid.push_back(thisMarker.Tvec.at<float>(0));
               markerid.push_back(thisMarker.Tvec.at<float>(1));
               markerid.push_back(thisMarker.Tvec.at<float>(2));*/
               if ( Markers.size()==3){
                   cout<< "The marker id is: "<<thisMarker.id<<" the position is x: "<<thisMarker.Tvec.at<float>(0)<<" y: "<<thisMarker.Tvec.at<float>(1)<<" z: "<<thisMarker.Tvec.at<float>(2)<<endl;
                   msg1.data.resize(3);
                   msg1.data[i]=thisMarker.Tvec.at<float>(0);
                   if (i==2){
                       aruco_positon_x_.publish(msg1);
                   }

               }
          }



        imshow("markers",InImage);
        waitKey(10);

}

    vector<Marker> detectMarker(Mat image){

        MarkerDetector Mdetector;
        //define the output as a vector of markers
        vector<Marker> Markers;
        //detect markers in InImage image and output them to Markers. TheCameraParameters are
        //extrinsic and intrinsic parameters of the camera which are predefined.
        Mdetector.detect(image,Markers,TheCameraParameters);

        return Markers;
    }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "rl_vision");
    ImageConverter ic;


    ros::spin();
    return 0;
}

