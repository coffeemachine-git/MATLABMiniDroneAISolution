proj = openProject(pwd);
load_system('PX4MultiCopterPerceptionAndControl');

% Set simulation time to 42 seconds to observe complete touchdown
set_param('PX4MultiCopterPerceptionAndControl', 'StopTime', '42.0');

% Log States
hUav = get_param('PX4MultiCopterPerceptionAndControl/UAV Dynamics', 'PortHandles');
set_param(hUav.Outport(1), 'DataLogging', 'on');
set_param(hUav.Outport(1), 'DataLoggingNameMode', 'Custom');
set_param(hUav.Outport(1), 'DataLoggingName', 'uavStates');

fprintf('Starting closed-loop flight simulation for 42s...\n');
simOut = sim('PX4MultiCopterPerceptionAndControl');
fprintf('Simulation completed!\n');

set_param(hUav.Outport(1), 'DataLogging', 'off');
save_system('PX4MultiCopterPerceptionAndControl');

uavStates = simOut.logsout.get('uavStates');
X = uavStates.Values.X.Data;
Y = uavStates.Values.Y.Data;
Z = uavStates.Values.Z.Data;
yaw = uavStates.Values.yaw.Data;
t = uavStates.Values.X.Time;

fprintf('================ FLIGHT SUMMARY ================\n');
fprintf('Total simulation time: %.2f s\n', t(end));
fprintf('Initial:  X=%6.3f, Y=%6.3f, Z=%6.3f, yaw=%6.1f deg\n', X(1), Y(1), Z(1), rad2deg(yaw(1)));

for ti = 5:5:40
    [~, idx] = min(abs(t - ti));
    fprintf('t=%2ds:   X=%6.3f, Y=%6.3f, Z=%6.3f, yaw=%6.1f deg\n', ...
        ti, X(idx), Y(idx), Z(idx), rad2deg(yaw(idx)));
end
fprintf('Final:    X=%6.3f, Y=%6.3f, Z=%6.3f, yaw=%6.1f deg\n', X(end), Y(end), Z(end), rad2deg(yaw(end)));

dx = diff(X); dy = diff(Y);
dist = sum(sqrt(dx.^2 + dy.^2));
fprintf('Total ground distance traveled: %.2f m\n', dist);
fprintf('================================================\n');

close(proj);
