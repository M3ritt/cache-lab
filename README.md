# cache-lab
This only runs LRU and does not error if the r value is not lru, fifo or optimal but since it only runs lru, I didn't think it was necessary to add an error for this. 
The main runner for this method is 'LRU' where it does majority of the running only calling the other methods. 
'Main' just sets the values for what's given and then calls 'lru'. 
The results statement is printed from the result function using the global variables for hits and misses.
---Disclaimer--
This code does not work for every situation. The input tag that is being stored needs to be updated and therefore causes errors
