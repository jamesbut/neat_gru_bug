function optimal_path = astar_on_environment(environment,goal_coordinates,starting_position)

environment_large = environment;
%environment_large(find(environment_large==0))

environment_large(find(environment_large==255))=1;
goal_coordinate_env = int8(zeros(size(environment_large)));
goal_coordinate_env(floor(goal_coordinates(2)*10),floor(goal_coordinates(1)*10)) =1;
startX = starting_position(1)*10;
startY = starting_position(2)*10;
optimal_path=ASTARPATH(startX,startY,environment_large,goal_coordinate_env,1);

% figure(1),imshow(environment),hold on,
% plot(startX,startY,'o')
% plot(goal_coordinates(1)*10,goal_coordinates(2)*10,'x'),
% plot(optimal_path(:,2),optimal_path(:,1),'g'), hold off
%  keyboard
end