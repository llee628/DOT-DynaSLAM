%% load data
close all
clear
clc


% ground truth
% change dir
%fileID = fopen('groundtruth_halfsphere.txt','r');
fileID = fopen('sitting_groundtruth.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_gt = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_gt = traj_gt';
traj_gt(:, 2:4) = traj_gt(:, 2:4) - traj_gt(1, 2:4);

% DynaSLAM: Mask RCNN
fileID = fopen('KeyFrameTrajectory_maskRCNN.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj = traj';
% 
% % DynaSLAM with YOLOv4
% fileID = fopen('YOLO_sitting_xyz.txt','r');
% formatSpec = '%f %f %f %f %f %f %f %f';
% sizeA = [8 Inf];
% traj_yolo = fscanf(fileID,formatSpec,sizeA);
% fclose(fileID);
% traj_yolo = traj_yolo';

% DynaSLAM with YOLOv4 + Dynamic object detection
fileID = fopen('KeyFrameTrajectory_DynaYOLO_sitting.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_yolo_DOT = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_yolo_DOT = traj_yolo_DOT';

% ORB_SLAM2
fileID = fopen('sitting_naive.txt','r');
formatSpec = '%f %f %f %f %f %f %f %f';
sizeA = [8 Inf];
traj_orb = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
traj_orb = traj_orb';

%% plot
%%{
figure(1);
plot3(traj_gt(:,2), traj_gt(:,3), traj_gt(:,4), 'linewidth', 1.5);
hold on;
plot3(15*traj(:,2), 15*traj(:,3), 15*traj(:,4), 'linewidth', 1.5);

% plot3(15*traj_yolo(:,2)-0.4, 15*traj_yolo(:,3), 15*traj_yolo(:,4), 'linewidth', 1.5);

plot3(7*traj_yolo_DOT(:,2), 7*traj_yolo_DOT(:,3), 7*traj_yolo_DOT(:,4), 'linewidth', 1.5);

plot3(7*traj_orb(:,2), 7*traj_orb(:,3), 7*traj_orb(:,4), 'linewidth', 1.5);

axis auto, grid on
xlabel('$X$', 'Interpreter', 'latex')
ylabel('$Y$', 'Interpreter', 'latex')
zlabel('$Z$', 'Interpreter', 'latex')
title("sitting\_xyz")

% legend('$Ground\ truth$', '$Dyna\_SLAM$', '$YOLO$', '$YOLO\_DOT$', '$ORB\_SLAM2$','Interpreter', 'latex', 'location', 'best')
legend('$Ground\ truth$', '$Dyna\_SLAM$', '$DOT\_SLAM$', '$ORB\_SLAM2$','Interpreter', 'latex', 'location', 'best')
view(-50,30)
%%}

% if use sensor fusion toolbox
%{
tp = theaterPlot('XLimit',[-2 2],'YLimit',[-2 2],'ZLimit',[-0.5 2]);
op = orientationPlotter(tp,'DisplayName','Fused Data',...
    'LocalAxesLength',2);
for i=1:size(traj_gt, 1)
    %plotOrientation(op, quaternion(traj_yolo(i,5),traj_yolo(i,6),traj_yolo(i,7),traj_yolo(i,8)));
    plotOrientation(op, quaternion(traj_gt(i,5),traj_gt(i,6),traj_gt(i,7),traj_gt(i,8)), [traj_gt(i,2) traj_gt(i,3) traj_gt(i,4)]);
    drawnow
    %pause(0.1);
end
%}