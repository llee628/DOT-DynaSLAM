#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core.hpp>
#include <fstream>
#include "yolo.h"
#include <vector>
#include <math.h> 
#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace dnn;
using namespace std;

const int DOT_THREAD = 3;

namespace yolov3 {

    yolov3Segment::yolov3Segment() {
        // Load names of classes
        string classesFile = "src/yolo/coco.names";
        ifstream ifs(classesFile.c_str());
        string line;
        while (getline(ifs, line)) classes.push_back(line);
        
        // Give the configuration and weight files for the model
        String modelConfiguration = "src/yolo/yolov4.cfg";
        String modelWeights = "src/yolo/yolov4.weights";
        

        // Load the network
        net = readNetFromDarknet(modelConfiguration, modelWeights);
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);
    }


    cv::Mat yolov3Segment::Segmentation(cv::Mat &image, cv::Mat &image2) {
        cv::Mat blob;
        // Create a 4D blob from a frame.
        blobFromImage(image, blob, 1/255.0, cvSize(this->inpWidth, this->inpHeight), Scalar(0,0,0), true, false);
        //Sets the input to the network
        this->net.setInput(blob);
        // Runs the forward pass to get output of the output layers
        vector<Mat> outs;
        this->net.forward(outs, this->getOutputsNames(this->net));

        int dilation_size = 15;
        cv::Mat kernel = getStructuringElement(cv::MORPH_ELLIPSE,
                                            cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                            cv::Point( dilation_size, dilation_size ) );
        cv::Mat mask = cv::Mat::ones(480,640,CV_8U);
        cv::Mat maskyolo = this->postprocess(image, image2, outs);
        cv::Mat maskyolodil = maskyolo.clone();
        cv::dilate(maskyolo, maskyolodil, kernel);
        mask = mask - maskyolodil;
        return mask;
    }


    // Remove the bounding boxes with low confidence using non-maxima suppression
    cv::Mat yolov3Segment::postprocess(Mat& frame, Mat& frame2, const vector<Mat>& outs)
    {
        vector<int> classIds;
        vector<float> confidences;
        vector<Rect> boxes;
        
        for (size_t i = 0; i < outs.size(); ++i)
        {
            // Scan through all the bounding boxes output from the network and keep only the
            // ones with high confidence scores. Assign the box's class label as the class
            // with the highest score for the box.
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            {
                Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                Point classIdPoint;
                double confidence;
                // Get the value and location of the maximum score
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > this->confThreshold)
                {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;
                    
                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(Rect(left, top, width, height));
                }
            }
        }
        
        cv::Mat mask = cv::Mat::zeros(480,640,CV_8U);
        // Perform non maximum suppression to eliminate redundant overlapping boxes with
        // lower confidences
        vector<int> indices;
        NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);

        // detection Dynamic optical points
        cv::Mat DynamicPts;
       

        for (size_t i = 0; i < indices.size(); ++i)
        {
            int idx = indices[i];
            Rect box = boxes[idx];
            //drawPred(classIds[idx], confidences[idx], box.x, box.y,
            //         box.x + box.width, box.y + box.height, frame);
            string c = this->classes[classIds[idx]];
            if (c == "person") {
                DynamicPts = opticalFlowDetect(frame, frame2);
                // detect number of moving objects
                int count = 0;
                int com_num = DynamicPts.cols;
    
                //  ********************************************
                // This is the block that cause seg fault
                for(int im = 0; im < com_num; im++){
                    if ( (DynamicPts.at<int>(0,im) >= max(0, box.x)) && ((DynamicPts.at<int>(0,im) < box.x+ box.width)) && (DynamicPts.at<int>(1,im) >= max(0, box.y)) && ((DynamicPts.at<int>(1,im)  < box.y+ box.height)) )
                    {
                        count++;
                        //cout<<"count = "<<count<<endl;

                        // if (count > 0){
                        //     for (int x = max(0, box.x); x < box.x + box.width && x < 640; ++x)
                        //         for (int y = max(0, box.y); y < box.y + box.height && y < 480; ++y)
                        //             mask.at<uchar>(y, x) = 1;
                        // }
                    }
                } 
                // ************************************************
                // test version, without threshold setup
                cout<<"count = "<<count<<endl;
                if(count>DOT_THREAD){
                    for (int x = max(0, box.x); x < box.x + box.width && x < 640; ++x)
                        for (int y = max(0, box.y); y < box.y + box.height && y < 480; ++y)
                            mask.at<uchar>(y, x) = 1;  
                }
            }
        }

        return mask;
    }

    // Get the names of the output layers
    vector<String> yolov3Segment::getOutputsNames(const Net& net){
        static vector<String> names;
        if (names.empty())
        {
            //Get the indices of the output layers, i.e. the layers with unconnected outputs
            vector<int> outLayers = this->net.getUnconnectedOutLayers();
            
            //get the names of all the layers in the network
            vector<String> layersNames = this->net.getLayerNames();
            
            // Get the names of the output layers in names
            names.resize(outLayers.size());
            for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
        }
        return names;
    }

    cv::Mat yolov3Segment::opticalFlowDetect(cv::Mat& old_frame, cv::Mat& frame){
        vector<Point2f> p0, p1;

        Mat old_gray, frame_gray;
        cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
        goodFeaturesToTrack(old_gray, p0, 0, 0.3, 7, Mat(), 7, false, 0.04);
        
        Mat mask = Mat::zeros(old_frame.size(), old_frame.type());
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // calculate optical flow
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
        if(p0.size() < 30 || true)
        {   
            cout << "FEW POINTS\n";
            goodFeaturesToTrack(old_gray, p0, 0, 0.3, 7, Mat(), 7, false, 0.04);

        }

        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
        
        vector<Point2f> good_new;
        
        int distance[p0.size()];
        cv::Mat mvpts = cv::Mat_<int>(2,p0.size()); // used to store position of moving features
        for(uint i = 0; i < p0.size(); i++)
        {
            
            // Select good points
            if(status[i] == 1) {
                good_new.push_back(p1[i]);
                // draw the tracks
                // line(mask,p1[i], p0[i], colors[i], 2);
                // circle(frame, p1[i], 5, colors[i], -1);

                distance[i] = sqrt( (p0[i].x - p0[i].y) + (p1[i].x - p1[i].y) );
                // set threshold to 3 (maybe need to tune)
                if(distance[i] > 3){ // moving
                    mvpts.at<int>(0,i) = std::round(p1[i].x);
                    mvpts.at<int>(1,i) = std::round(p1[i].y);
                }else{
                    mvpts.at<int>(0,i) = -1;
                    mvpts.at<int>(1,i) = -1;
                }
            }
        }

        return mvpts; 
    }

}