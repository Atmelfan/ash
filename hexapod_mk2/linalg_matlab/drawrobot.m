function [ coords ] = drawrobot( length, legs, angles, yaw, pitch, roll )
%DRAWROBOT Summary of this function goes here
%   Detailed explanation goes here
    for i = 1:6

        x = angles(i, 2)*cos(angles(i, 1));
        y = angles(i, 2)*sin(angles(i, 1));
        a1 = makehgtform('yrotate', roll)*makehgtform('xrotate', pitch)*makehgtform('zrotate', yaw)*[x, y, 0, 1]';
        a = a1;
        b = legs(:,:,i)*a;
        coords(:,i) = legs(:,:,i)^(-1)*b;
    end

    plot3(coords(1,:), coords(2,:), coords(3,:), 'o');
    hold on;
    circle(0,0,sqrt(10^2 + length^2));
    circle(0,0,sqrt(0 + length^2));
    plot([-2.5 2.5 -2.5 2.5 -2.5 2.5], [10 10 0 0 -10 -10], 'x');
    plot([-2.5 2.5 2.5 -2.5 -2.5],[-10 -10 10 10 -10]);
    axis([-15 15, -15 15]);
    axis equal;
    drawnow;

end

