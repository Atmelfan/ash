[y,Fs] = audioread('t.wav');

%spectrogram(y(:,1))
plot(y(:,1))
sound(y(:,1), Fs)