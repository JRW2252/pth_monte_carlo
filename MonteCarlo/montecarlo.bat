#!/bin/bash
#SBATCH -J myMonteCarlo  # Job name   
#SBATCH -o myjob.%j.out  # Name of stdout output file(%j expands to jobId)
#SBATCH -e MCarlo.e      # Name of stderr output file(%j expands to jobId)
#SBATCH -p normal        # normal queue for jobs > 256 nodes
#SBATCH -N 8             # Total number of nodes requested (16 cores/node)
#SBATCH -n 128
#SBATCH -t 00:20:00      # Run time (hh:mm:ss) - 0.33 hours

g++ -std=c++0x -Wall MonteCarlo.cpp -o MCarlo -pthread

# ibrun ./MCarlo
./MCarlo
