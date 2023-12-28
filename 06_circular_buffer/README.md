#### Circular Buffer

Two pointers are used:
* Head pointer  (for _insetion_)
* Tail pointer  (for _reading_)

* When data is **received** _head pointer_ is _incremented by 1_
* When data is **read** _tail pointer_ is _incremented by 1_


##### Example:

Sending the string _"http"_ to the circular buffer

* String length = 4
* After sending the entire string, **head pointer** = 4 (tail pointer = 0 since we have not read from the buffer)
* After reading the entire buffer, **tail pointer** = 4

##### Finding the next location after head:

```c
location = (buffer->head + 1) % buffer_size

head = 4
buffer_size = 6

location = (buffer->head + 1) % buffer_size
location = (4 + 1) % 6
location = 5 % 6
location = 5
```   


##### No new data:

`buffer->head == buffer->tail`


##### Reading data:

1. Read data at tail position into a variable
`char new_data = buffer->data[buffer->tail]`

2. Compute new tail position
`buffer->tail = (buffer->tail + 1) % buffer_size`   


##### Writing data:

1. Write data at head position  
`buffer->data[buffer->head] = new_data

2. Compute new head position  
`buffer->head` = (buffer->head + 1) % buffer_size 