path_to_trajectories = '/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/trajectories_temp/';
path_to_envs = '/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/argos_params/environments/kim_envs/';
num_of_envs = 10;

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
   
    img_dilated = environments(i).img;
    
    %Generate optimal path
    optimal_path = astar_on_environment(img_dilated,[2 2],[11 11]);
    
    diff_optimal_path = diff(optimal_path);
    
    if ~isempty(diff_optimal_path)
        
        optimal_path_length = sum(sqrt(diff_optimal_path(:,1).^2+diff_optimal_path(:,2).^2))/10;     
        
        
    end
    
end



