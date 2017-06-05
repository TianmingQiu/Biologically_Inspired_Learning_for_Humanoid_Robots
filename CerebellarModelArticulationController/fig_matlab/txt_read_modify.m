clear;close all;clc;
Object_Center = load('Object_Center.txt') ;
Joint_value = load('Joint_value.txt');

A = Joint_value(1:5:750,:);
B = Object_Center(1:5:750,:);

fid = fopen('Joint_value_150.txt','wt');
fprintf(fid,'%g\t%g\n',A');      
fclose(fid);

fid2 = fopen('Object_Center_150.txt','wt');
fprintf(fid2,'%g\t%g\n',B');      
fclose(fid2);

