function h = circle(x,y,r)
    th = 0:pi/25:2*pi;
    xunit = r * cos(th) + x;
    yunit = r * sin(th) + y;
    h = plot(xunit, yunit);