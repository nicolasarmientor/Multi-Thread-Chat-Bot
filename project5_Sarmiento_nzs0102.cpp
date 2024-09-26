// Project 5
// Nicolas Sarmiento
// 904275018
// nzs0102
// project5_Sarmiento_nzs0102.cpp

// For this project I had to do some research and had some external help from stackoverflow to achieve what I wanted to execute.
// For instance, I needed help comprehending how to properly use the mutex library features as well as the chronos library to ensure unique thread access-
// and a simulated-timed response, this is why for both threads I made use of different times so I could properly ensure an alternating execution between the threads. 

// The main idea was to replicate the interaction between a user (consumer) and an automated chatbot (producer), and simulate a conversation. The list of messages used 
// includes just some of the most common questions to be asked and the chatbot will use this shared buffer to retrieve the information given by the user, in this case the 
// consumer thread.

// How to compile: g++ -o project5_Sarmiento_nzs0102 project5_Sarmiento_nzs0102.cpp -std=c++11 -pthread
// How to execute: ./project5_Sarmiento_nzs0102

// ------- Library imports ------- //

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
#include <chrono>
#include <queue>

using namespace std;

// ------- Definition of only data buffer ------- //

queue<string> message_queue;

// ------- Definition and initialization of size of previous data buffer ------- //

const unsigned int buffer_size = 3;

// ------- Mutex variable creation to ensure mutual exclusion of threads when accessing the buffer ------- //

mutex mutex_variable;

// ------- Signal variable definition for both threads ------- //

condition_variable signal_producer;
condition_variable signal_consumer;

// ------- Thread #1: Producer ------- //

void producer() {

    // ------- List of questions to be asked ------- //

    string message_set[] = {
        "Good morning!",
        "How far is the nearest Walmart?",
        "What is today's date?",
        "What does the weather look like for tomorrow?",
        "What is your best pick-up line?",
        "Bye!"
    };

    // ------- Loop for inserting question by question to the buffer with simulation waiting times and signal updating ------- //

    for (const string& message : message_set) {
        this_thread::sleep_for(chrono::milliseconds(200)); // User typing simulation (200 ms)
        unique_lock<mutex> lock(mutex_variable);
        signal_producer.wait(lock, [] { return message_queue.size() < buffer_size; }); // Used stackoverflow for this part of the code to properly wait if the buffer is full
        message_queue.push(message);
        cout << "User: " << message << endl;
        signal_consumer.notify_one();
    } 
}

// ------- Thread #2: Consumer ------- //

void consumer() {
    while(true) {
        unique_lock<mutex> lock(mutex_variable);
        signal_consumer.wait(lock, [] { return !message_queue.empty(); }); // Used stackoverflow for this part of the code to properly wait if the buffer is empty

        string message = message_queue.front();
        message_queue.pop();
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(100)); // User typing simulation (100 ms)

        string answer = "Chatbot: Message processed: '" + message + "'";
        cout << answer << endl;

        signal_producer.notify_one();

        // ------- Built-in loop break ------- //

        if (message == "Bye!") {
            break;
        }
    }
}

// ------ Main function ------- //

int main() {
    cout << " " << endl;

    // ------- Thread creations ------- //

    thread producer_thread(producer);
    thread consumer_thread(consumer);

    // ------ Wait for thread completion ------ //

    producer_thread.join();
    consumer_thread.join();

    cout << " " << endl;

    return 0;
}
