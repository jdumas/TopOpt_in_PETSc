#!/bin/sh
#PBS -N JobName
#PBS -l nodes=8:ppn=12
#PBS -k oe
#PBS -q topopt

source /appl/htopopt/RH61G/set_env.sh
DATADIR=Result_Problem2_01

echo $PBS_O_WORKDIR
cd $PBS_O_WORKDIR
mkdir $DATADIR
cp ../topopt/*.py $DATADIR
mpirun -np 96 -mca btl openib,self ../topopt/topopt \
-nx 49 -ny 25 -nz 25 -workdir $PBS_O_WORKDIR/$DATADIR \
-filter 1 -pb 2
