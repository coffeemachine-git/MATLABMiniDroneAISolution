function setpoint = RedLineGuidance(image, currentPos)
%#codegen
% REDLINEGUIDANCE Robust closed-loop perception and guidance for MATLAB Minidrone Red Line Follower
%
% Fully autonomous guidance system designed to follow arbitrary red line tracks regardless of
% geometry: normal corners (90 deg), sharp acute corners (45-60 deg / hairpins), obtuse turns,
% smooth curves, line gaps, and distractors, with precision alignment and landing on the destination circle.
%
% Inputs:
%   image:      120x160x3 uint8 downward camera frame
%   currentPos: [X; Y; Z; yaw] drone state in NED frame
%
% Output:
%   setpoint:   [X_cmd; Y_cmd; Z_cmd; yaw_cmd] in NED frame

persistent state x_cmd y_cmd z_cmd yaw_cmd is_init
persistent last_valid_yaw loss_timer takeoff_timer align_timer landed_timer
persistent x0 y0 yaw0 circle_x circle_y
persistent corner_x corner_y corner_turn_dir turn_start_yaw
persistent circle_confirm_timer corner_cooldown incoming_yaw
persistent turn_score

% State definitions:
% 0: TAKEOFF / CLIMB
% 1: TRACK (Lookahead pursuit for straight lines and gentle curves)
% 2: APPROACH_CORNER (Straight-line deceleration into corner vertex, zero lateral drift, zero overshoot)
% 3: CORNER_TURN (In-place yaw rotation holding corner vertex, handles 45-160 deg turns)
% 4: GAP_RECOVER (Crawl along last heading if line temporarily lost)
% 5: ALIGN_CIRCLE (Precision 2D centering over destination circle)
% 6: LAND (Active descent with closed-loop centering)
% 7: LANDED (Touchdown hold)

dt = 0.20;          % Camera frame period (5 Hz)
CRUISE_ALT = -0.80; % Cruise altitude (0.8m above ground)

if isempty(is_init)
    state = 0;
    x0 = currentPos(1);
    y0 = currentPos(2);
    yaw0 = currentPos(4);
    x_cmd = x0;
    y_cmd = y0;
    z_cmd = CRUISE_ALT;
    yaw_cmd = yaw0;
    last_valid_yaw = yaw0;
    incoming_yaw = yaw0;
    circle_x = x0;
    circle_y = y0;
    corner_x = x0;
    corner_y = y0;
    corner_turn_dir = 1;
    turn_start_yaw = yaw0;
    loss_timer = 0;
    takeoff_timer = 0;
    align_timer = 0;
    landed_timer = 0;
    circle_confirm_timer = 0;
    corner_cooldown = 0;
    turn_score = 0;
    is_init = true;
end

% Decrement corner cooldown timer
if corner_cooldown > 0
    corner_cooldown = max(0, corner_cooldown - dt);
end

% Extract RGB color channels
R = double(image(:,:,1));
G = double(image(:,:,2));
B = double(image(:,:,3));

% 1. Robust Red Pixel Segmentation
% Rejects gravel floor (grey/green), yellow distractors (R~G), blue distractors, and shadows
is_red = (R > 75) & ...
         (R - G > 25) & ...
         (R - B > 25) & ...
         (R ./ max(G + B, 1) > 0.65);

nRed = sum(is_red(:));

% Altitude-adaptive metric scaling (meters per pixel)
curr_h = max(-currentPos(3), 0.15);
scale = 0.00432 * (curr_h / 0.80);

% 2. Image Feature Extraction
% Camera frame geometry:
% Row 1 = Forward (+X body), Row 120 = Rear (-X body), Row 60 = Nadir (drone center)
% Col 1 = Left (-Y body), Col 160 = Right (+Y body), Col 80 = Centerline

[rAll, cAll] = find(is_red);
if ~isempty(rAll)
    c_all = mean(cAll);
    r_all = mean(rAll);
    min_c = min(cAll);
    max_c = max(cAll);
    min_r = min(rAll);
    max_r = max(rAll);
    r_span = max_r - min_r;
    c_span = max_c - min_c;
else
    c_all = 80; r_all = 60;
    min_c = 80; max_c = 80; min_r = 60; max_r = 60;
    r_span = 0; c_span = 0;
end

% Regional zoning:
% Mid zone: rows 39 to 68 (primary corridor anchor)
maskMid = is_red(39:68, :);
[rM, cM] = find(maskMid);
nMid = length(cM);
if ~isempty(cM)
    % Centerline corridor: reject distant old legs / acute angle wedge distractors
    cM_corridor = cM(abs(cM - 80) < 38);
    rM_corridor = rM(abs(cM - 80) < 38);
    if ~isempty(cM_corridor)
        c_mid = mean(cM_corridor);
        r_mid = mean(rM_corridor) + 38;
    else
        c_mid = mean(cM);
        r_mid = mean(rM) + 38;
    end
else
    c_mid = c_all;
    r_mid = 54;
end

% Forward zone: rows 10 to 38
maskFar = is_red(10:38, :);
[rF, cF] = find(maskFar);
nFar = length(cF);
if ~isempty(cF)
    cF_corridor = cF(abs(cF - c_mid) < 38);
    rF_corridor = rF(abs(cF - c_mid) < 38);
    if ~isempty(cF_corridor)
        c_far = mean(cF_corridor);
        r_far = mean(rF_corridor) + 9;
    else
        c_far = mean(cF);
        r_far = mean(rF) + 9;
    end
    c_span_far = max(cF) - min(cF);
    min_c_far = min(cF);
    max_c_far = max(cF);
else
    c_far = 80;
    r_far = 24;
    c_span_far = 0;
    min_c_far = 80;
    max_c_far = 80;
end

% Near / Base zone: rows 60 to 95 (directly beneath drone)
maskNear = is_red(60:95, :);
[rN, cN] = find(maskNear);
nNear = length(cN);
if ~isempty(cN)
    cN_corridor = cN(abs(cN - c_mid) < 38);
    rN_corridor = rN(abs(cN - c_mid) < 38);
    if ~isempty(cN_corridor)
        c_near = mean(cN_corridor);
        r_near = mean(rN_corridor) + 59;
    else
        c_near = mean(cN);
        r_near = mean(rN) + 59;
    end
else
    c_near = c_all;
    r_near = r_all;
end

% Side branch detection: line extends towards side image boundaries
isSideBranch = (max_c >= 152) || (min_c <= 8);

% Vector from mid to forward path (lookahead vector, completely immune to rear/incoming track):
dr_fwd = max(r_mid - r_far, 1);
dc_fwd = c_far - c_mid;
psi_fwd = atan2(dc_fwd, dr_fwd);

% Vector from near base to forward path:
dr_line = max(r_near - r_far, 1);
dc_line = c_far - c_near;
psi_line = atan2(dc_line, dr_line);

% Lateral offset of track ahead of drone (using mid zone row ~54):
lat_err = (c_mid - 80) * scale; % meters

% 3. Robust Landing Circle Discrimination
% Dedicated circle inspection zone (rows 10 to 80)
maskCircle = is_red(10:80, :);
[rC, cC] = find(maskCircle);
nCircle = length(cC);
if ~isempty(cC)
    r_span_c = max(rC) - min(rC);
    c_span_c = max(cC) - min(cC);
    min_c_c = min(cC);
    max_c_c = max(cC);
    fill_factor_c = nCircle / max(r_span_c * c_span_c, 1);
else
    r_span_c = 0; c_span_c = 0;
    min_c_c = 80; max_c_c = 80;
    fill_factor_c = 0;
end

is_circle_candidate = (nCircle >= 500) && (nCircle <= 2400) && ...
                      (r_span_c >= 25) && (r_span_c <= 62) && ...
                      (c_span_c >= 35) && (c_span_c <= 58) && ...
                      (abs(r_span_c - c_span_c) <= 18) && ...
                      (fill_factor_c >= 0.52) && ...
                      (min_c_c > 18) && (max_c_c < 142) && ...
                      (corner_cooldown <= 0) && ...
                      (state == 1 || state == 2 || state == 4);

if is_circle_candidate
    circle_confirm_timer = circle_confirm_timer + dt;
else
    circle_confirm_timer = max(0, circle_confirm_timer - 2 * dt);
end

% Circle confirmed only after persistent detection across multiple frames
isCircle = (circle_confirm_timer >= 0.20);

% 4. Corner & Sharp Turn Detection (Early Lookahead Detection)
% Detects 90-degree corners, acute angles, and obtuse turns BEFORE reaching the vertex:
isCornerAhead = (corner_cooldown <= 0) && ~is_circle_candidate && ...
                (((nFar < 35) && (nMid > 45) && (isSideBranch || c_span > 75)) || ...
                 ((c_span_far > 40) && (nFar > 45) && (min_c_far < 20 || max_c_far > 140)) || ...
                 ((abs(psi_line) > deg2rad(28)) && (nFar > 40) && (isSideBranch || c_span > 50)));

% Is the drone currently AT the corner vertex?
% The forward line has ended directly at nadir (rows 10-38 empty) while the corner is directly underneath
isAtCorner = (corner_cooldown <= 0) && ~is_circle_candidate && (nFar < 20) && (nNear > 50) && (isSideBranch || c_span > 75);

% 5. Finite State Machine Execution
if state == 0 % TAKEOFF / CLIMB
    z_cmd = CRUISE_ALT;
    x_cmd = x0;
    y_cmd = y0;
    yaw_cmd = yaw0;
    last_valid_yaw = yaw0;
    incoming_yaw = yaw0;
    
    takeoff_timer = takeoff_timer + dt;
    if (currentPos(3) <= (CRUISE_ALT + 0.15)) && (takeoff_timer >= 2.0)
        state = 1; % Enter regular track following
        loss_timer = 0;
    end

elseif state == 1 % LINE TRACKING (Straight lines & gentle curves)
    z_cmd = CRUISE_ALT;
    
    if isCircle
        state = 5; % Enter ALIGN_CIRCLE
        align_timer = 0;
        circle_x = currentPos(1);
        circle_y = currentPos(2);
    elseif isCornerAhead
        state = 2; % Enter APPROACH_CORNER (smooth straight-line deceleration into vertex)
        incoming_yaw = yaw_cmd; % Record the incoming straight heading
        turn_score = 0; % Reset turn direction evidence
    elseif nRed < 35
        loss_timer = loss_timer + dt;
        if loss_timer > 0.4
            state = 4; % Enter GAP_RECOVER
        end
    else
        loss_timer = 0;
        
        v_cruise = 0.25; % Cruise speed (m/s)
        
        % Lookahead heading error (forward-looking FOV, immune to rear vertices):
        psi_err = psi_fwd;
        
        % Adaptive speed based on alignment and lateral centering
        v_fwd = v_cruise * max(0.25, cos(psi_err))^2 * max(0.40, 1.0 - abs(lat_err) / 0.15);
        
        % Yaw rate command
        yaw_rate = 1.4 * psi_err;
        yaw_cmd = yaw_cmd + yaw_rate * dt;
        last_valid_yaw = yaw_cmd;
        
        % Body frame displacement setpoint
        dx_b = v_fwd * dt;
        dy_b = 0.65 * lat_err * dt + 0.25 * (c_far - 80) * scale * dt;
        
        % Convert body displacements to NED world frame
        cy = cos(yaw_cmd);
        sy = sin(yaw_cmd);
        x_cmd = x_cmd + (dx_b * cy - dy_b * sy);
        y_cmd = y_cmd + (dx_b * sy + dy_b * cy);
    end

elseif state == 2 % APPROACH_CORNER (Straight-line deceleration into corner vertex)
    z_cmd = CRUISE_ALT;
    
    % Accumulate branch evidence during the approach:
    % Pixels with c > 96 are on the RIGHT of incoming track centerline
    % Pixels with c < 64 are on the LEFT of incoming track centerline
    % (Incoming track is centered at col 80, width ~23 px, spanning cols 68:92, so it never reaches these thresholds!)
    nRight = sum(cAll > 96);
    nLeft  = sum(cAll < 64);
    turn_score = turn_score + (double(nRight) - double(nLeft));
    
    if isCircle
        state = 5; % Enter ALIGN_CIRCLE
        align_timer = 0;
        circle_x = currentPos(1);
        circle_y = currentPos(2);
    elseif isAtCorner || (nFar < 16)
        % Arrived at the corner vertex! Lock position and begin in-place rotation
        state = 3; % Enter CORNER_TURN
        corner_x = currentPos(1);
        corner_y = currentPos(2);
        turn_start_yaw = yaw_cmd;
        
        % Final branch check at vertex (evaluates both sides across full frame)
        nBranchRight = sum(cAll > 96);
        nBranchLeft  = sum(cAll < 64);
        
        if (turn_score > 30) || (nBranchRight > (nBranchLeft + 15))
            corner_turn_dir = 1;  % Turn Right (CW)
        elseif (turn_score < -30) || (nBranchLeft > (nBranchRight + 15))
            corner_turn_dir = -1; % Turn Left (CCW)
        elseif nBranchRight >= nBranchLeft
            corner_turn_dir = 1;
        else
            corner_turn_dir = -1;
        end
    else
        % CRITICAL FIX: Maintain straight heading along incoming line while decelerating!
        % DO NOT steer sideways into the branch before reaching the vertex!
        v_crawl = 0.08; % Slow glide into the corner (zero momentum overshoot)
        
        % Hold the incoming straight-line yaw heading
        yaw_cmd = incoming_yaw;
        last_valid_yaw = incoming_yaw;
        
        % Stay centered laterally over the incoming track centerline ONLY!
        % Side branch pixels (c > 96 or c < 64) MUST NOT corrupt lateral centering!
        cN_incoming = cN(cN >= 64 & cN <= 96);
        if ~isempty(cN_incoming)
            c_near_incoming = mean(cN_incoming);
        else
            c_near_incoming = 80;
        end
        lat_err_incoming = (c_near_incoming - 80) * scale;
        
        dx_b = v_crawl * dt;
        dy_b = 0.35 * lat_err_incoming * dt;
        
        cy = cos(yaw_cmd);
        sy = sin(yaw_cmd);
        x_cmd = x_cmd + (dx_b * cy - dy_b * sy);
        y_cmd = y_cmd + (dx_b * sy + dy_b * cy);
    end

elseif state == 3 % CORNER_TURN (In-place yaw rotation holding corner vertex)
    % 1. Firmly lock position setpoint at the corner vertex: prevents overshoot & backward drift
    x_cmd = corner_x;
    y_cmd = corner_y;
    z_cmd = CRUISE_ALT;
    
    % Unwrapped yaw turned so far
    yaw_turned = abs(atan2(sin(yaw_cmd - turn_start_yaw), cos(yaw_cmd - turn_start_yaw)));
    
    % 2. Adaptive dual-speed yaw rate:
    % Coarse slew: 0.80 rad/s (~46 deg/s) while turning towards branch
    % Fine capture: 0.28 rad/s (~16 deg/s) once branch appears in forward view (prevents overshooting the line!)
    if (yaw_turned >= deg2rad(25)) && (nFar > 25)
        yaw_rate = 0.28;
    else
        yaw_rate = 0.80;
    end
    
    yaw_cmd = yaw_cmd + corner_turn_dir * yaw_rate * dt;
    last_valid_yaw = yaw_cmd;
    
    % Recalculate yaw_turned after update
    yaw_turned = abs(atan2(sin(yaw_cmd - turn_start_yaw), cos(yaw_cmd - turn_start_yaw)));
    
    % 3. Check for alignment with the new branch:
    % Minimum 30 deg rotation prevents false early lock on incoming leg
    isAlignedWithNewBranch = (yaw_turned >= deg2rad(30)) && ...
                            (nFar > 35) && (nMid > 40) && ...
                            (abs(c_far - 80) < 18) && ...
                            (abs(psi_fwd) < deg2rad(15));
    
    if isAlignedWithNewBranch
        state = 1; % Aligned with new branch! Resume forward line tracking
        x_cmd = currentPos(1); % Smooth re-anchor from actual position
        y_cmd = currentPos(2);
        yaw_cmd = yaw_cmd + 0.50 * psi_fwd; % Fine snap heading to line
        last_valid_yaw = yaw_cmd;
        corner_cooldown = 2.5; % Prevent re-triggering corner on the same vertex
        loss_timer = 0;
        turn_score = 0;
    end

elseif state == 4 % GAP RECOVERY (Crawl forward across track gap)
    loss_timer = loss_timer + dt;
    v_crawl = 0.08;
    cy = cos(last_valid_yaw);
    sy = sin(last_valid_yaw);
    x_cmd = x_cmd + v_crawl * dt * cy;
    y_cmd = y_cmd + v_crawl * dt * sy;
    z_cmd = CRUISE_ALT;
    yaw_cmd = last_valid_yaw;
    
    if isCircle
        state = 5; % Enter ALIGN_CIRCLE
        align_timer = 0;
        circle_x = currentPos(1);
        circle_y = currentPos(2);
    elseif nRed > 70
        state = 1; % Reacquired red track
        loss_timer = 0;
    elseif loss_timer > 12.0
        % Safety emergency touchdown if track lost completely for > 12 seconds (~1m crawl)
        state = 6;
        circle_x = currentPos(1);
        circle_y = currentPos(2);
    end

elseif state == 5 % ALIGN OVER CIRCLE (Precision 2D centering over landing disc)
    if nCircle > 300
        r_circ = mean(rC) + 9;
        c_circ = mean(cC);
        dx_b = (60 - r_circ) * scale;
        dy_b = (c_circ - 80) * scale;
    else
        dx_b = (60 - r_all) * scale;
        dy_b = (c_all - 80) * scale;
    end
    cy = cos(last_valid_yaw);
    sy = sin(last_valid_yaw);
    
    % Smooth proportional centering over disc
    circle_x = currentPos(1) + 0.40 * (dx_b * cy - dy_b * sy);
    circle_y = currentPos(2) + 0.40 * (dx_b * sy + dy_b * cy);
    x_cmd = circle_x;
    y_cmd = circle_y;
    z_cmd = CRUISE_ALT;
    yaw_cmd = last_valid_yaw;
    
    align_timer = align_timer + dt;
    isCentered = (abs(dx_b) < 0.05) && (abs(dy_b) < 0.05);
    if (align_timer >= 0.6 && isCentered) || (align_timer >= 1.6)
        state = 6; % Centered! Start descent
    end

elseif state == 6 % CONTROLLED LANDING DESCENT (Active centering while descending)
    if nCircle > 300
        r_circ = mean(rC) + 9;
        c_circ = mean(cC);
        dx_b = (60 - r_circ) * scale;
        dy_b = (c_circ - 80) * scale;
    else
        dx_b = (60 - r_all) * scale;
        dy_b = (c_all - 80) * scale;
    end
    cy = cos(last_valid_yaw);
    sy = sin(last_valid_yaw);
    
    if nRed > 120
        circle_x = currentPos(1) + 0.25 * (dx_b * cy - dy_b * sy);
        circle_y = currentPos(2) + 0.25 * (dx_b * sy + dy_b * cy);
    end
    x_cmd = circle_x;
    y_cmd = circle_y;
    yaw_cmd = last_valid_yaw;
    
    z_cmd = min(0.04, z_cmd + 0.22 * dt); % Smooth 0.22 m/s descent
    if currentPos(3) > -0.06
        state = 7; % Touchdown complete
    end

elseif state == 7 % LANDED
    x_cmd = circle_x;
    y_cmd = circle_y;
    z_cmd = 0.05; % Hold firmly on ground
    yaw_cmd = last_valid_yaw;
    
    % Settle timer on the ground
    if currentPos(3) > -0.06
        landed_timer = landed_timer + dt;
    end
    
    % Once firmly landed and settled for 2.0s, signal simulation completion!
    if landed_timer >= 2.0
        z_cmd = 99.0; % MISSION COMPLETE: Landed and settled on target circle!
    end
end

setpoint = [x_cmd; y_cmd; z_cmd; yaw_cmd];
end
