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
 
 */

#include "timer.h" 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <random>
#include <iostream>
#include <set>
#include <math.h>
using namespace std;

// function headers
void *throwDarts(void *arg);
void calcHits(set < pair <double, double> > &coordinate);
double calcPi();
// function headers

// globals
double NUM_THREADS = 100;
double NUM_DARTS;
double NUM_HITS;
// globals

int main (int argc, const char * argv[])
{
//    cout << "Enter the number of darts to be thrown: ";
//    cin >> NUM_DARTS;

    NUM_DARTS = 8000000;
    NUM_HITS = 0;
    pthread_t* thread_handles;
    thread_handles = (pthread_t *) malloc(NUM_THREADS*sizeof(pthread_t));
    
    // timing the time to make throw and join threads
    timer myTimer;
    myTimer.start();
    printf("\nStarted to throw the %.f darts with %.f threads.\n", NUM_DARTS, NUM_THREADS);
    for(long i = 0; i < NUM_THREADS; i++){
        pthread_create(&thread_handles[i], NULL, throwDarts, (void *) i);
//        printf("\tCreated thread: %ld of %d\n", i, NUM_THREADS);
    }
    
    for(int i = 0; i < NUM_THREADS; i++) {
//        printf("Joining thread: %d\n", &thread_handles[i]);
        pthread_join(thread_handles[i], NULL);
    }
    printf("Done throwing darts and joining threads\n\n");
    printf("Elapsed time: %f seconds\n", (float)myTimer.elapsedTime()/10000000);
    printf("Number of hits in the circle: %.f \n", NUM_HITS);
    printf("Estimate for Pi: %f\n\n", calcPi());
    free(thread_handles);
}

void * throwDarts(void * arg)
{
    long my_rank = (long) arg;
    long i, my_start, my_end, group;
    group = NUM_DARTS / NUM_THREADS;
    my_start = group * my_rank;
    my_end = my_start + group;
    
    // set up RNG's
    std::random_device rd1, rd2;
    std::mt19937 mt1(rd1());
    std::mt19937 mt2(rd2());
    
    // create accurate distribution
    std::uniform_real_distribution<double>dist(-RAND_MAX, RAND_MAX);
    
    // init pairs for storing a set with coordinates
    set< pair <double, double> > coordinate;
    
//    printf("thread %ld\t start: %ld\t end: %ld\n", my_rank, my_start, my_end);
    
    for (i = my_start; i < my_end; ++i) { // gen coordinates for throw
        double x, y;
        x = ((double)dist(mt1)/RAND_MAX);
        y = ((double)dist(mt2)/RAND_MAX);
        pair<double,double> p (x,y);
        coordinate.insert(p);
    }
    
    // done with filling each set of coordinates
    calcHits(coordinate);
    return NULL;
}

void calcHits(set < pair < double, double > > & coordinate){
    uint localHits = 0;
    pthread_mutex_t myLock;
    
    for (set<pair<double,double> >::iterator it = coordinate.begin() ;
         it != coordinate.end(); it++)
    {
        //squares the values
        double x = pow (get<0>(*it), 2);
        double y = pow (get<1>(*it), 2);
        
        //sums x^2 and y^ to check if in circle
        if ((x+y) <= 1)
            localHits++;
    }
    pthread_mutex_lock(&myLock);
        NUM_HITS += localHits;
    pthread_mutex_unlock(&myLock);
}

double calcPi(){
    double myPi =  (4 * NUM_HITS / NUM_DARTS);
    return (float) myPi;
}
