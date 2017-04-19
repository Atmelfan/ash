clear;
close;
clf;
theta = 0;

coords = [];
p=0

length = 5
angles = [
    p+pi-atan(10/length), sqrt(10^2 + length^2);
    p+atan(10/length), sqrt(10^2 + length^2);
    p-pi, sqrt(0 + length^2);
    p+0, sqrt(0 + length^2);
    p-pi+atan(10/length), sqrt(10^2 + length^2);
    p-atan(10/length), sqrt(10^2 + length^2)
    ];


legs(:,:,1) = makehgtform('zrotate', pi, 'translate', [ 2.5 -10 0]);
legs(:,:,2) = makehgtform('zrotate',  0, 'translate', [-2.5 -10 0]);
legs(:,:,3) = makehgtform('zrotate', pi, 'translate', [ 2.5   0 0]);
legs(:,:,4) = makehgtform('zrotate',  0, 'translate', [-2.5   0 0]);
legs(:,:,5) = makehgtform('zrotate', pi, 'translate', [ 2.5  10 0]);
legs(:,:,6) = makehgtform('zrotate',  0, 'translate', [-2.5  10 0]);


drawrobot(length, legs, angles, pi/24,0,0);
