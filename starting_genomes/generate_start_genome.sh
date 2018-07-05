#!/bin/bash

# The purpose of this bash script is to automatically generate neat
# starting genome files, as it can be monotonous to do, especially with many
# inputs and outputs

#number of inputs (not including bias)
num_inputs=1

#number of outputs
num_outputs=2

GENOME_FILE=../starting_genomes/start_genome

#Beginning of file
echo "genomestart 1" > $GENOME_FILE
echo "trait 1 0.1 0 0 0 0 0 0 0" >> $GENOME_FILE
echo "trait 2 0.2 0 0 0 0 0 0 0" >> $GENOME_FILE
echo "trait 3 0.3 0 0 0 0 0 0 0" >> $GENOME_FILE

#Bias node
echo "node 1 0 1 3" >> $GENOME_FILE

#Input nodes
for ((i=0; i<num_inputs; i++)); do
   let index=$i+2
   echo "node $index 0 1 1"
done >> $GENOME_FILE

#Output nodes
for ((i=0; i<num_outputs; i++)); do
   let index=$i+2+num_inputs
   echo "node $index 0 0 2"
done >> $GENOME_FILE

#Connections
for ((i=0; i<num_outputs; i++)); do
   let index2=$i+2+num_inputs
   for ((j=0; j<num_inputs+1; j++)); do
      let index1=$j+1
      let inv_num="$j+$i*(num_inputs+1)+1"
      echo "gene 1 $index1 $index2 0.0 0 $inv_num 0 1"
   done >> $GENOME_FILE
done >> $GENOME_FILE

#End of file
echo "genomeend 1" >> $GENOME_FILE
