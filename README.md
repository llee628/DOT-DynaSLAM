# Baseline model DOT-DynaSLAM

DynaSLAM is a visual SLAM system that is robust in dynamic scenarios for monocular, stereo and RGB-D configurations. Having a static map of the scene allows inpainting the frame background that has been occluded by such dynamic objects.

<img src="imgs/teaser.png" width="900px"/>

DynaSLAM: Tracking, Mapping and Inpainting in Dynamic Scenes   
[Berta Bescos](http://bertabescos.github.io), [José M. Fácil](http://webdiis.unizar.es/~jmfacil/), [Javier Civera](http://webdiis.unizar.es/~jcivera/) and [José Neira](http://webdiis.unizar.es/~jneira/)   
RA-L and IROS, 2018

Based on the work of DynaSLAM, we change the moving object detector with YOLOv4 and use our dynamic object tracking (algorithm) to get and even faster and more accurate results.

We provide examples to run the SLAM system in the [TUM dataset](http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets) as monocular and rgb-d.

## Prerequisites
- We have tested the system in **Ubuntu 18.04** and **20.04**
- gcc >= 7.4.0
- OpenCV >= 3.4.5
- Install ORB-SLAM2 prerequisites: C++11 or C++0x Compiler, Pangolin, OpenCV and Eigen3  (https://github.com/raulmur/ORB_SLAM2)
- Install boost libraries with the command `sudo apt-get install libboost-all-dev`.
- Install python 2.7, keras and tensorflow

## Getting Started
- Clone this repo:
```bash
git clone https://github.com/llee628/DOT-DynaSLAM.git
```
```
cd DOT-DynaSLAM
chmod +x build.sh
./build.sh
```
- Download the 'yolov4.cfg' and 'yolov4.weights' from https://github.com/AlexeyAB/darknet#pre-trained-models and place them in the folder 'DOT-DynaSLAM/src/yolo'

## RGB-D Example on TUM Dataset
- Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it.

- Associate RGB images and depth images executing the python script [associate.py](http://vision.in.tum.de/data/datasets/rgbd-dataset/tools):

  ```
  python associate.py PATH_TO_SEQUENCE/rgb.txt PATH_TO_SEQUENCE/depth.txt > associations.txt
  ```
These associations files are given in the folder `./Examples/RGB-D/associations/` for the TUM dynamic sequences.

- Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER` to the uncompressed sequence folder. Change `ASSOCIATIONS_FILE` to the path to the corresponding associations file. `YOLO`is an optional parameter.

  ```
  ./Examples/RGB-D/rgbd_tum_yolo Vocabulary/ORBvoc.txt Examples/RGB-D/TUMX.yaml PATH_TO_SEQUENCE_FOLDER ASSOCIATIONS_FILE (PATH_TO_YOLO)
  ```
  
If `PATH_TO_YOLO` is **not** provided, only the geometrical approach is used to detect dynamic objects.

If `PATH_TO_YOLO` is provided, YOLOv4 is used to segment the dynamic content of every frame. These masks are saved in the provided folder `PATH_TO_YOLO`. 

## Monocular Example on TUM Dataset
- Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it to the data/.

- Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER`to the uncompressed sequence folder. By providing the last argument `PATH_TO_YOLO`, dynamic objects are detected with YOLOv4.
```
./Examples/Monocular/mono_tum_yolo Vocabulary/ORBvoc.txt Examples/Monocular/TUMX.yaml PATH_TO_SEQUENCE_FOLDER (PATH_TO_YOLO)
```
If `PATH_TO_YOLO` is **not** provided, only the geometrical approach is used to detect dynamic objects.

If `PATH_TO_YOLO` is provided, YOLOv4 is used to segment the dynamic content of every frame. These masks are saved in the provided folder `PATH_TO_YOLO`. 

## Acknowledgements
Our code builds on [ORB-SLAM2](https://github.com/raulmur/ORB_SLAM2) and [DynaSLAM](https://github.com/BertaBescos/DynaSLAM).

# DOT-DynaSLAM
