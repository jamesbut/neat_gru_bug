clear all, clc

path_to_trajectories = '/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/trajectories_temp/';
path_to_envs = '/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/argos_params/environments/kim_envs/';
num_of_envs = 209;

%Read data

for i = 1:num_of_envs
    
    % Read in trajectories
    traj_s1 = 'trajectory_';
    traj_s2 = '.txt';
    
    trajectory_path = strcat(path_to_trajectories, traj_s1, int2str(i), traj_s2);
    
    environments(i).trajectory = csvread(trajectory_path);
    
    %Read in environments images
    img_s1 = 'rand_env_';
    img_s2 = '.png';
    
    img_path = strcat(path_to_envs, img_s1, int2str(i), img_s2);
    
    environments(i).img = imread(img_path);
    
end

%Analyse data

for i = 1:length(environments)
    
    i
   
    img_dilated = environments(i).img;
    
    %Generate optimal path
    %Think the [2,2] is wrong here, should be [1.5,1.5]
    optimal_path = astar_on_environment(img_dilated,[2 2],[11 11]);
    %disp(optimal_path)
    
    diff_optimal_path = diff(optimal_path);
    %disp(diff_optimal_path)
    
    if ~isempty(diff_optimal_path)
        
        %Divide by 10 because image going into A* is 10 times the size of
        %the actual environment
        optimal_path_length = sum(sqrt(diff_optimal_path(:,1).^2+diff_optimal_path(:,2).^2))/10;
        %disp(optimal_path_length)
        
        run_path_diff = diff(environments(i).trajectory);
        %disp(environments(i).trajectory)
        %disp(length(environments(i).trajectory))
        %disp(run_path_diff)
        
        %Add 1 for the final meter to the target
        run_path_length = sum(sqrt(run_path_diff(:,1).^2+run_path_diff(:,2).^2)) + 1;
        %disp(run_path_length)
        
        run_path_lengths(i) = run_path_length;
        path_percentages(i) = run_path_length/optimal_path_length;
        
    else
        
        disp("A* produced an error")
        run_path_lengths(i) = NaN;
        path_percentages(i) = NaN;
        
    end
    
end

disp(mean(path_percentages))
disp(median(path_percentages))

figure,
boxplot(path_percentages(:))