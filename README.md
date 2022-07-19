# FTL Optimization for Deep-Learning based workloads
2023 Pusan National University Graduation Project


### Contributors: 
> ### 최성찬 (201824641)
> **Contact info - 010-xxxx-xxxx**

> ### Odgerel Ariunbold (201824623)
> **Contact info - 010-xxxx-xxxx**

> ### Ganchuluun Narantsatsralt (201824621)
> **Contact info - 010-7591-0810**

```data``` folder contains the data to train and currently it is only the sample file.   
```data/traceProcess``` is a script to process whole data from ssdtrace-00 to ssdtrace-26 at once
```data/ssdtrace.gz``` is a gz file which is output of above traceProcess script. This will be used for training model.
```data/labeling``` folder contains the labeled data and codes to lable the datas.
```data/labeling/Sector.cpp``` is a code to calculate frequency, access interval, and overall i/o size.

```src/``` only source codes must be included in the subfolder of ```src```

```src/model``` folder contains the deep learning and machine learning models to be implemented

```src/utils``` if you need any other script place the script in this folder
