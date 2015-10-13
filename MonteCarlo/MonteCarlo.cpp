/*
 * File:   main.cpp
 * Author: Ryan Williams
 *
 * Created on September 24, 2015, 4:45 PM
 *
 *
 
 approx_pi_darts.cpp
 Parallel_HW5
 Created by James Williams on 9/22/15.
 
 Purpose: simulate throwing darts within a square at random locations.
 the darts will approximate the value of pi, by calculating the
 total darts in circle/total darts thrown.
 
 The program will be using pthreads and will use the sitmo_prng_engine to produce
 all random numbers.
 
 Compiling/Running: Running in terminal requires the g++ compiler and will throw errors
 when using the gcc compiler. The number of threads and darts to be thrown are included
 in the program already and will not be taking them in as user input. I did this because
 I didn't want to take the time to optomize the number of threads per number of darts to 
 throw from a users input. Ah, just forget about it. . . .
 
 Compiling on Stampede: Compiles on stampede using the following command: 
 g++ -std=c++0x MonteCarlo.cpp -o MCarlo -pthread
 
 Note: While running on stampede, I have noticed inconsistent runtimes. 
 Work on cleaning up code for efficiency/optimizing functions.
 */

#include "timer.h" 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <random>
#include <iostream>
#include <set>
#include <random>
#include <math.h>
using namespace std;

// function headers
void *throwDarts(void *arg);
void calcHits(set < pair <double, double> > &coordinate);
double calcPi();
void useThreads(double n); // create and join threads
// function headers

// globals
long long NUM_THREADS;
long long NUM_DARTS;
long long NUM_HITS;
timer myTimer;
// globals

int main (int argc, const char * argv[]) {
    long long threadCount[] = {16, 32, 64};
    long long dartsArr [] = {6400000, 64000000, 640000000};
    
    for (int i = 0; i < 3; ++i) {
        NUM_DARTS = dartsArr[i];
        for (int j = 0; j < 3; ++j) {
            NUM_HITS = 0;
            NUM_THREADS = threadCount[j];
            useThreads(threadCount[j]);
        }
//        int count = 0;
//        ++count;
//        while (open && count < 3) {
////            NUM_DARTS = 6400000;
//            open = false;
//            
//        }
    }
}

void useThreads(double n) {
    pthread_t* thread_handles;
    thread_handles = (pthread_t *) malloc(n*sizeof(pthread_t));
    
    printf("\nStarted throwing %lld darts, while using %lld threads.\n",
           NUM_DARTS, NUM_THREADS);
    myTimer.start();
    for(long i = 0; i < n; i++) {
        pthread_create(&thread_handles[i], NULL, throwDarts, (void *) i);
    }
    
    for(int i = 0; i < n; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    
    printf("Finished throwing darts and joining threads\n");
    printf("Number of hits in the circle: %lld \n", NUM_HITS);
    printf("Estimate for Pi: %f\n", (float)calcPi());
    printf("Time elapsed: %f seconds\n", ((float)myTimer.elapsedTime()/10000000));
    free(thread_handles);
}

void * throwDarts(void * arg) {
    pthread_mutex_t lock;
    random_device rd1;
    random_device rd2;
    mt19937 mt1(rd1());
    mt19937 mt2(rd2());
    long long my_rank = (long) arg;
    long long myCount = 0;
    long long i, my_start, my_end, group;
    group = NUM_DARTS / NUM_THREADS;
    my_start = group * my_rank;
    my_end = my_start + group;
    
//    set<pair<double,double> > coordinate;
    for (i = my_start; i < my_end; ++i) { // gen coordinates for darts
        double x, y;
        x = ((double)mt1()/(RAND_MAX))-1;
        y = ((double)mt2()/(RAND_MAX))-1;
        if((x*x)+(y*y) <=1){
//            pair<double,double> p (x,y); // pair darts as cart coordinate
//            coordinate.insert(p); // insert for evaluation (slower than need be)
            ++myCount;
        }
    }
//    printf("Rank: %lld\t Hits: %lld\n", my_rank, myCount);
//    calcHits(coordinate); // evaluate complete set
   
    pthread_mutex_lock(&lock);
        NUM_HITS += myCount;
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void calcHits(set < pair <double,double> > & coordinate) {
    /*
     * * * * * * * * * NOT USING FUNCTION * * * * * * * * *
     Funciton is extremely memory intensive with very large
     sets of pairs. Also the iteration process is extremely 
     slow for evaluating the sets passed into the function.
     */
    uint localHits = 0;
    pthread_mutex_t myLock;
    for (set< pair <double,double> >::iterator it = coordinate.begin();
         it != coordinate.end(); it++) {
        if (((*it).first*(*it).first) + ((*it).second*(*it).second) <= 1) {
            localHits++;
        }
    }
    pthread_mutex_lock(&myLock); // lock access to global sum
        NUM_HITS += localHits;
    pthread_mutex_unlock(&myLock); // unlock access to global sum
}

double calcPi(){
    double myPi =  (4 * (double)NUM_HITS / (double)NUM_DARTS);
    return myPi;
}
