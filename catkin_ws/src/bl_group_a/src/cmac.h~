#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h> 
#include <stdio.h>
#include "robot_config.h"
using namespace std;

#define Resolution 50
#define N 2 //input dimensions
#define Q 2 //output dimensions
#define Field_Size 5
#define MSE_THRESHOLD 0.00006
#define alpha 0.5
#define nsample 857

static int RFpos[Field_Size][N] = { { 0, 3 }, { 1, 0 }, { 2, 2 }, { 3, 4 }, { 4, 1 } };
//static int RFpos[Field_Size][N] = { { 0, 2 }, { 1, 0 }, { 2, 1 }};
class CMAC
{
public:
    void Predict(double x, double y, double joint_value[]);
    void Train(double a_in[][N], double c_out[][Q]);
    void SavePara();
    void LoadPara();

private:
    void Forward();
    void Init();
    int Position[Field_Size][N];
    double wa[Resolution][Resolution] ; //L2
    double wb[Resolution][Resolution] ; //L2
    double Y[N]; //L1
    double X[Q]; //L3
    double MSE;
    int j;
    int k;
    ofstream fileMSE;

};

void CMAC::Train(double Y_in[nsample][N], double X_out[nsample][Q])
{
    int iter = 1;
    Init(); // intialize Layer 2, assign random value to 'W'
    fileMSE.open("data/MSE_ROBOT.txt");
    while (1)
    {
        MSE = 0;
        for (int m = 0; m < nsample; m++)
        {
            //get the input data
            for (int c = 0; c < N; c++)
            {
                Y[c] = Y_in[m][c];
            }
            Forward();
            //update at each sample
            for (int r = 0; r < Field_Size; r++)
            {
                j = Position[r][0];
                k = Position[r][1];
                wa[j][k] += alpha * (X_out[m][0] - X[0]) / Field_Size;
            }
            for (int r = 0; r < Field_Size; r++)
            {
                j = Position[r][0];
                k = Position[r][1];
                wb[j][k] += alpha * (X_out[m][1] - X[1]) / Field_Size;
            }
            double output_error = 0;
            //MSE
            for (int i = 0; i < Q; i++){
                output_error += (X_out[m][i] - X[i]) * (X_out[m][i] - X[i]);
            }
            MSE += 0.5 * output_error;
        }//end for nsample loop
        MSE = MSE / (nsample * 2);
        if (iter % 1 == 0)
        {
            cout << iter << "th iteration!" << endl << MSE << endl;
            fileMSE << MSE << endl;
        }
        if (MSE < MSE_THRESHOLD)
        {
            cout << "seccessful trained!" << endl; break;
        }
        iter++;
    }//end while loop
    fileMSE.close();
}

void CMAC::Init()  //assign random value to 'W'
{
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            wa[j][k] = (double)rand() / (double)(RAND_MAX)* 2 - 1;
            wb[j][k] = (double)rand() / (double)(RAND_MAX)* 2 - 1;
        }
    }

}

void CMAC::Forward()
{
    int local_coord;
    int shift_amount;
    double input_index_double;
    int input_index;
    int coord;
    int j;
    int k;
    for (int r = 0; r < Field_Size; r++)
    {
        int Coord[N];
        for (int c = 0; c < N; c++)
        {
            input_index_double = round(Y[c] * Resolution);
            input_index = int(input_index_double);
	    //handle with the left & bottom border:
            if (input_index > (Resolution-Field_Size)){ input_index = (Resolution-Field_Size); } 
            shift_amount = Field_Size - input_index % Field_Size;
            local_coord = (shift_amount + RFpos[r][c]) % Field_Size;
            coord = input_index + local_coord;
            Coord[c] = coord;
            Position[r][c] = coord;
        }
    }

    X[0] = 0;
    for (int r = 0; r < Field_Size; r++)
    {
        j = Position[r][0];
        k = Position[r][1];
        X[0] += wa[j][k];
    }
    X[1] = 0;
    for (int r = 0; r < Field_Size; r++)
    {
        j = Position[r][0];
        k = Position[r][1];
        X[1] += wb[j][k];
    }
}

void CMAC::SavePara()
{
    ofstream inFile;
    inFile.open("data/W1.txt", ios::trunc);
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            inFile << wa[j][k] << "\t";
        }
        inFile << endl;
    }
    inFile.close();
    inFile.open("data/W2.txt", ios::trunc);
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            inFile << wb[j][k] << "\t";
        }
        inFile << endl;
    }
    inFile.close();
}

void CMAC::LoadPara()
{
    ifstream file;
    file.open("data/W1.txt");
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            file >> wa[j][k] ;
        }
    }
    file.close();
    file.open("data/W2.txt");
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            file >> wb[j][k];
        }
    }
    file.close();
}

void CMAC::Predict(double x, double y, double joint_value[])
{

    Y[0] = x;
    Y[1] = y;

    int local_coord;
    int shift_amount;
    int input_index;
    int coord;
    int j;
    int k;
    for (int r = 0; r < Field_Size; r++)
    {
        int Coord[N];
        for (int c = 0; c < N; c++)
        {
            input_index = static_cast <int>(round(Y[c] * Resolution));
            shift_amount = Field_Size - input_index % Field_Size;
            local_coord = (shift_amount + RFpos[r][c]) % Field_Size;
            coord = input_index + local_coord;
            Coord[c] = coord;
            Position[r][c] = coord;
        }
    }
        ifstream file;
    file.open("W1.txt");
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            file >> wa[j][k] ;
        }
    }
    file.close();
    file.open("W2.txt");
    for (int j = 0; j < Resolution; j++)
    {
        for (int k = 0; k < Resolution; k++)
        {
            file >> wb[j][k];
        }
    }
    file.close();
        X[0] = 0;
        for (int r = 0; r < Field_Size; r++)
        {
            j = Position[r][0];
            k = Position[r][1];
            X[0] += wa[j][k];
        }
        X[1] = 0;
        for (int r = 0; r < Field_Size; r++)
        {
            j = Position[r][0];
            k = Position[r][1];
            X[1] += wb[j][k];
        }
    joint_value[0] = X[0];
    joint_value[1] = X[1];
    cout << "Prediction:" << endl << joint_value[0] << " " << joint_value[1]  << endl;
}
