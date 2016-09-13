function [ S0, S1, S2 ] = ik_calc( x, y, z, L1, L2, L3 ) %#codegen
%IK_CALC Summary of this function goes here
%   Detailed explanation goes here
    d_sq = x^2 + y^2;
	d = sqrt(d_sq) - L1;
	l_sq = d^2 + z^2;
	l = sqrt(l_sq);
	s1 = (l^2 + L2^2 - L3^2)/(2*l*L2);
	s2 = (L2^2 + L3^2 - l^2)/(2*L2*L3);

	S0 = (180/pi)*(atan2(y, x));
	S1 = (180/pi)*(acos(s1) + atan2(z, d));
	S2 = (180/pi)*(acos(s2));

end

