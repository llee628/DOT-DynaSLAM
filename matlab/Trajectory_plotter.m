close all
clear
clc

% ```modified the file name and path to your trajectories.

% ground truth
fileID = fopen('sitting_groundtruth.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_gt = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_gt = traj_gt';
traj_gt(:, 2:4) = traj_gt(:, 2:4) - traj_gt(1, 2:4);

% DynaSLAM: Mask RCNN
fileID = fopen('KeyFrameTrajectory_maskRCNN_sitting.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj = traj';

% DOT SLAM
fileID = fopen("KeyFrameTrajectory_DOT_sitting.txt","r");
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_yolo_DOT = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_yolo_DOT = traj_yolo_DOT';

% ORB_SLAM2
fileID = fopen('KeyFrameTrajectory_orbSLAM_sitting.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_orb = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_orb = traj_orb';

%% plot trajectories
figure(1);
plot3(traj_gt(:,2), traj_gt(:,3), traj_gt(:,4), 'linewidth', 1.5,'Color','#0072BD');
hold on;
plot3(traj(:,2), traj(:,3), traj(:,4), 'linewidth', 1.5,'Color','#EDB120');

plot3(traj_yolo_DOT(:,2), traj_yolo_DOT(:,3), traj_yolo_DOT(:,4), 'linewidth', 1.5, 'Color','#77AC30');

plot3(traj_orb(:,2), traj_orb(:,3), traj_orb(:,4), 'linewidth', 1.5,'Color','r');

axis auto, grid on
xlabel('$X$', 'Interpreter', 'latex')
ylabel('$Y$', 'Interpreter', 'latex')
zlabel('$Z$', 'Interpreter', 'latex')
title("sitting\_xyz")

legend('$Ground\ truth$', '$Dyna\_SLAM$', '$DOT\_SLAM$', '$ORB\_SLAM2$','Interpreter', 'latex', 'location', 'best')
view(-50,35) % uncomment to show 3D image
% view(90,-90) % uncomment to show 2D image of x,y axis.

