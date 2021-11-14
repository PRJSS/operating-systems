//
//  main.cpp
//  process cpp
//
//  Created by Бернат Данила on 01.11.2021.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <ctime>

template <typename Duration>
char print_time(tm t, Duration fraction) {
    using namespace std::chrono;
    printf("[%02u:%02u:%02u.%03u]",t.tm_hour, t.tm_min, t.tm_sec,
                static_cast<unsigned>(fraction / milliseconds(1)));
    return ' ';
    // VS2013's library has a bug which may require you to replace
    // "fraction / milliseconds(1)" with
    // "duration_cast<milliseconds>(fraction).count()"
}

int main ()
{
    using namespace std;
    using namespace std::chrono;
    
    pid_t pid; /* идентификатор процесса */
    pid = fork (); /* создание второго процесса */
    if (pid > 0) {
        pid = fork (); /* создание третьего процесса */
    }
    
    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();
    tp -= duration_cast<seconds>(tp);
    time_t tt = system_clock::to_time_t(now);
    
    printf("%c\nСвой pid=%d\nРодительский pid=%d\n\n",print_time(*localtime(&tt), tp), getpid(), getppid());
    
    if (pid > 0){
        system("ps -x");
    }

        
    
}

