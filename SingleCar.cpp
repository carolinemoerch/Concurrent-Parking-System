#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex entry_mtx;
std::mutex exit_mtx;
std::condition_variable cv_entry, cv_exit;

//entry gate signals
bool car_waiting_entry = false;
bool entry_open = false;
bool car_inside = false;

//exit gate signals
bool car_waiting_exit = false;
bool exit_open = false;
bool car_outside = false;


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
void car() {
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
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout <<"(Car) Car parked for 3 seconds\n";

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
    
    std::thread entry(entry_gate);
    std::thread exit(exit_gate);
    while(true){
    std::thread car1(car);
    car1.join();
    }
    entry.join();
    exit.join();
    
    
    return 0;
}