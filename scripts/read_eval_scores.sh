# This script searches through all eval_scores_*.txt files
# and prints out scores above a certain value.
# This saves me looking through them all the time.
# Don't know why I never done this sooner tbh.

EVAL_DIR=$PWD"/../scores/eval_scores/*"

# Take number of finishes to look for from command line
num_finishes_to_look_for=$1

if [[ $num_finishes_to_look_for -eq '' ]]; then
   num_finishes_to_look_for=180
fi

for file_name in $EVAL_DIR; do

   # Only look at files not directories
   if [[ -f $file_name ]]; then

      # Get file number
      file_num=$( echo $file_name | grep -o -E '[0-9]+' )

      while IFS=',' read -ra LINE
      do

         # for i in "${LINE[@]}"; do
         #    echo $i
         # done
         # echo "-----"

         gen_num=${LINE[0]}
         num_finishes=${LINE[1]}
         mean_traj_per_astar=${LINE[2]}
         mean_indv_fitness=${LINE[3]}

         #echo $num_finishes
         #echo "-----"

         if [[ $num_finishes -gt num_finishes_to_look_for ]]; then
            echo $file_num','$gen_num','$num_finishes','$mean_traj_per_astar
            #echo 'File num: '$file_num' Num finishes: '$num_finishes
         fi

      done  < $file_name

   fi

done
