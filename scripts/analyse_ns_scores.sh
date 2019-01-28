# This script takes all of the novelty archive eval scores
# and searches through them for potential winners

EVAL_DIR=$PWD"/../scores/eval_scores/*"

for file_name in $EVAL_DIR; do

   # Only look at files not directories
   if [[ -f $file_name ]]; then

      # Get file number
      file_num=$( echo $file_name | grep -o -E '[0-9]+' )

      # Read line from file
      IFS=',' read -ra LINE < $file_name

      # for i in "${LINE[@]}"; do
      #    echo $i
      # done
      #echo "-----"
      
      gen_num=${LINE[0]}
      num_finishes=${LINE[1]}
      mean_traj_per_astar=${LINE[2]}
      mean_indv_fitness=${LINE[3]}

      #echo $num_finishes
      #echo "-----"

      if [[ $num_finishes -gt 0 ]]; then
         echo $file_num
      fi

   fi

done
