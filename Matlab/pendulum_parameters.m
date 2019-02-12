%% pendulum model parameters
m_p = 0.270;        % full pendulum mass
l_cm  = 0.16267;    % center of mass distance
J = 162231e-9;      % Wheel momentum of inertia
g = 9.81;           % gravity acceleration
J_p = m_p*l_cm^2;   % full pendulum momentum of inertia

%% motor parameters
c_m = 0.024;        % motor torque constant
c_e = 6.3/325;      % motor back EMF constant
R_a = 10.21;        % motor armature resistance

% controller sampling time
Ts =50e-3;          % controller sampling time

%% linear system equations
A = [0          1       0;...
    g/l_cm      0       c_e*c_m/(J_p*R_a);...
    -g/l_cm     0       -c_m*c_e/(J*R_a)];

B = [0;...
    -c_m/(J_p*R_a);...
    c_m/(J*R_a)];

C = [1 0 0];

D = [0];

% discretization
G = ss(A,B,C,D);
Gd = c2d(G,Ts);

%% LQR controller
Qd = diag([1,1,1]);
Rd = 10;

[Kr, P] = dlqr(Gd.a,Gd.b,Qd,Rd);
Kr =-Kr;

% copy this to the code in the file pendulum.c
disp(strcat('float Kr[3] = {',num2str(Kr(1)),',',num2str(Kr(2)),',',num2str(Kr(3)),'};'))