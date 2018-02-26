README.TXT
Team Members: John Nguyen, Ryan Young

We used a prefix trie to store the data from the places2k.txt file.  
Before the data is inserted in the trie its contents are parsed.  The state is
moved to the front, spaces are removed, non-alphabetic characters are removed,
and undefining word attributes such as 'city', 'borough', etc. The prefix trie 
has an array of 26 child nodes per node. One for each letter of the alphabet. 
Each node also contains a struct to hold a searchedCity struct.  There is also 
an int to designate that the node is a leaf.  In the leaf node, the data for an 
inserted word is stored.  During search, the index is checked corresponding
to each letter of the search word.  It continues until there are no more characters.  
If the node has 2 child, the search is too vague.

For the Airport Search we used a KD tree with two dimensions latitude and logitude. An array of kdNodes were created from the given text file. Using this array we used a quick sort algorithm to find the media value and start converting the array into a tree structure. Once the tree was created, we used a nearest neighbor algorithm to search for the nearest airports given a search latitude and logitude from the middle serve. 
A linked list was used to store the 5 closest airports.

John's contributions.
I utilized third party code to learn the algorithm and adapt it to our project. 
From rosetta code: https://rosettacode.org/wiki/K-d_tree I found the code to create 
a simple KDTree, as well as the nearest neighbor. The code used a quick sort to find 
the median value from an array. While this returned the nearest neighbor I decided 
to use a priority queue to save the nodes that were visited along the traversal and order 
by distance. The code used for reference for the priority queue using a linked list was from: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
Weakness - Inititially we were trying to return an AirportList from the airport server, this caused large delays due to numerous seg faults and investigation. We eventually moved to using a Struct and union similar to the second project and this resolved many of the issues. Another weakness for this project was the in depth use of pointers and passing values between functions and programs. Without an IDE or debugger this was a lot more challenging. 
Strengths - By the end of the project I was much more familiar with the CMD debugger gdb, and using it to trace memory addresses. 

Ryan's contributions:
I worked on implementing the prefix trie and search.    I set up the connection
of servers from places client, to the middle server/client, and airport server.
I troubleshooted a vast number of segmentation faults.
Strength: Commitment.  It's hard for me to stop when there are bugs.
Weakness: Pointers and memory.  In bootcamp we learned Java.  C is new and my
knowledge if fairly basic.  After this project I am more proficient.

How to run the Airport Search Program
Extract the content of the zipped folder

1. Run "make" from the same directory
2. Start the airport_server using the command
	./airport_server airport-locations.txt &
3. Start the places server using the command
	./pa_server localhost places2k.txt
4. Run the client by using a state and city and the commnad
	./places_cliet localhost CITY State

 
