proj = openProject(pwd);
load_system('PerceptionAndGuidance');

mfPath = 'PerceptionAndGuidance/Image processing and Guidance/RedLineGuidance';
funcCode = fileread(fullfile(pwd, 'support', 'RedLineGuidance.m'));
rt = sfroot;
chart = rt.find('-isa', 'Stateflow.EMChart', 'Path', mfPath);
chart.Script = funcCode;
save_system('PerceptionAndGuidance');

fprintf('Compiling PerceptionAndGuidance...\n');
set_param('PerceptionAndGuidance', 'SimulationCommand', 'update');
fprintf('PerceptionAndGuidance compiled successfully!\n');

fprintf('Compiling PX4MultiCopterPerceptionAndControl...\n');
load_system('PX4MultiCopterPerceptionAndControl');
set_param('PX4MultiCopterPerceptionAndControl', 'SimulationCommand', 'update');
fprintf('PX4MultiCopterPerceptionAndControl compiled successfully!\n');

save_system('PerceptionAndGuidance');
save_system('PX4MultiCopterPerceptionAndControl');

close(proj);
