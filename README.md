# BStarTree
Left-leaning B* Tree developed during file organization discipline.

The left-leaning B* Tree is an indexation file, created upon a data file, and used to search for specific information in that file quicker. 
Time complexity for searching a LLB*T is O(log n), whereas directly in the data file, it would be O(n), and require a lot more memory pages to be loaded in RAM.  

This implementation assumes really small memory pages (120 bytes), as a requirement from the teacher. 
This implementation currently supports only search and insertion, deletion is to be implmented in the future. 
Root Node split 1:2, any other nodes split 2:3 (as a standard B* Tree).  

The page size should be modifiable by changes some defines.
