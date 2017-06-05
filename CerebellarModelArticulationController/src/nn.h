#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h> 
#include <stdio.h>
#include "robot_config.h"
using namespace std;


#define N 2 //dimension of INPUT
#define P 8 //dimension of hidden layer
#define Q 2 //dimension of OUTPUT
#define m 30 //dimension of pairs 
#define MSE_THRESHOLD 0.00003

class FFNN{ //define ForwardFeed Neuronal Network

public:
	void Predict(double x, double y,double joint_value []); //predict output by using the trained NN
	void Train(double a_in[][N], double c_out[][Q]);
	void SavePara();
	void LoadPara();

private:
	void InitNetWork();         //initialize network
	double Sigmoid(double x);
	void FeedForwrd();
	void CalDelta();
	void Update();

private: 
	double alpha = 0.5; //learning rate
	double beta = 0.5;

	double a[N];
	double b[P];
	double c[Q];
	double d[Q];
	double e[P];

	double V[N][P];
	double W[P][Q];
	double theta[P];
	double fai[Q];
	double MSE;
	double dV[N][P];
	double dW[P][Q];
	double dtheta[P];
	double dfai[Q];
	ofstream fileMSE;
};


void FFNN::Train(double a_in[][N], double c_out[][Q]){
	cout << "Begin to train BP NetWork!" << endl;
	InitNetWork();

	//use m-dimensions pairs to train
	int iter = 1;
	fileMSE.open("data/MSE_ROBOT.txt");
	while (1)
	{
		MSE = 0;
		for (int h = 0; h < N; h++){
			for (int i = 0; i < P; i++){
				dV[h][i] = 0;
				dtheta[i] = 0;
			}
		}
		for (int i = 0; i < P; i++){
			for (int j = 0; j < Q; j++){
				dW[i][j] = 0;
				dfai[j] = 0;
			}
		}


		
		iter++;
		for (int k = 0; k < m; k++)
		{
			//get the input data
			for (int h = 0; h < N; h++)
			{
				a[h] = a_in[k][h];

			}
			
			FeedForwrd();
			//CalDelta();
			for (int j = 0; j < Q; j++)
			{
				d[j] = c[j] * (1 - c[j]) * (c_out[k][j] - c[j]);
				
			}

			for (int i = 0; i < P; i++)
			{
				double sum_Wd = 0;
				for (int j = 0; j < Q; j++)
				{
					sum_Wd += W[i][j] * d[j];
				}
				e[i] = b[i] * (1 - b[i]) * sum_Wd;
				
			}

			//caldelta
			for (int i = 0; i < P; i++)
			{
				for (int j = 0; j < Q; j++)
				{
					dW[i][j] += alpha * b[i] * d[j];
					dfai[j] += alpha * d[j];
				}
			}
			for (int i = 0; i < P; i++)
			{
				for (int h = 0; h < N; h++)
				{
					dV[h][i] += beta * a[h] * e[i];
					
					dtheta[i] += beta * e[i];
				}
			}

			double output_error = 0;
			//MSE
			for (int j = 0; j < Q; j++){
				output_error += (c_out[k][j] - c[j]) * (c_out[k][j] - c[j]);
			}
			MSE += 0.5 * output_error;
		}
		MSE = MSE / (m * Q);
		for (int i = 0; i < P; i++)
		{
			for (int j = 0; j < Q; j++)
			{
				W[i][j] += dW[i][j] / m;
				fai[j] += dfai[j] / m;
			}
		}
		for (int i = 0; i < P; i++)
		{
			for (int h = 0; h < N; h++)
			{
				V[h][i] += dV[h][i] / m;
				theta[i] += dtheta[i] / m;
			}
		}
		if (iter % 10000 == 0){
			cout << "this is " << iter << "th iteration!" << endl;
			cout << MSE << endl;
			
				
			fileMSE << MSE << endl;
			
		}
		
		if (MSE<MSE_THRESHOLD) { cout << "seccessful trained!" << endl; break; }
	}
	fileMSE.close();
}


void FFNN::InitNetWork(){
	for (int h = 0; h < N; h++){
		for (int i = 0; i < P; i++){
			V[h][i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2-1;
			theta[i] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2-1;
		}
	}
	for (int i = 0; i < P; i++){
		for (int j = 0; j < Q; j++){
			W[i][j] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2-1;
			fai[j] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2-1;
		}
	}
	
}

double FFNN::Sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}

void FFNN::FeedForwrd(){
	for (int i = 0; i < P; i++){
		double sum_to_b = 0;
		for (int h = 0; h < N; h++){
			sum_to_b += a[h] * V[h][i];
		}
		b[i] = Sigmoid(sum_to_b + theta[i]);
		
	}

	for (int j = 0; j < Q; j++){
		double sum_to_c = 0;
		for (int i = 0; i < P; i++){
			sum_to_c += b[i] * W[i][j];
		}
		c[j] = Sigmoid(sum_to_c + fai[j]);
	}

}



void FFNN::SavePara()
{
	ofstream inFile;
	
	inFile.open("data/V.txt", ios::trunc);
	for (int h = 0; h < N; h++){
		for (int i = 0; i < P; i++){
			inFile << V[h][i] << "\t";
		}
		inFile << endl;
	}
	inFile.close();

	inFile.open("data/theta.txt", ios::trunc);
	for (int i = 0; i < P; i++){
		inFile << theta[i] << "\t";
	}
	inFile.close();

	inFile.open("data/W.txt", ios::trunc);
	for (int i = 0; i < P; i++){
		for (int j = 0; j < Q; j++){
			inFile << W[i][j] << "\t";
		}
		inFile << endl;
	}
	inFile.close();

	inFile.open("data/fai.txt", ios::trunc);
	for (int j = 0; j < Q; j++){
		inFile << fai[j]<<"\t";
	}
	
	inFile.close();
}



void FFNN::LoadPara(){
	ifstream file;
	file.open("data/V.txt");
	for (int h = 0; h < N; h++){
		for (int i = 0; i < P; i++){
			file >> V[h][i];
			
		}
	}
	file.close();

	file.open("data/W.txt");
	for (int i = 0; i < P; i++){
		for (int j = 0; j < Q; j++){
			file >> W[i][j];
			
		}
	}
	file.close();

	file.open("data/theta.txt");
	for (int i = 0; i < P; i++){
		file >> theta[i];
	}
	file.close();

	file.open("data/fai.txt");
	for (int j = 0; j < Q; j++){
		file >> fai[j];
	}
	file.close();
}

void FFNN::Predict(double x, double y,double joint_value [])
{
	
	a[0] = x;
	a[1] = y;
	cout << "input" << a[0] << "\t" <<a[1]<< endl;

	for (int i = 0; i < P; i++){
		double sum_to_b = 0;
		for (int h = 0; h < N; h++){
			sum_to_b += a[h] * V[h][i];
		}
		b[i] = Sigmoid(sum_to_b + theta[i]);
	}
	for (int j = 0; j < Q; j++){

		double sum_to_c = 0;
		for (int i = 0; i < P; i++){
			sum_to_c += b[i] * W[i][j];
		}
		c[j] = Sigmoid(sum_to_c + fai[j]);

	}


        joint_value[0]=c[0];
        joint_value[1]=c[1];

	
	
}
