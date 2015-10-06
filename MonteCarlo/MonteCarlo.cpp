/*
 * File:   main.cpp
 * Author: Ryan Williams
 *
 * Created on September 24, 2015, 4:45 PM
 */
/*
 
 approx_pi_darts.cpp
 Parallel_HW5
 Created by James Williams on 9/22/15.
 
 Purpose: simulate throwing darts within a square at random locations.
 the darts will approximate the value of pi, by calculating the
 total darts in circle/total darts thrown.
 
 The program will be using pthreads and will use the sitmo_prng_engine to produce
 all random numbers.
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <random>
#include <iostream>
#include <set>
#include <math.h>

using namespace std;

void *throwDarts(void *arg);
uint calcHits(set < pair < double, double> > &coordinate);
double calcPi(uint num);

int NUM_THREADS = 5;
long NUM_DARTS;
int NUM_HITS;

int main ()
{
//    cout << "Enter the number of darts to be thrown: ";
//    cin >> NUM_DARTS;

    NUM_DARTS = 500;
    NUM_HITS = 0;
    pthread_t* thread_handles;
    thread_handles = (pthread_t *) malloc(NUM_THREADS*sizeof(pthread_t));
    
    for(long i=0; i < NUM_THREADS; i++){
        pthread_create(&thread_handles[i], NULL, throwDarts, (void *)i);
//        printf("\tCreated thread: %ld of %d\n", i, NUM_THREADS);
    }
    
    for(int i=0; i < NUM_THREADS; i++) {
        //        printf("Joining thread: %d\n", &thread_handles[i]);
        pthread_join(thread_handles[i], NULL);
    }
    float estimatePi = (4 * NUM_HITS)/(double)NUM_DARTS;

    printf("\nEstimate for Pi: %f\n", (float) estimatePi);
    free(thread_handles);
}

void *throwDarts(void *arg)
{
    
    long my_rank = (long) arg;  /* Use long in case of 64-bit system */
    printf("Hello from thread %ld of %d\n", my_rank+1, NUM_THREADS);
    
    // set up RNG's
    std::random_device rd1, rd2;
    std::mt19937 mt1(rd1());
    std::mt19937 mt2(rd2());
    
    // create accurate distribution
    std::uniform_real_distribution<double>dist(-RAND_MAX, RAND_MAX);
    
    // init pairs for storing a set with coordinates
    set< pair <double, double> > coordinate;
    printf("throws/threads: %d / %d = %d \n", (int)NUM_DARTS,
           (int)NUM_THREADS, (int)NUM_DARTS/(int)NUM_THREADS);

    // loop for throwing number of darts in trial
    for (int i = 0; i < NUM_DARTS; ++i) {
        // generate numbers between -1 and 1
        double x, y;
        x = ((double)dist(mt1)/RAND_MAX);
        y = ((double)dist(mt2)/RAND_MAX);
        pair<double,double> p (x,y);
        printf("i: %*d \tP: %*f, \t %*f \n", 3, i, 3, p.first, 3, p.second);
        coordinate.insert(p);
    }// done throwing darts
    
    uint myHits = calcHits(coordinate);
    printf("Calculated Pi: %f\n", calcPi(myHits));
    return NULL;
}

/* thread function to calc num hits for thread */
void* threadNumHits(void* rank){
    long t_rank = (long) rank;
    return NULL;
}
uint calcHits(set < pair < double, double > > & coordinate){
    uint localHits = 0;
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
    return localHits;
}

double calcPi(uint num){
    int myHits = (int) num;
    double myPi = (4*myHits)/NUM_DARTS;
    return myPi;
}
