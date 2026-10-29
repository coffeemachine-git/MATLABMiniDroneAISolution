%% LAUNCH_TRACK_BUILDER
% Safely adds the scripts folder to the path, initializes workspace variables, and launches the App Designer track builder.

thisDir = fileparts(mfilename('fullpath'));
scriptsDir = fullfile(thisDir, 'scripts');
if exist(scriptsDir, 'dir')
    addpath(scriptsDir);
end

% Pre-populate drone initialization variables in base workspace
try
    evalin('base', 'InitVars;');
catch
    evalin('base', 'init = struct(''posNED'', [0 0 -0.046]);');
end

drone_track_builder;
