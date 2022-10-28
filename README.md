# Logic-Sampler

A pair of programs (for the moment) that enable a "cheap as chips" approach to implementing a logic analyser for I2C communications.

The two programs are a sketch for an Arduino Mega 2560 which performs capture of binary data and a companion Linux command line program
which performs post-processing to include a basic analysis of any I2C protocol identified.

## LogicAnalyser.ino

This basic script works in a simple manner:  It waits for input after which (on reciept) it proceeds to capture as much data from port K (all
8 bits) into memory.  Once the memory buffer is full it outputs a data an "ASCII Art" version of the data captured to allow a visual analysis
of the data caught.

At this point a futher capture and display cycle can be initiated with further input (notionally just pressing enter).

The output looks something like the following:
```
Digital Sampler
(press key to sample)
Sampling!

index   delta   sample  7       6       5       4       3       2       1       0
0       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
1       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
2       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
3       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
4       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
5       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
6       250     96      | .     . |     . |     | .     | .     | .     | .     | .  
```
Futher down more interesting output would resemble this:
```
64      250     96      | .     . |     . |     | .     | .     | .     | .     | .
65      250     96      | .     . |     . |     | .     | .     | .     | .     | .
66      250     96      | .     . |     . |     | .     | .     | .     | .     | .
67      250     96      | .     . |     . |     | .     | .     | .     | .     | .
68      250     96      | .     . |     ._|     | .     | .     | .     | .     | .
69      2       64      | .     ._|     | .     | .     | .     | .     | .     | .
70      2       0       | .     |_.     | .     | .     | .     | .     | .     | .
71      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .
72      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
73      2       96      | .     ._|     ._|     | .     | .     | .     | .     | .
74      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
75      2       64      | .     ._|     | .     | .     | .     | .     | .     | .
76      2       0       | .     |_.     | .     | .     | .     | .     | .     | .
77      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .
78      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
79      2       96      | .     ._|     . |     | .     | .     | .     | .     | .
80      1       32      | .     |_.     . |     | .     | .     | .     | .     | .
81      2       96      | .     ._|     . |     | .     | .     | .     | .     | .
82      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
83      2       96      | .     ._|     . |     | .     | .     | .     | .     | .
84      1       32      | .     | .     ._|     | .     | .     | .     | .     | .
85      0       0       | .     |_.     | .     | .     | .     | .     | .     | .
86      1       64      | .     ._|     | .     | .     | .     | .     | .     | .
87      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
88      2       64      | .     ._|     | .     | .     | .     | .     | .     | .
89      2       0       | .     |_.     | .     | .     | .     | .     | .     | .
90      2       64      | .     ._|     | .     | .     | .     | .     | .     | .
91      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
92      3       64      | .     ._|     | .     | .     | .     | .     | .     | .
93      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
94      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .
95      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
96      2       96      | .     ._|     . |     | .     | .     | .     | .     | .
97      1       32      | .     |_.     . |     | .     | .     | .     | .     | .
98      2       96      | .     ._|     . |     | .     | .     | .     | .     | .
99      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
100     2       96      | .     ._|     ._|     | .     | .     | .     | .     | .
101     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
102     2       64      | .     ._|     | .     | .     | .     | .     | .     | .
103     1       0       | .     | .     |_.     | .     | .     | .     | .     | .
104     0       32      | .     |_.     . |     | .     | .     | .     | .     | .
105     0       96      | .     ._|     ._|     | .     | .     | .     | .     | .
106     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
107     2       64      | .     ._|     | .     | .     | .     | .     | .     | .
108     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
109     2       64      | .     . |     |_.     | .     | .     | .     | .     | .
110     2       96      | .     . |     . |     | .     | .     | .     | .     | .
111     250     96      | .     . |     . |     | .     | .     | .     | .     | .
112     250     96      | .     . |     . |     | .     | .     | .     | .     | .
113     250     96      | .     . |     . |     | .     | .     | .     | .     | .
```
The above is an I2C Bus communication with the Clock on pin 6 and the Data on pin 5.

The three leading numbers are as follows:

* The sample index number 0 to 2999 (at the moment)
* The 'skipped sampes' between this sample and the previous sample (maximum 250)
* The raw didgital value read from the input port (current port K)

At this level there is no analysis of the data.

For me, running under Linux, the best approach to using this sketch is to use minicom with the following command line:
```
minicom -b 115200 -D /dev/ttyUSB0 -C samples.data
```
This provides the interaction with the Arduino required to start a sampling processs and
simultaeously captures the resulting data in a local file (sample.data) for further analysis.


## replay_i2c.c

This is a simple minded program reading from stdin writing to stdout, expecting the output from the Arduino Sketch
and appending to the end of lines any I2C related analysis it recognises.

The program takes two numerical arguments; the first is the bit number of the I2S clock the seconds the bit number
of the I2C data.

Thus, to analyse I2C data captured on pins 6 and 5 I run the following
```
./replay_i2c 6 5 < ../samples.data | more
```
The program outputs the input sample data (as generated by the above sketch) and appends text for I2C actions.  Picking
the same section of samples as above I get the following output:
```
64      250     96      | .     . |     . |     | .     | .     | .     | .     | .
65      250     96      | .     . |     . |     | .     | .     | .     | .     | .
66      250     96      | .     . |     . |     | .     | .     | .     | .     | .
67      250     96      | .     . |     . |     | .     | .     | .     | .     | .
68      250     96      | .     . |     ._|     | .     | .     | .     | .     | .
69      2       64      | .     ._|     | .     | .     | .     | .     | .     | .     START
70      2       0       | .     |_.     | .     | .     | .     | .     | .     | .
71      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .     0
72      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
73      2       96      | .     ._|     ._|     | .     | .     | .     | .     | .     1
74      1       0       | .     |_.     | .     | .     | .     | .     | .     | .     
75      2       64      | .     ._|     | .     | .     | .     | .     | .     | .     0
76      2       0       | .     |_.     | .     | .     | .     | .     | .     | .     
77      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .     0
78      2       32      | .     |_.     . |     | .     | .     | .     | .     | .     
79      2       96      | .     ._|     . |     | .     | .     | .     | .     | .     1
80      1       32      | .     |_.     . |     | .     | .     | .     | .     | .     
81      2       96      | .     ._|     . |     | .     | .     | .     | .     | .     1
82      2       32      | .     |_.     . |     | .     | .     | .     | .     | .     
83      2       96      | .     ._|     . |     | .     | .     | .     | .     | .     1
84      1       32      | .     | .     ._|     | .     | .     | .     | .     | .     
85      0       0       | .     |_.     | .     | .     | .     | .     | .     | .
86      1       64      | .     ._|     | .     | .     | .     | .     | .     | .     0
87      1       0       | .     |_.     | .     | .     | .     | .     | .     | .     
88      2       64      | .     ._|     | .     | .     | .     | .     | .     | .     Ack adrs:39 Write
89      2       0       | .     |_.     | .     | .     | .     | .     | .     | .
90      2       64      | .     ._|     | .     | .     | .     | .     | .     | .     0
91      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
92      3       64      | .     ._|     | .     | .     | .     | .     | .     | .     0
93      1       0       | .     |_.     | .     | .     | .     | .     | .     | .
94      2       64      | .     ._|     |_.     | .     | .     | .     | .     | .     0
95      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
96      2       96      | .     ._|     . |     | .     | .     | .     | .     | .     1
97      1       32      | .     |_.     . |     | .     | .     | .     | .     | .
98      2       96      | .     ._|     . |     | .     | .     | .     | .     | .     1
99      2       32      | .     |_.     . |     | .     | .     | .     | .     | .
100     2       96      | .     ._|     ._|     | .     | .     | .     | .     | .     1
101     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
102     2       64      | .     ._|     | .     | .     | .     | .     | .     | .     0
103     1       0       | .     | .     |_.     | .     | .     | .     | .     | .
104     0       32      | .     |_.     . |     | .     | .     | .     | .     | .
105     0       96      | .     ._|     ._|     | .     | .     | .     | .     | .     1
106     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
107     2       64      | .     ._|     | .     | .     | .     | .     | .     | .     Ack data:29
108     2       0       | .     |_.     | .     | .     | .     | .     | .     | .
109     2       64      | .     . |     |_.     | .     | .     | .     | .     | .     0
110     2       96      | .     . |     . |     | .     | .     | .     | .     | .     STOP
111     250     96      | .     . |     . |     | .     | .     | .     | .     | .
112     250     96      | .     . |     . |     | .     | .     | .     | .     | .
113     250     96      | .     . |     . |     | .     | .     | .     | .     | .
```

So far this seems to be accurate and useful though more work is required.
