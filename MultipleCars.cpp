#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cmath>

std::mutex entry_mtx;
std::mutex exit_mtx;
std::mutex entry_queue_mtx; //Låser hvis en anden bil er ved en entry gate.
std::mutex entry_queue_mtx; //Låser hvis en anden bil bentter exit gate
std::condition_variable cv_entry, cv_exit, cv_queue;

//entry gate signals
bool car_waiting_entry = false;
bool entry_open = false;
bool car_inside = false;
bool entry_busy = false; //Tilføjet global variabel der definere om en anden bil er ved at bruge gaten

//exit gate signals
bool car_waiting_exit = false;
bool exit_open = false;
bool car_outside = false;
bool exit_busy = false;

std::queue< 

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
        
        exit_lock.unlock();
    }
}

//car thread
void car(int id) {
    //Car enters
    std::unique_lock<std::mutex> entry_lock(entry_mtx);
    car_waiting_entry = true;
    cv_entry.notify_one(); //Tell car is waiting

    cv_entry.wait(entry_lock, [] {
            return entry_open;
        });

    std::cout << "(Car) Entry gate open.. Driving inside\n";
    car_inside = true; 
    cv_entry.notify_one(); //Tell car is inside
      
    entry_lock.unlock();

    //Car is parked for x seconds
    int seconds = rand() % 10;
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    std::cout <<"(Car) Car parked for " << seconds << "seconds\n";

    //Car exits
    std::unique_lock<std::mutex> exit_lock(exit_mtx);
    car_waiting_exit = true;
    cv_exit.notify_one(); //Tell car is waiting

    cv_exit.wait(exit_lock, [] {
        return exit_open;
        });

    std::cout << "(Car) Exit gate open.. Driving outside\n";   
    car_outside = true;
    cv_exit.notify_one(); //Tell car is outside

    exit_lock.unlock();
}

int main() {
    srand(time(nullptr));

    std::thread entry(entry_gate);
    std::thread exit(exit_gate);

    std::vector<std::thread> cars;

    for(int i = 0; i<3;i++) {
        cars.emplace_back(car,i);
    }

    for(auto& car : cars) {
        car.join();
    }

    entry.join();
    exit.join();

    return 0;
}