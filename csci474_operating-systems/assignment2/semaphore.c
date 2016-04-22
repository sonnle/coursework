/*
 * Le_Son_CSCI474_Assignment2.c
 *
 * Program using threads and semaphores to simulate a hotel.
 * Aside from simulating hotel, keeps track of where Guests spend their time.
 * Outputs results to terminal window during execution.
 *
 * Compiled using standard Linux gcc command formatted:
 * 	gcc IndividualAssignment1_Le.c -o (output) -lpthread
 * 	NOTE: some versions of gcc require -std=c99 option when compiling
 * 		  mainly for the for(int i = 0; ... )
 *
 * Ran using ./(output)
 *
 * Terminal output generates many lines
 *  Each run will be different from the previous because of the randomness
 *  of the program, but all Event Ordering and Mutual Exclusion is enforced
 *  every time without causing hangs/deadlocks.
 *
 *  Created on: Nov 30, 2015
 *      Author: sle
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

// Bool
typedef enum {false, true} bool;

// Enum used to define what activity user will be doing
typedef enum {SwimmingPool, Restaurant, FitnessCenter, BusinessCenter} Activity;

// Semaphores
sem_t maxGuestsBuf; // Max number of guests (5)
sem_t checkInMutEx; // Check-in receptionist mutual exclusion
sem_t checkInGreetEO; // Guest goes to check-in before greet
sem_t checkInRoomEO; // Receptionist must say room number before giving to guest
sem_t checkOutMutEx; // Check-out receptionist mutual exclusion
sem_t checkOutGreetEO; // Guest goes to check-out before greet
sem_t checkOutTotalEO; // Receptionist must say a total amount before giving to guest
sem_t checkOutPaymentEO; // Guest pays before transaction can complete

sem_t roomOccupiedMutEx; // Used to ensure mutual exclusion when accessing roomOccupied boolean array
sem_t eventCountMutEx; // Used to ensure mutual exclusion when accessing eventCount int array

bool roomOccupied[5]; // Boolean array to keep track of room occupancy
int eventCount[4]; // Integer array to keep track of where guests spend their time

// Globals that allow for communication between Check-In receptionist and current Guest
int currentInGuest;
int currentInRoom;

//Globals that allow for communication between Check-Out receptionist and current Guest
int currentOutGuest;
int currentOutRoom;
int currentTotal;

void *Guest(void *args)
{
	// Get the guest number
	int *guestNum = (int *) args;
	int tempRoom;
	// Wait until there is room in the hotel
	sem_wait(&maxGuestsBuf);
		printf("Guest %d waits for check-in.\n", *guestNum);

		// Wait until the check-in receptionist is available
		sem_wait(&checkInMutEx);
			currentInGuest = *guestNum; // Communicate to check-in receptionist your guest number
			printf("Guest %d goes to the check-in receptionist.\n", *guestNum);

			// After going to check-in receptionist, allow the receptionist to greet you
			sem_post(&checkInGreetEO);

			// Wait for to post what room number you will get
			sem_wait(&checkInRoomEO);
				tempRoom = currentInRoom;
				printf("Guest %d receives room %d and completes check-in.\n", *guestNum, tempRoom);

		// No longer communicating with the check-in receptionist after you receive room
		sem_post(&checkInMutEx);

		// Randomize an activity
		Activity guestActivity = (Activity) rand() % 4; // Random activity

		// Randomize a time for that activity
		int sleepTime = ((int) rand() % 3) + 1; // Random time 1-3

		// Make sure that there is no one else accessing the array that keeps track of Guest Activity
		sem_wait(&eventCountMutEx);
		eventCount[guestActivity]++; // Increment based on randomized enum
		sem_post(&eventCountMutEx);

		// Switch on that activity and print that out to terminal
		switch(guestActivity)
		{
			case SwimmingPool:
				printf("Guest %d visits the swimming pool.\n", *guestNum);
				break;
			case Restaurant:
				printf("Guest %d visits the restaurant.\n", *guestNum);
				break;
			case FitnessCenter:
				printf("Guest %d visits the fitness center.\n", *guestNum);
				break;
			case BusinessCenter:
				printf("Guest %d visits the business center.\n", *guestNum);
				break;
		}

		// Sleep for the randomized amount of time
		sleep(sleepTime);

		printf("Guest %d goes to the check-out line.\n", *guestNum);

		// Wait for the check-out receptionist to free up
		sem_wait(&checkOutMutEx);
			// Communicate with the check-out receptionist
			currentOutGuest = *guestNum;
			currentOutRoom = tempRoom;
			printf("Guest %d goes to the check-out receptionist.\n", *guestNum);

			// Make sure Guest is at the check-out before being greeted
			sem_post(&checkOutGreetEO);

			// Make sure that Guest waits for the check-out receptionist to give total before paying
			sem_wait(&checkOutTotalEO);
			printf("Guest %d gets a balance of $%d dollars.\n", *guestNum, currentTotal);
			printf("Guest %d pays the balance of $%d dollars.\n", *guestNum, currentTotal);
			// Alert check-out receptionist that Guest is done making payment
			sem_post(&checkOutPaymentEO);
		// Done checking out so free up the check-out receptionist
		sem_post(&checkOutMutEx);
	// Leave the hotel so signal for another guest to be able to come in
	sem_post(&maxGuestsBuf);
	return NULL;
}

void *CheckInReceptionist(void *args)
{
	while(1)
	{
		// Wait for the guest to approach before greeting
		sem_wait(&checkInGreetEO);
			printf("The check-in receptionist greets guest %d.\n", currentInGuest);
			// Wait to access the array that stores room occupancy
			sem_wait(&roomOccupiedMutEx);
			// Find the first available room
			for(int i = 0; i < 5; i++)
			{
				if(!roomOccupied[i])
				{
					roomOccupied[i] = true;
					currentInRoom = i;
					break;
				}
			}
			// Allow others to access the array
			sem_post(&roomOccupiedMutEx);
			printf("The check-in receptionist assigns guest %d room %d.\n", currentInGuest, currentInRoom);
			// Make sure that receptionist assigns room before allowing Guest to proceed
			sem_post(&checkInRoomEO);
	}
	return NULL;
}

void *CheckOutReceptionist(void *args)
{
	while(1)
	{
		// Wait for Guest to approach before greeting
		sem_wait(&checkOutGreetEO);
			printf("The check-out receptionist greets guest %d and receives key for room %d.\n", currentOutGuest, currentOutRoom);
			// Wait to access the array that defines room occupancy
			sem_wait(&roomOccupiedMutEx);
			roomOccupied[currentOutRoom] = false; // Set current guest's room to false
			// Allow others to access the array
			sem_post(&roomOccupiedMutEx);
			// Randomize a balance for the Guest from 500 to 999
			printf("Calculating balance for guest %d.\n", currentOutGuest);
			currentTotal = ((int) rand() % 500) + 500;
			// Make sure balance is calculated before allowing Guest to pay
			sem_post(&checkOutTotalEO);
			// Wait for Guest payment before finalizing the transaction and completeing checkout
			sem_wait(&checkOutPaymentEO);
			printf("Receive $%d from guest %d and complete check-out.\n", currentTotal, currentOutGuest);
	}
	return NULL;
}

int main(void)
{
	// Initializing the different semaphores
	sem_init(&maxGuestsBuf, 0, 5);
	sem_init(&checkInMutEx, 0, 1);
	sem_init(&checkInGreetEO, 0, 0);
	sem_init(&checkInRoomEO, 0, 0);
	sem_init(&checkOutMutEx, 0, 1);
	sem_init(&checkOutGreetEO, 0, 0);
	sem_init(&checkOutTotalEO, 0, 0);
	sem_init(&checkOutPaymentEO, 0, 0);

	sem_init(&roomOccupiedMutEx, 0, 1);
	sem_init(&eventCountMutEx, 0, 1);

	// Initialize integer array that keeps track of guest activities
	for(int i = 0; i < 4; i++)
	{
		eventCount[i] = 0;
	}

	// Creating threads for guests, check-in, and check-out receptionist
	pthread_t checkIn, checkOut;
	pthread_t guestList[10];

	int threadNum[10];
	int buffer[2];

	// Create the threads for check-in and check-out receptionist
	pthread_create(&checkIn, NULL, CheckInReceptionist, (void *) buffer[0]);
	pthread_create(&checkOut, NULL, CheckOutReceptionist, (void *) buffer[1]);

	// Create the threads for guests
	for(int i = 0; i < 10; i++)
	{
		threadNum[i] = i;
		pthread_create(&guestList[i], NULL, Guest, &threadNum[i]);
	}

	// Wait for all of the guests to finish before continuing
	for(int i = 0; i < 10; i++)
	{
		pthread_join(guestList[i], NULL);
	}

	// Number of guests is always 10 so hardcoded.
	printf("Number of Customers\n");
	printf("Total guests: 10\n");

	// Print out the integer array that kept track of guest activities
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				printf("Swimming Pool: %d\n", eventCount[i]);
				break;
			case 1:
				printf("Restaurant: %d\n", eventCount[i]);
				break;
			case 2:
				printf("Fitness Center: %d\n", eventCount[i]);
				break;
			case 3:
				printf("Business Center: %d\n", eventCount[i]);
				break;
		}
	}
}

