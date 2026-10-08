#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <queue>

std::mutex entry_mtx;
std::mutex exit_mtx;
std::mutex entry_queue_mtx;
std::mutex exit_queue_mtx; 
std::mutex capacity_mtx; //Capacity mutex
std::condition_variable cv_entry, cv_exit, cv_queue_entry, cv_queue_exit, cv_capacity; //Capacity added

int capacity = 3; //Tilføjet global variabel for capacity. Vi har plads til 3 biler.

//entry gate signals
bool car_waiting_entry = false;
bool entry_open = false;
bool car_inside = false;
bool entry_busy = false; //Tilføjet global variabel der definere om en anden bil er ved at bruge gaten

//exit gate signals
bool car_waiting_exit = false;
bool exit_open = false;
bool car_outside = false;
bool exit_busy = false; //Tilføjet global variabel der definere om en anden bil er ved at bruge gaten

std::queue<int> entry_queue;
std::queue<int> exit_queue;

//entry gate thread
void entry_gate() {
    while (true) {
        std::unique_lock<std::mutex> entry_lock(entry_mtx);
        cv_entry.wait(entry_lock, [] {
            return car_waiting_entry;
        });

        std::cout << "(Entry gate) Car waiting.. opening gate\n";
        entry_open = true;
        cv_entry.notify_one(); //Tell car can enter

        cv_entry.wait(entry_lock, [] {
            return car_inside;
        });

        std::cout << "(Entry gate) Car inside.. closing gate\n";

        car_waiting_entry = false;
        car_inside = false;
        entry_open = false;

        cv_entry.notify_one();

        entry_lock.unlock();
    }
         
}

//exit gate thread
void exit_gate() {
    while (true) {
        std::unique_lock<std::mutex> exit_lock(exit_mtx) ;
        cv_exit.wait(exit_lock, [] {
            return car_waiting_exit;
        });

        std::cout << "(Exit gate) Car waiting.. opening gate\n";
        exit_open = true;
        cv_exit.notify_one(); //Tell car can enter

        cv_exit.wait(exit_lock, [] {
            return car_outside;
        });

        std::cout << "(Exit gate) Car outside.. closing gate\n";

        car_waiting_exit = false;
        car_outside = false;
        exit_open = false;

        cv_exit.notify_one();
        
        exit_lock.unlock();
    }
}

//car thread
void car(int id) {
    // Car joins entry queue
    std::unique_lock<std::mutex> entry_gate_lock(entry_queue_mtx);
    entry_queue.push(id);

    // Wait until it's our turn
    cv_queue_entry.wait(entry_gate_lock, [id] {
        return entry_queue.front() == id && !entry_busy;
    });

    entry_busy = true; // Reserve the gates
    entry_queue.pop();

    //Car enters
    std::unique_lock<std::mutex> entry_lock(entry_mtx);
    std::unique_lock<std::mutex> capacity_lock(capacity_mtx);

    cv_capacity.wait(capacity_lock, [] { //Wait for capacity to be more than 0
        return capacity > 0;
    });
    std::cout << "(Car " << id << ") Capacity: " << capacity << ". There is room for the car\n";

    capacity--; //Dekrementere capacity
    capacity_lock.unlock();

    car_waiting_entry = true;
    cv_entry.notify_one(); //Tell car is waiting

    cv_entry.wait(entry_lock, [] {
            return entry_open;
        });

    std::cout << "(Car " << id << ") Entry gate open.. Driving inside\n";
    car_inside = true; 
    cv_entry.notify_one(); //Tell car is inside

    cv_entry.wait(entry_lock, [] { //Venter på porten lukker
        return !entry_open;
    });

    entry_lock.unlock();

    // Nu må næste bil kører igennem porten
    entry_busy = false;
    cv_queue_entry.notify_one();
    entry_gate_lock.unlock();
      

    //Car is parked for x seconds
    int seconds = id + 2;
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    std::cout << "(Car " << id << ") Car parked for " << seconds << " seconds\n";

    capacity_lock.lock();
    capacity++;
    cv_capacity.notify_one();

    // Car joins exit queue
    std::unique_lock<std::mutex> exit_gate_lock(exit_queue_mtx);
    exit_queue.push(id);

    // Wait until it's our turn to exit
    cv_queue_exit.wait(exit_gate_lock, [id] {
        return exit_queue.front() == id && !exit_busy;
    });

    exit_busy = true; // Reserve the gates
    exit_queue.pop();

    //Car exits
    std::unique_lock<std::mutex> exit_lock(exit_mtx);
    car_waiting_exit = true;
    cv_exit.notify_one(); //Tell car is waiting

    cv_exit.wait(exit_lock, [] {
        return exit_open;
        });

    std::cout << "(Car " << id << ") Exit gate open.. Driving outside\n";   
    car_outside = true;
    
    cv_exit.notify_one(); //Tell car is outside

    cv_exit.wait(exit_lock, [] { //Venter på porten lukker
        return !exit_open;
    });
    
    capacity_lock.unlock();
    exit_lock.unlock();
    

    exit_busy = false; //Release entry gate busy. New car can drive outside.
    cv_queue_exit.notify_one();
    exit_gate_lock.unlock();

}

int main() {
    std::thread entry(entry_gate);
    std::thread exit(exit_gate);

    entry.detach();
    exit.detach();

    std::vector<std::thread> cars;

    for (int i = 0; i < 5; i++) {
        cars.emplace_back(car, i);
    }

    for (auto& car_thread : cars) {
        car_thread.join();
    }

    return 0;
}