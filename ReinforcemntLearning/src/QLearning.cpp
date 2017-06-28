#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h> 
#include <stdio.h>
#include <iomanip>
#include <ros/ros.h>
#include "std_msgs/Int16.h"
#include "robot_config.h"
#include "std_msgs/Float64MultiArray.h"
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>

using namespace std;



#define GAMMA 0.9
#define STATES 5
#define ACT 3

double goal;
double hip_roll;
double *dp_hip_roll;

ros::Publisher nao_speech_;

class QLEARNING
{
    ros::NodeHandle nh_1;
    ros::Publisher action_send;

public:
    QLEARNING()
    {
        action_send = nh_1.advertise< std_msgs::Int16 > ("/robot_action", 5);
    }
    ~QLEARNING()
    {
    }
    void Train();
    void Predict();
    void SavePara();
    void LoadPara();
    void Motion();
    void SaveQPara(int i);
    int GoalkeeperPos(double coordinate);
    std_msgs::Int16 msg;

private:
    int N[STATES][ACT] = {};
    double Q[STATES][ACT] = {};
    double R; //Reward
    double alpha; //1/n
    int current_state; //0,1,2,3,4
    int action;   //0:kick, 1:move in(move left) 2:move out(move right) (use right leg)
    double delta; //to judge the convergence
    void Act();
    void Update();
    int Max(int s);
    int find_next_state();
    int change_initial_state();
    int act_select_rand(int a, int b);
    int goalkeeper_pos;
    int Ns[STATES][ACT][STATES] = {};
    double Pr[STATES][ACT][STATES] = {};
    double Pr_temp[STATES][ACT][STATES] = {};
    int current_state_message();
    void Update_Pr(int st, int at);
    double Sum[STATES][ACT] = {};
    int motion;
    int ss = 4;

};



void QLEARNING::Train()
{
    char yes;
    cout<<"Are you ready?(Y/N)"<<endl;
    cin>>yes;
	Q[0][1] = -99999; //angle border
	Q[4][2] = -99999;
    for (int s=0; s++; s<STATES)
    {
        for (int a=0;a++;a<ACT)
        {
            for(int s_p=0;s_p++; s_p<STATES)
            {
                Pr[s][a][s_p]=0.2;
                Pr_temp[s][a][s_p]=0.2;
            }
        }
    }
	//set the initial state
    current_state = 4;
    int i=0;

    while(1){
        Act();
        //observe reward after taking action

        cout << "give the reward: ";
        cin >> R; //get the reward
        if (R == 666) {
            cout << "Successful training!!" << endl;
            SavePara();
            break;
        }

        Update();
        SaveQPara(i);
        i++;
        sleep(3);
        //ros::spinOnce();
    }
}

void QLEARNING::Motion()
{

    int s_actual;
    while(1){
        cout<<"current hip joint:"<<hip_roll<<endl;
        cout << "give the motion: ";
        cin >> motion;

        switch (motion)
        {
        case 0:
            cout << "Kick!" << endl;

            msg.data=0;
            action_send.publish(msg);
            cout << "Wait:" ;
            sleep(1); cout << "=====";
            sleep(1); cout << "=====";
            sleep(1); cout << "=====";
            sleep(1); cout << "=====";
            sleep(1); cout << "=====";

            sleep(0.5); cout << "====="  << "100%" <<endl  << endl;
            //return 0;
            cout << "ideal current state: state" << ss <<endl;
            break;
        case 1:
            cout << "Move in" << endl;
            ss -= 1;
            //return 1;
            msg.data=1;
            action_send.publish(msg);
            sleep(1); cout << "====================";
            sleep(0.5); cout << "====================" << "100%" <<endl << endl;
            //s_actual = current_state_message();
            //cout << "actual current state: state" << s_actual <<endl;
            cout << "ideal current state: state" << ss <<endl;
            break;
        case 2:
            cout << "Move out" << endl;
            ss += 1;
            msg.data=2;
            action_send.publish(msg);
            sleep(1); cout << "====================";
            sleep(0.5); cout << "====================" << "100%" <<endl << endl;
            //s_actual = current_state_message();
            //cout << "actual current state: state" << s_actual <<endl;
            //return 2;
            cout << "ideal current state: state" << ss <<endl;
            break;
        default:
            cout << "Undefined Move!" << endl;
            break;
        }
        sleep(2);
        ros::spinOnce();
        s_actual = current_state_message();
        cout << "actual current state: state" << s_actual <<endl;
    }

}

void QLEARNING::Act()
{
	//choose action from current state derived from Q(epsilon-greedy)
	/*int k;
	k = rand() % 100;
	cout<<"k="<<k<<endl;
	if (k > 85) action = rand() % 2;
	else action = Max(current_state);*/
	action = Max(current_state);
	//update N(s,a) after each trail:
	N[current_state][action] += 1;
	//excute the action

	switch (action)
	{
	case 0:
		cout << "Kick!" << endl;

        msg.data=0;
        action_send.publish(msg);
        cout << "Wait:" ;
        sleep(1); cout << "=====";
        sleep(1); cout << "=====";
        sleep(1); cout << "=====";
        sleep(1); cout << "=====";
        sleep(1); cout << "=====";

        sleep(0.5); cout << "====="  << "100%" <<endl  << endl;
		//return 0;
		break;
	case 1:
		cout << "Move in" << endl;

		//return 1;
        msg.data=1;
        action_send.publish(msg);
        sleep(1); cout << "====================";
        sleep(0.5); cout << "====================" << "100%" <<endl << endl;

		break;
	case 2:
		cout << "Move out" << endl;

        msg.data=2;
        action_send.publish(msg);
        sleep(1); cout << "====================";
        sleep(0.5); cout << "====================" << "100%" <<endl << endl;
		//return 2;
		break;
    default:
        cout << "Undefined Move!" << endl;
        break;
	}


}

int QLEARNING::GoalkeeperPos(double coordinate)
{
    goalkeeper_pos = (int)(coordinate / 0.20); //can be modified later
    cout<<"goalkeeper postion is: "<<goalkeeper_pos<<endl;
    stringstream  ss;
    ss<<"Goal keeper at position "<<goalkeeper_pos;
    std_msgs::String msgs;
    msgs.data=ss.str();
    nao_speech_.publish(msgs);

	return goalkeeper_pos;
}


int QLEARNING::Max(int s)
{
	int act_max;
	double a, b, c;
	a = Q[s][0];
	b = Q[s][1];
	c = Q[s][2];
	if (a > b) act_max = 0;
	else if (a == b) act_max = act_select_rand(0, 1);
	else act_max = 1;

	if (Q[s][act_max] > c) return act_max;
	else if (Q[s][act_max] == c) return act_select_rand(act_max, 2);
	else return 2;
}

int QLEARNING::act_select_rand(int m, int n)
{
	int k;
	k = rand() % 2;
	switch (k)
	{
	case 0:
		return m;
		break;
	case 1:
		return n;
		break;
    default:
        return m;
        break;
	}
}
void QLEARNING::Update_Pr(int st, int at)
{


    for (int s_next = 0; s_next < STATES; s_next++)
    {
        int n_sum = 0;
        for (int s_pp = 0; s_pp < STATES; s_pp++)
        {
            n_sum += Ns[st][at][s_pp];
        }
        Pr[st][at][s_next] = Ns[st][at][s_next] / n_sum;
    }

}

void QLEARNING::Update()
{
    //double n = N[current_state][action];
    //alpha = 1 / n;



    int state_prime;
    int max_action_prime;
    double maxQ;
    double sum_pq = 0; // sum of Pr * maxQ when p' varying from 0-4
    cout<<"state before action: "<<current_state<<endl;
    state_prime = current_state_message(); // get the message from robot
    Ns[current_state][action][state_prime] += 1;
    cout<<"state after action: "<<state_prime<<endl;
    Update_Pr(current_state, action);// seem too helpless
    for (int s_prime = 0; s_prime < STATES; s_prime++)
    {
        max_action_prime = Max(s_prime);
        maxQ = Q[s_prime][max_action_prime];
        sum_pq += Pr[current_state][action][s_prime] * maxQ;
    }

    //Q[current_state][action] = Q[current_state][action] + alpha * (R + GAMMA * maxQ - Q[current_state][action]);
    Q[current_state][action] = R + GAMMA * sum_pq;
    //update for next implementation
    if (R == 20){
        //update probability after a trial
        /*for (int s = 0; s < STATES; s++)
        {
            for (int a = 0; a < ACT; a++)
            {

                for (int s_prime = 0; s_prime < STATES; s_prime++)
                {
                    Pr[s][a][s_prime] = Pr_temp[s][a][s_prime];
                }
            }
        }*/
        cout << "This is " << N[current_state][action] << "th trial!" << endl;
        int new_initial_state, step, direction;
        new_initial_state = change_initial_state();
        step = abs(new_initial_state - current_state);
        direction = new_initial_state - current_state;
        if (direction < 0) {
            for (int i = 0; i < step; i++){
                msg.data=1;
                action_send.publish(msg);
                cout<<"move in"<<endl;
            }
        }
        else if (direction > 0){
            for (int i = 0; i < step; i++){
                msg.data=2;
                action_send.publish(msg);
                cout<<"move out"<<endl;
            }
        }

        current_state = new_initial_state;

        for (int s = 0; s < STATES; s++)
        {
            cout << "state" << s << endl;
            for (int a = 0; a < ACT; a++)
            {
                cout << "action" << a << endl;
                for (int s_prime = 0; s_prime < STATES; s_prime++)
                {
                    cout << Pr[s][a][s_prime] << "\t";
                }
                cout << endl;
            }
            cout << endl;
        }
    }
    else current_state = state_prime;
    action = Max(current_state);



    //print Q(s,a)

    /*cout << endl << "\t\t\t\t\t\tN(s,a) Table" << endl;
    cout << "|-----------------------------------------------------------";
    cout << "------------------------------------------------------------|" << endl;

    cout << "|" << "\t" << N[0][0] << "\t\t" << "|"
    << "\t" << N[1][0] << "\t\t" << "|"
    << "\t" << N[2][0] << "\t\t" << "|"
    << "\t" << N[3][0] << "\t\t" << "|"
    << "\t" << N[4][0] << "\t\t" << "|" << endl;
    cout << "|\t\t\t|\t\t\t|\t\t\t|\t\t\t|\t\t\t|" << endl;
    cout << "|" << N[0][1] << "\t\t" << N[0][2] << "\t|"
    << N[1][1] << "\t\t" << N[1][2] << "\t|"
    << N[2][1] << "\t\t" << N[2][2] << "\t|"
    << N[3][1] << "\t\t" << N[3][2] << "\t|"
    << N[4][1] << "\t\t" << N[4][2] << "\t|" << endl;
    cout << "|-----------------------------------------------------------";
    cout << "------------------------------------------------------------|" << endl;*/

    cout << endl << "\t\t\t\t\t\tQ(s,a) Table" << endl;
    cout << "|-----------------------------------------------------------";
    cout << "------------------------------------------------------------|" << endl;
    cout << "|" << "\t" << Q[0][0] << "\t\t" << "|"
        << "\t" << Q[1][0] << "\t\t" << "|"
        << "\t" << Q[2][0] << "\t\t" << "|"
        << "\t" << Q[3][0] << "\t\t" << "|"
        << "\t" << Q[4][0] << "\t\t" << "|" << endl;
    cout << "|\t\t\t|\t\t\t|\t\t\t|\t\t\t|\t\t\t|" << endl;
    cout << setiosflags(ios::fixed) << setprecision(3);
    cout << "|" << "X" << "\t\t" << Q[0][2] << "\t|"
        << Q[1][1] << "\t\t" << Q[1][2] << "\t|"
        << Q[2][1] << "\t\t" << Q[2][2] << "\t|"
        << Q[3][1] << "\t\t" << Q[3][2] << "\t|"
        << Q[4][1] << "\t\t" << "X" << "\t|" << endl;
    cout << "|-----------------------------------------------------------";
    cout << "------------------------------------------------------------|" << endl;

    switch (current_state)
    {
    case 0:
        cout << "\t^" << endl;
        break;
    case 1:
        cout << "\t\t\t\t^" << endl;
        break;
    case 2:
        cout << "\t\t\t\t\t\t\t^" << endl;
        break;
    case 3:
        cout << "\t\t\t\t\t\t\t\t\t\t^" << endl;
        break;
    case 4:
        cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t^" << endl << endl;
        break;
    }
}



int QLEARNING::find_next_state()
{
	int next_state;
	switch (action)
	{
	case 0:
		next_state = current_state; //later replace with joint command messages
		break;
	case 1:
		next_state = current_state - 1;
		break;
	case 2:
		next_state = current_state + 1;
		break;
    default:

        cout << "Undefined Move!" << endl;
        break;
	}
	return next_state;
}

int QLEARNING::change_initial_state()
{
	int new_initial_state;
	int k;
	k = rand() % 4;
	switch (k)
	{
	case 0:
		new_initial_state = (current_state + 1) % 5;
		break;
	case 1:
		new_initial_state = (current_state + 2) % 5;
		break;
	case 2:
		new_initial_state = (current_state + 3) % 5;
		break;
	case 3:
		new_initial_state = (current_state + 4) % 5;
		break;
    default:

        break;
	}
	return new_initial_state;
}


void QLEARNING::SavePara()
{
    ofstream inFile;
	switch (goalkeeper_pos)
	{
	case 0:

        inFile.open("data/Q0.txt", ios::trunc);
		for (int a = 0; a < ACT; a++){
			for (int s = 0; s < STATES; s++){
				inFile << Q[s][a] << "\t";
			}
			inFile << endl;
		}

		inFile.close();
		break;
	case 1:

		inFile.open("data/Q1.txt", ios::trunc);
		for (int a = 0; a < ACT; a++){
			for (int s = 0; s < STATES; s++){
				inFile << Q[s][a] << "\t";
			}
			inFile << endl;
		}

		inFile.close();
		break;
	case 2:

		inFile.open("data/Q2.txt", ios::trunc);
		for (int a = 0; a < ACT; a++){
			for (int s = 0; s < STATES; s++){
				inFile << Q[s][a] << "\t";
			}
			inFile << endl;
		}

		inFile.close();
		break;
	case 3:

		inFile.open("data/Q3.txt", ios::trunc);
		for (int a = 0; a < ACT; a++){
			for (int s = 0; s < STATES; s++){
				inFile << Q[s][a] << "\t";
			}
			inFile << endl;
		}

		inFile.close();
		break;
	case 4:

		inFile.open("data/Q4.txt", ios::trunc);
		for (int a = 0; a < ACT; a++){
			for (int s = 0; s < STATES; s++){
				inFile << Q[s][a] << "\t";
			}
			inFile << endl;
		}

		inFile.close();
		break;

	}
    cout << "Successful save the " << goalkeeper_pos << "th goalkeeper Q value table!!" << endl;
}

void QLEARNING::SaveQPara(int i)
{
    ofstream inFile;
    stringstream ss;
    string s;

    switch (goalkeeper_pos)
    {
    case 0:
         ss<<"data/Q0_"<<i<<".txt";
         s=ss.str();
        inFile.open(s, ios::trunc);
        for (int a = 0; a < ACT; a++){
            for (int s = 0; s < STATES; s++){
                inFile << Q[s][a] << "\t";
            }
            inFile << endl;
        }

        inFile.close();
        break;
    case 1:
        ss<<"data/Q1_"<<i<<".txt";
        s=ss.str();
       inFile.open(s, ios::trunc);

        for (int a = 0; a < ACT; a++){
            for (int s = 0; s < STATES; s++){
                inFile << Q[s][a] << "\t";
            }
            inFile << endl;
        }

        inFile.close();
        break;
    case 2:

        ss<<"data/Q2_"<<i<<".txt";
        s=ss.str();
       inFile.open(s, ios::trunc);
        for (int a = 0; a < ACT; a++){
            for (int s = 0; s < STATES; s++){
                inFile << Q[s][a] << "\t";
            }
            inFile << endl;
        }

        inFile.close();
        break;
    case 3:

        ss<<"data/Q3_"<<i<<".txt";
        s=ss.str();
       inFile.open(s, ios::trunc);
        for (int a = 0; a < ACT; a++){
            for (int s = 0; s < STATES; s++){
                inFile << Q[s][a] << "\t";
            }
            inFile << endl;
        }

        inFile.close();
        break;
    case 4:

        ss<<"data/Q4_"<<i<<".txt";
        s=ss.str();
       inFile.open(s, ios::trunc);
        for (int a = 0; a < ACT; a++){
            for (int s = 0; s < STATES; s++){
                inFile << Q[s][a] << "\t";
            }
            inFile << endl;
        }

        inFile.close();
        break;

    }
    cout << "Successful save the " << goalkeeper_pos << "th goalkeeper Q value table!!" << endl;
}

void QLEARNING::LoadPara()
{
		ifstream file;
		switch (goalkeeper_pos)
		{
		case 0:
			
            file.open("data/Q0.txt");

			for (int a = 0; a < ACT; a++){
				for (int s = 0; s < STATES; s++){
					file >> Q[s][a];
				}
			}
			file.close();
			break;

		case 1:
			
			file.open("data/Q1.txt");

			for (int a = 0; a < ACT; a++){
				for (int s = 0; s < STATES; s++){
					file >> Q[s][a];
				}
			}
			file.close();
			break;
		case 2:
			
			file.open("data/Q2.txt");

			for (int a = 0; a < ACT; a++){
				for (int s = 0; s < STATES; s++){
					file >> Q[s][a];
				}
			}
			file.close();
			break;
		case 3:
			
			file.open("data/Q3.txt");

			for (int a = 0; a < ACT; a++){
				for (int s = 0; s < STATES; s++){
					file >> Q[s][a];
				}
			}
			file.close();
			break;
		case 4:
			
			file.open("data/Q4.txt");

			for (int a = 0; a < ACT; a++){
				for (int s = 0; s < STATES; s++){
					file >> Q[s][a];
				}
			}
			file.close();
			break;
		}
        cout << "Successful load the " << goalkeeper_pos << "th goalkeeper Q value table!!" << endl;
}

void QLEARNING::Predict()
{
    //set the initial state
    std_msgs::String msgs;
    msgs.data="Now I am goint to random postion";
    nao_speech_.publish(msgs);

    current_state = rand() % 5;
    cout<<"current_state"<<current_state<<endl;
    int step, direction;
    step = abs(current_state - 4);
    direction = current_state- 4;
    if (direction < 0) {
        for (int i = 0; i < step; i++){
            msg.data=1;
            action_send.publish(msg);
            cout<<"move in"<<endl;
            sleep(1);
        }
    }
    else if (direction > 0){
        for (int i = 0; i < step; i++){
            msg.data=2;
            action_send.publish(msg);
            cout<<"move out"<<endl;
            sleep(1);
        }
    }

    sleep(3);
    msgs.data="Now start to kick";
    nao_speech_.publish(msgs);

    while (1){
        action = Max(current_state);



        switch (action)
        {
        case 0:
            //sleep(1);
            cout << "Kick!" << endl;
            msg.data=0;
            action_send.publish(msg);
            //return 0;
            sleep(4);
            break;

        case 1:
            //sleep(1);
            cout << "Move in" << endl;
            //return 1;
            msg.data=1;
            action_send.publish(msg);
            sleep(1);
            break;
        case 2:
            //sleep(1);
            cout << "Move out" << endl;
            msg.data=2;
            action_send.publish(msg);
            sleep(1);
            //return 2;
            break;
        default:
            cout << "Undefined Move!" << endl;
            break;
        }
        current_state = find_next_state();
        if (action == 0){ cout << "Goal!!" << endl;
            sleep(2);
            msgs.data="Goal";
            nao_speech_.publish(msgs);
            break; }


    }
}

// callback function for tactile buttons (TBs) on the head

void tactileCB(const robot_specific_msgs::TactileTouch::ConstPtr& __tactile_touch)
{
    QLEARNING reinl;
    sleep(1);




    // check TB 3 (rear)   enable tracking object option
    if (((int)__tactile_touch->button == 3) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;
        std_msgs::String msgs;
        msgs.data="Initialization the pose";
        nao_speech_.publish(msgs);
        sleep(5);


     }

    // check TB 2 (middle)  tranning neural network and save weights parameter
    if (((int)__tactile_touch->button == 2) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;
        reinl.GoalkeeperPos(goal);
        reinl.LoadPara();
        reinl.Predict();


    }

    // check TB 1 (front)
    if (((int)__tactile_touch->button == 1) && ((int)__tactile_touch->state == 1))
    {
        cout << "TB " << (int)__tactile_touch->button << " touched" << endl;

        //reinl.Motion();
        cout<<"goal value is "<<goal<<endl;
        reinl.GoalkeeperPos(goal);
        reinl.Train();
        reinl.SavePara();

    }

}

// callback function for goal keeper detectin
void arucoCB(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
    //cout << "Here is aruco call back function"<<endl;
    //cout << "left: " << msg->data[0] << "right: " << msg->data[1] << " middle: " << msg->data[2] << endl;
    goal =( msg->data[2]-msg->data[0])/(msg->data[1]-msg->data[0]);
    //cout<<"goal is: "<<goal<<endl;




}

void currentHipStateCB(const std_msgs::Float32::ConstPtr& msg){
     hip_roll=msg->data;
    //cout<< "current hip roll "<<hip_roll<<endl;

}


int QLEARNING::current_state_message() //!!!!!new
{
    //get message from central node here:
    int exact_joint_state;
    double roll_joint;
    ros::spinOnce();
    sleep(1);
    roll_joint=hip_roll;
    exact_joint_state = (int)((hip_roll+0.0552659+0.025)/(0.05));
    exact_joint_state = 4 - exact_joint_state;
    cout<<"exact state"<<exact_joint_state<<endl;
    cout<<"current_hip_roll"<<hip_roll<<endl;
    return exact_joint_state;
}


 int main(int argc, char** argv)
	{
        ros::init(argc, argv, "QLearning");
        ros::NodeHandle nh_;
        ros::Subscriber tactile_sub;
        ros::Subscriber aruco_position_x_;
        ros::Subscriber current_hipp_state_;



        nao_speech_ = nh_.advertise<std_msgs::String>("speech", 10);

        // subscribe to get goalkeeper postion
        aruco_position_x_=nh_.subscribe("/aruco_positon_x",5,arucoCB);
        current_hipp_state_=nh_.subscribe("/current_hipp_state",5,currentHipStateCB);
        // subscribe to tactile and touch sensors
        tactile_sub = nh_.subscribe("tactile_touch", 1, tactileCB);




       /* while(ros::ok()){
            switch (exc_flag){
                    case 0 :
                           break;
                    case 1 :
                            //reinl.GoalkeeperPos(goal);
                            //reinl.Train();
                            //reinl.SavePara();
                            //reinl.Motion();*/
                            //reinl.Motion();
                            /*break;
                    case 2 :
                            reinl.GoalkeeperPos(goal);
                            reinl.LoadPara();
                            reinl.Predict();
                            sleep(2);
                            break;
                     default :
                              break;

            }

        }*/
        ros::spin();
        return  0;
		
    }
