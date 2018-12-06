# This script takes in a directory name as first argument, checks
# whether it exists if not it makes the directory and transfers
# the score files from the server name given as second argument.

# If it does exist it looks in the directory and finds an appropriate
# name for the score files, uses that and transfers files from server.

directory_name=$1
server_name=$2

TRAINING_DIR=$PWD"/../scores/training_scores/archive_scores/"$directory_name
EVAL_DIR=$PWD"/../scores/eval_scores/archive_scores/"$directory_name
GRAPH_TRAINING_DIR=$PWD"/../scores/training_scores/archive_graphs/"$directory_name
GRAPH_EVAL_DIR=$PWD"/../scores/eval_scores/archive_graphs/"$directory_name

# Function to get appropriate file number and possibly create new directory
# if needed
function get_file_num {

	# If directory does not exist, create it
	if [ ! -d "$1" ]; then

		echo "Directory does not exist"

		mkdir $1
		FILE_NUM="1"

	# Otherwise check for appropriate file numbers
	else

		echo "Directory does exist"

		#Check all files in directory to see what number to assign to the new files
		for file_name in $1"/*"; do
	    	file_num=$( echo $file_name | grep -o -E '[0-9]+\'$2 | grep -o -E '[0-9]+' )
	    	file_num_arr=$(echo $file_num | tr " " "\n")
	    	max_file_num=$(echo "${file_num_arr[*]}" | sort -nr | head -n1)
		done

		FILE_NUM=$(($max_file_num+1))

	fi

}

# Get appropriate file numbers for training and eval files
get_file_num $TRAINING_DIR '.txt'
training_file_num=$FILE_NUM
get_file_num $EVAL_DIR '.txt'
eval_file_num=$FILE_NUM

# Check the file numbers are the same - they always should be
if [ $training_file_num -ne $eval_file_num ]; then
	echo "Something has gone dead wrong... check eval and training file nums"
	echo "Exiting script!!"
	exit 1
fi


# scp files from server and name them appropriately

server_dir_path="/home/james/neat_gru_bug/"

file_save_name_training=$directory_name"_training_"$training_file_num".txt"
scp $server_name":"$server_dir_path"scores/training_scores/training_scores.txt" $TRAINING_DIR"/"$file_save_name_training
echo "Saved file as: "$file_save_name_training

file_save_name_eval=$directory_name"_eval_0_"$eval_file_num".txt"
scp $server_name":"$server_dir_path"scores/eval_scores/eval_scores_0.txt" $EVAL_DIR"/"$file_save_name_eval
echo "Saved file as: "$file_save_name_eval


# Create graphs from scores

# Get graph file nums
get_file_num $GRAPH_TRAINING_DIR '.png'
graph_training_file_num=$FILE_NUM
get_file_num $GRAPH_EVAL_DIR '.png'
graph_eval_file_num=$FILE_NUM

# Check for errors
if [ $graph_training_file_num -ne $training_file_num ] || [ $graph_eval_file_num -ne $eval_file_num ]; then
	echo "Something has gone dead wrong... check graph nums and non-graphs nums"
	echo "Exiting script!!"
	exit 1
fi

# Call python code to save graphs
python3 score_analysis_training.py $directory_name $graph_training_file_num
python3 score_analysis_eval.py $directory_name $graph_eval_file_num

echo "Graphs saved"
