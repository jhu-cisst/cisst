% Date: 2014-03-28
% Zihan Chen 

clc; clear; 
close all;

% get data for robQuintic
fid = fopen('LinearRnLog.txt', 'r');
dataLinear = textscan(fid, ['t = %f q = %f %f \n']);
tLinear = dataLinear{1};
qLinear(:,1) = dataLinear{2};
qLinear(:,2) = dataLinear{3};
fclose(fid);

% get data for robQuintic
fid = fopen('QuinticRnLog.txt', 'r');
dataQuintic = textscan(fid, ['t = %f q = %f %f \n']);
tQuintic = dataQuintic{1};
qQuintic(:,1) = dataQuintic{2};
qQuintic(:,2) = dataQuintic{3};
fclose(fid);



%% plot
plot3(tLinear, qLinear(:,1), qLinear(:,2));
grid on;
xlabel('Time (sec)');
ylabel('Joint 1 (rad)');
zlabel('Joint 2 (rad)');

hold on;
plot3(tQuintic, qQuintic(:,1), qQuintic(:,2), 'red');  

text(tLinear(1), qLinear(1, 1) + 0.1, qLinear(1,2) + 0.2, 'Start');
text(tLinear(length(tLinear)), ...
    qLinear(length(tLinear), 1) + 0.1, ...
    qLinear(length(tLinear),2) + 0.2, 'Stop');

legend('robLinearRn', ...
       'robQuintic');