README: Lab1_acpalum

How I designed my project:
	I created 10 threads to represent my drones each of which carried an instance of 
	a struct that I created. The struct carries the state of the drone and the x and y
	coordinates for the current, the starting and the destination position of that 
	particular drone. All of the threads are able to know the state and position of the
	other 9 drones and with that information are able to know where they are on the board
	and where they are going. Using this informaiton, I was able to code an algorithm that
	allowed the drones to safely move from their starting position to there destination
	positions without colliding with the other drones.

The main part of my code:
	The main part of my code resides in the struct called drone_context. This struct, paired
	with the use of threads, allows everything else in the program to be possible. Knowing
	the states and positions of the other drones is essential, and when every thread knows
	this information about any other thread, collision avoidance is possible.

How to run my code:
	make all

	/*program will ask you to input x and y coordinate of starting position and x and y coordinate of destination*/
	./level0

	/*shows ten drones all starting from different locations, moving to different destinations, and then returning to their respective starting locations*/
	./level1a destinations.txt

	/*shows ten drones all starting from different locations, moving to different destinations, avoiding statis obsticles and then returning to their respective starting locaiton while avoiding static obstacles*/
	./level1b destinations.txt <x_coord obsticle 1> <y_coord obsticle 1> <x_coord obsticle 2> <y_coord obsticle 2>

	/*not much different from level1b. adding obsticles is optional*/
	./level2 destinations.txt <x_coord obstible 1> <y_coord obsticle 1> <x_coord obsticle 2> <y_coord obstible 2>

Text Files explained:
	all text files have 10 lines for each drone. each line follows this format:
	<x_coord of starting position> <y_coord of starting position> <x_coord of destination> <y_coord of destination>
	
	destinations.txt: has ten lines for ten drones with different starting and destination coordinates
	eastwest.txt: has two drones that travel in a horizontal straight line towards eachother...the other 8 drones start and end at coordinate (0, 0). Use this text file to show horizontal avoidance.
	northsouth.txt: has two drones that travel in a vertical straight line towards eachother...the other 8 drones start and end at coordinate (0, 0). Use this text file to show vertical avoidance.

state_machine.pdf:
	Each drone is simultaneously running this state machine. Every time the system clock ticks 
	each drone locks the system state matrix, reads the state, checks for dispatch when at the GATE, 
	looks for impending collisions and moves toward destination when it is not at the GATE.
	The drones are moving or waiting to move in all states other than GATE and AVOID.

Feedback about project:
	I really enjoyed this project! I thougt it was challenging, but I learned a lot! I, like most
	of the class, wish the grading criteria was a little bit more explicit. I would be interested
	to learn about how this project could be converted into something physical with the use of
	hardware like a Crazy Fly. Overall, the process of completing this lab was really fun and
	satisfying once completed!
	
