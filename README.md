# Concurrent Parking Lot Control System

A C++ project developed as part of the Systems (SYS) course at Aarhus University.

## Project Overview

The objective of this assignment was to implement a parking lot control system that manages cars entering and exiting a parking lot using concurrent programming.

The system uses multiple threads to simulate cars and coordinate access to the parking lot.

## Key Concepts

- **Multithreading:** Simulating multiple cars concurrently using `std::thread`.
- **Mutexes:** Protecting shared resources from race conditions.
- **Condition Variables:** Synchronizing cars and parking gates.
- **Resource Management:** Coordinating access to entry and exit gates.

## Requirements

The assignment focused on:

- Implementing thread synchronization using mutexes and condition variables.
- Modeling a parking lot with cars entering and exiting.
- Handling shared resources and limited parking capacity.
- Avoiding semaphores and atomic variables.

## Implementation

The solution simulates multiple cars arriving at a parking lot, waiting for access to the entry gate, parking for varying durations, and eventually exiting.

Separate entry and exit mechanisms allow cars to enter and leave concurrently while coordinating access to the gates.

## Technologies

- C++
- C++ Standard Library
- `std::thread`
- `std::mutex`
- `std::condition_variable`
