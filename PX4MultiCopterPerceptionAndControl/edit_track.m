%% EDIT_TRACK Custom Track Generator for Minidrone Red Line Follower
% Use this script to easily define and visualize any competition track layout
% (straight lines, 90 deg corners, acute/obtuse turns, S-curves, etc.)
%
% Running this script automatically generates and saves 'initData.mat',
% which is loaded by the 3D simulation when you run 'run_competition'.

clear; clc;

fprintf('====================================================\n');
fprintf('       CUSTOM TRACK BUILDER & CONFIGURATOR          \n');
fprintf('====================================================\n\n');

%% 1. DEFINE TRACK WAYPOINTS [X, Y] (meters)
% Arena boundaries: X in [0.2, 3.8], Y in [0.2, 3.8]
% Drone starts at Waypoint 1 and follows each segment in order.
% The landing circle will automatically be placed at the final waypoint.

% --- PRESET 1: Standard L-Shape (Default Competition Track, 90-deg Turn) ---
waypoints = [
    0.5, 1.0;   % Start
    2.5, 1.0;   % Corner (turn 90 deg)
    2.5, 3.0;   % End of line
];

% --- UNCOMMENT BELOW FOR PRESET 2: Z-Shape / Zig-Zag (Two 90-deg Turns) ---
% waypoints = [
%     0.5, 0.8;
%     2.2, 0.8;   % Turn 1 (Right)
%     2.2, 2.4;   % Turn 2 (Left)
%     3.5, 2.4;   % End
% ];

% --- UNCOMMENT BELOW FOR PRESET 3: U-Turn / Hairpin (Sharp Turns) ---
% waypoints = [
%     0.6, 1.0;
%     2.6, 1.0;   % Turn 1
%     2.6, 2.2;   % Turn 2
%     1.0, 2.2;   % Return leg
% ];

% --- UNCOMMENT BELOW FOR PRESET 4: Sharp Acute Corner Track (135-deg Turn) ---
% waypoints = [
%     0.5, 1.0;   % Start
%     2.5, 1.0;   % Acute corner vertex
%     1.2, 2.3;   % Sharp acute turn (135-deg turn)
% ];

%% 2. TRACK APPEARANCE & PROPERTIES
trackColor = [1, 0, 0];    % RGB color: [1 0 0] = Red
circleDiameter = 0.20;     % Landing circle diameter (meters)
lineWidth = 0.10;          % Red line width (meters)

%% 3. BUILD INITDATA STRUCTURE
initData = struct();
initData.Color = trackColor;
initData.Lines = struct('StartX', {}, 'StartY', {}, 'EndX', {}, 'EndY', {});

numPts = size(waypoints, 1);
for i = 1:(numPts - 1)
    initData.Lines(end+1) = struct(...
        'StartX', waypoints(i, 1), ...
        'StartY', waypoints(i, 2), ...
        'EndX',   waypoints(i+1, 1), ...
        'EndY',   waypoints(i+1, 2));
end

% Place landing circle slightly extended along final segment direction
lastDir = waypoints(end, :) - waypoints(end-1, :);
lastDirNorm = lastDir / norm(lastDir);
circlePos = waypoints(end, :) + 0.15 * lastDirNorm;

initData.Circle = struct(...
    'X', circlePos(1), ...
    'Y', circlePos(2), ...
    'Z', 0.01, ...
    'Diameter', circleDiameter);

% Initial drone spawn position (NED frame relative to start translation [dx=0.5, dy=1.0])
% Default drone translation offset in UE4: dx = 0.5, dy = 1.0
initData.posNED = [waypoints(1, 1) - 0.5, waypoints(1, 2) - 1.0, -0.046];

%% 4. SAVE TO INITDATA.MAT
saveFile = fullfile(fileparts(mfilename('fullpath')), 'initData.mat');
save(saveFile, '-struct', 'initData');
fprintf('[SUCCESS] Generated and saved custom track to:\n  %s\n\n', saveFile);
fprintf('Total segments: %d\n', length(initData.Lines));
fprintf('Landing circle: (X = %.2f, Y = %.2f)\n', circlePos(1), circlePos(2));
fprintf('Initial drone:  (X = %.2f, Y = %.2f)\n\n', initData.posNED(1), initData.posNED(2));

%% 5. VISUALIZE TRACK
figure('Name', 'Custom Track Preview', 'Position', [200 200 600 550]);
hold on; grid on; axis equal;
xlim([0 4]); ylim([0 4]);
xlabel('X (meters)'); ylabel('Y (meters)');
title('Custom Track Preview (Arena: 4m x 4m)');

% Plot Arena Boundary
rectangle('Position', [0 0 4 4], 'EdgeColor', [0.5 0.5 0.5], 'LineStyle', '--');

% Plot Red Line Segments
for i = 1:length(initData.Lines)
    plot([initData.Lines(i).StartX, initData.Lines(i).EndX], ...
         [initData.Lines(i).StartY, initData.Lines(i).EndY], ...
         'r-', 'LineWidth', 6);
end

% Plot Joints
for i = 1:numPts
    plot(waypoints(i, 1), waypoints(i, 2), 'ro', 'MarkerFaceColor', 'r', 'MarkerSize', 8);
end

% Plot Landing Circle
th = linspace(0, 2*pi, 40);
r = circleDiameter / 2;
fill(circlePos(1) + r*cos(th), circlePos(2) + r*sin(th), [1 0.7 0.7], 'EdgeColor', 'r', 'LineWidth', 2);

% Plot Start Marker
plot(waypoints(1, 1), waypoints(1, 2), 'go', 'MarkerFaceColor', 'g', 'MarkerSize', 10);
text(waypoints(1, 1) + 0.1, waypoints(1, 2), 'START', 'Color', [0 0.5 0], 'FontWeight', 'bold');
text(circlePos(1) + 0.15, circlePos(2), 'LAND', 'Color', 'r', 'FontWeight', 'bold');

legend({'Arena Boundary', 'Track Line', '', 'Landing Disc', 'Start'}, 'Location', 'northeast');

fprintf('Run "run_competition" or open "PX4MultiCopterPerceptionAndControl.slx" to test your drone on this track!\n');
