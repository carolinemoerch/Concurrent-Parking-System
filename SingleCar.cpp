#include <iostream>
#include <threads>
#include <mutex>
#include <conditional_variable>
#include <chrone>

std:: mutex mtx;
std:: conditional_variable cv_entry, cv_exit;

//entry gate signals
bool car_waiting_entry = false;
bool entry_open = false;
bool car_inside = false;

//entry gate tread
void entry_gate() {
    while (true) {
        
    }
}


