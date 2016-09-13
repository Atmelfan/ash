te = -10*log(1/5);
t = linspace(0,te);
v = exp(-t/10);
plot(t, v)
hold on
t1 = linspace(1, floor(te), 5);
v1 = exp(-t1/10);
plot(t1,v1, 'x')



