import wave
import pyaudio
import alsaaudio
import sys
import time


class SoundCallback(object):
    lasttime = 0
    time = 0

    def __init__(self, wf, pyaudio):
        self.wave = wf
        self.audio = pyaudio
        self.stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                             channels=wf.getnchannels(),
                             rate=wf.getframerate(),
                             output=True,
                             stream_callback=self)

    def start(self):
        self.stream.start_stream()
        self.time = time.time() - self.lasttime

    def stop(self):
        self.stream.stop_stream()
        self.lasttime = self.get_time()

    def is_active(self):
        return self.stream.is_active()

    def get_time(self):
        return time.time() - self.time

    def __call__(self, in_data, frame_count, time_info, status):
        data = self.wave.readframes(frame_count)
        return data, pyaudio.paContinue

    def close(self):
        p.close(self.stream)


if __name__ == '__main__':
    sounds = {}
    plays = []

    if len(sys.argv) < 2:
        print("Plays a wave file.\n\nUsage: %s filename.wav" % sys.argv[0])
        sys.exit(-1)
    # instantiate PyAudio (1)
    p = pyaudio.PyAudio()
    initialtime = time.time()
    with open(sys.argv[1], 'r') as com:
        for line in com:
            if not line or line.startswith("#"):
                continue
            args = line.split()
            if not args:
                continue
            cmd = args[0].lower()
            print(args)
            if cmd == "load" and len(args) >= 2:
                sounds[args[1]] = wave.open(args[1], 'rb')
                print("Loaded '%s'!" % args[1])
            elif cmd == "play" and len(args) >= 2:
                if args[1] in sounds:
                    playing = SoundCallback(sounds[args[1]], p)
                    sounds[args[1]].rewind()
                    playing.start()
                    print("Playing '%s'!" % args[1])
            elif cmd == "sync" and len(args) >= 2:
                t = float(args[1])
                if t == 0.0:
                    initialtime = time.time()
                else:
                    tbegin = time.time()
                    while time.time() - initialtime < t:
                        time.sleep(0.1)
                    print("Waited %f seconds!" % (time.time() - tbegin))




    while not any(x.is_active() for x in plays):
        time.sleep(0.1)

    for val in plays:
        val.stop()
        val.close()

    for val in sounds:
        val.close()

    # close PyAudio (7)
    p.terminate()



