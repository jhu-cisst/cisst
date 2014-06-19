% Date: 2014-03-28
% Zihan Chen 

clc; clear; 
close all;

% copyfile('/PATH/TO/FunctionRnLog.txt', './');

% open file & preprocess 
fid = fopen('LinearRnLog.txt', 'r');

% scan
data = textscan(fid, ['t = %f q = %f %f \n']);

t = data{1};
q(:,1) = data{2};
q(:,2) = data{3};


% plot
plot(q(:,1), q(:,2));
grid on;
xlabel('Joint 1');
ylabel('Joint 2');
                     