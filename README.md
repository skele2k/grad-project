# SSD performance improvement through deep learning-based workload analysis
2023 Pusan National University Graduation Project


### Contributors: 
> ### 최성찬 (201824641)

> ### Odgerel Ariunbold (201824623)

> ### Ganchuluun Narantsatsralt (201824621)

```data``` folder contains the data to train and currently it is only the sample file. 

  ```data/traceProcess``` is a script to process whole data from ssdtrace-00 to ssdtrace-26 at once

  ```data/ssdtrace.gz``` is a gz file which is output of above traceProcess script. This will be used for training model.

  ```data/labeling``` folder contains the labeled data and codes to lable the datas.

  ```data/labeling/Sector.cpp``` is a code to calculate frequency, access interval, and overall i/o size.

```src/``` only source codes must be included in the subfolder of ```src```   

```src/greedyGC``` Garbage Collection algorithm implementation ```src```


```src/model``` Deep learning models

```src/model/cpp``` Deployment of LSTM Model to C++ environment. 

```src/utils``` other useful scripts 
