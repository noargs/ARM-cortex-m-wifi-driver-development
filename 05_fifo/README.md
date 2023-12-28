#### FIFO (First-in-first-out) Data Structure

We need the data structure to manage both the packets, received and sent out. 

* It is used to link _producers_ to _consumers_ (i.e. modules that produce data and modules that consume data)

    `fifo_put()`                     `fifo_get()`
   ------------->    [ **FIFO** ]    -------------->
    producer                         consumer
     
##### FIFO: Example of producers and consumers

| Producer               | Consumer                 |
|:----------------------:|:------------------------:|
| Keyboard input         | Program that displays it |
| Program sends messages | Program receive messages |     

We will run the code on STM32 board and use Serial capture program _RealTerm_ for Windows or _SerialTools_ for MacOS to get debug output. In order for debug output to be seen correctly we will keep **Baud Rate** as 115200 **Data bits** as 8, **Stop bits** 1 and **Parity** none