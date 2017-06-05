clear;close all;clc;
MSE = double(load('MSE_ROBOT_3_150.txt'));

MSE2 = [[1:31]',MSE];
plot(MSE);title('MSE  Field Size = 3, Samples = 150')
hold on

plot(MSE2(1,1),MSE2(1,2),'g*');

plot(MSE2(2,1),MSE2(2,2),'g*');

plot(MSE2(3,1),MSE2(3,2),'g*');

plot(MSE2(4:3:end,1),MSE2(4:3:end,2),'g*');