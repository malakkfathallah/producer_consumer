This project simulates a bounded-buffer producer-consumer problem using semaphores for mutual exclusion and synchronization between processes. The producer processes continuously generate commodity prices, and the consumer process displays the latest prices along with an average price. The prices are shared through a shared memory buffer.

Objective:

Implement a bounded-buffer producer-consumer problem.
Use semaphores for mutual exclusion and synchronization.
Implement shared memory for interprocess communication.
Files in this Repository:

This repository contains the following files:

buffer.h - A header file that defines common methods and structs used by both the producer and consumer, including the shared buffer structure.
consumer.cpp - The consumer process code, which fetches prices from the shared buffer and displays them with average prices.
producer.cpp - The producer process code, which generates random commodity prices and places them in the shared buffer.
Makefile - A makefile for compiling and building the project.
How to Run
Compile the code: Use the make all command to compile the code.

Running the Consumer: The consumer process displays the prices of commodities and their averages. Run the consumer with the required arguments for the bounded-buffer size.

Running the Producer: Each producer process generates and places commodity prices in the shared buffer. Run a producer with the required arguments: commodity name, price mean, price standard deviation, sleep interval in milliseconds, and bounded-buffer size.

Important Notes:

Producers and consumers run concurrently.
The shared buffer is implemented using System V IPC (Shared Memory).
Semaphores are used for synchronization and mutual exclusion between producer and consumer.
Functionality
Producer:

Generates a new price for a specified commodity following a normal distribution with a given mean and standard deviation.
The generated price is then placed in the shared buffer.
After placing a price, the producer sleeps for the specified interval.
Consumer:

Continuously fetches the latest prices from the shared buffer.
Displays the current price for each commodity and calculates the average price of the last 5 readings.
Shows an arrow (up/down) to indicate whether the price has increased or decreased.
Example Output from Consumer
Displays the current price and the average price of the commodities, along with an indication of whether the price has gone up or down.

Dependencies:
This project uses System V IPC mechanisms for inter-process communication, which are available by default on most Unix-like operating systems.

Compilation Instructions:
To compile the project, run the provided Makefile.

Cleaning up:
To clean up the compiled files, run the make clean command.
