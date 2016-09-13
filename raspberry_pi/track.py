#!/home/ash/.virtualenvs/cv3/bin/python
#import cv
import sound2
import cv2
import math
import os
import sys
import thread
import time
import io
import getopt
import numpy as np

#True if you're using the raspberry pi camera module

#use_pi_camera = "fswebcam -r 640x480 --jpeg 100 -D 0 --no-info --no-timestamp --no-banner --no-overlay --quiet tmp.jpg"
CAMERA_WIDTH = 640
CAMERA_HEIGHT = 480
WINDOW_NAME = "Anomalous Tactical Target Acquisition and Categorizing System(ATTACS) version 2.19 2nd edition"
gui_enabled = False
capture = None
quiet = False
use_pi_camera = False
haarcascade_path = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt2.xml"
target_count = 0

def detect_faces(image):
    faces = []
    storage = cv2.CreateMemStorage()
    print image
    detected = cv2.HaarDetectObjects(image, cascade, storage, 1.2, 2, cv.CV_HAAR_DO_CANNY_PRUNING, (100,100))
    if detected:
        for (x,y,w,h),n in detected:
            faces.append((x,y,w,h))
    return faces

def track(image):
    faces = detect_faces(image)
    t = 0
    #debug gui:
    #paint rectangles and text in happy colors
    for (x,y,w,h) in faces:
        t = t+1
        dist = (0.127*CAMERA_WIDTH)/(w*0.75)
        if not quiet:
            print "#target %i, dist ~%.2fm, pitch %i, yaw %i" % (t, dist, y, x)

        if gui_enabled:
            cv2.Rectangle(image, (x,y), (x+w,y+h), 255)
            cv2.PutText(image,"Target %i, dist " % t + "~%.2fm" % dist, (x,y), font, cv.Scalar( 0, 0, 255, 0 ))
    global target_count
    if t < target_count:
	sound2.play("search")
    if t > target_count:
	sound2.play("active")
    target_count = t
    #misc info
    if gui_enabled:
        width, height = cv.GetSize(image)
        cv2.PutText(image,"Target count: " + str(t), (0,30), font, cv.Scalar( 0, 0, 255, 0 ))
        cv2.PutText(image,"Cat count: " + str(0), (0,45), font, cv.Scalar( 0, 0, 255, 0 ))
        cv2.PutText(image,"IMPORTANT! Distance is estimated, consider them calculated by David!", (0,15), font, cv.Scalar( 0, 0, 255, 0 ))
        cv2.PutText(image,"Time: %s" % (time.ctime(time.time()) ), (0,height - 2), font, cv.Scalar( 0, 0, 255, 0 ))
        cv2.ShowImage(WINDOW_NAME, image)

if __name__ == "__main__":
    #create new window if debug gui is enabled
    sound2.play("autosearch")
    print WINDOW_NAME

    try:
        opts, args = getopt.getopt(sys.argv[1:],"w:c:rgqh",["webcam=","cascade=" ,"raspicam", "gui", "quiet", "help"])
    except getopt.GetoptError:
        print "try './track.py --help'"
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print "Usage: track.py (options)"
            print "Options:"
            print "\t-h, --help\tshows help"
            print "\t-c, --cascade\toverride haarcascade (default is 'haarcascade_frontalface_alt2')"
            print "\t-w, --webcam\tuse webcam"
            print "\t-r, --raspicam\tuse raspberry pi camera"
            print "\t-q, --quiet\tdon't output tracking data, only messages & errors"
            sys.exit()
        if opt in ("-g", "--gui"):
            gui_enabled = True
        elif opt in ("-w", "--webcam"):
            print "Using webcam %s" % arg
            capture = cv.CaptureFromCAM(int(arg))
            cv2.SetCaptureProperty(capture, cv.CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT)
            cv2.SetCaptureProperty(capture, cv.CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH)
        elif opt in ("-c", "--cascade"):
            print "Using haarcascade %s" % arg
            haarcascade_path = "/usr/share/opencv/haarcascades/%s.xml" % arg
        elif opt in ("-r", "--raspicam"):
            print "Using raspicam"
            use_pi_camera = True
        elif opt in ("-q", "--quiet"):
            print "Quiet mode"
            quiet = True

    if not use_pi_camera and capture==None:
        print "You need to use either raspicam or webcam!"
        sys.exit()

    if gui_enabled:
        cv.NamedWindow(WINDOW_NAME, cv.CV_WINDOW_AUTOSIZE)
        font = cv2.InitFont(cv.CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 4)

    #variables and storage
    cascade = cv2.CascadeClassifier(haarcascade_path)
    try:
        if use_pi_camera:
            import picamera
            with picamera.PiCamera() as camera:
                camera.resolution = (CAMERA_WIDTH, CAMERA_HEIGHT)
		#camera.led = False
		camera.hflip = True
		camera.vflip = True
		print "Camera ready!"
		stream = io.BytesIO()
		for pic in camera.capture_continuous(stream, 'jpeg'):
			stream.seek(0)
			data = np.fromstring(stream.getvalue(), dtype=np.uint8)
			image = cv2.cv.fromarray(cv2.imdecode(data, 1))
			print "Capturing"
			track(image)
			if gui_enabled:
				cv2.WaitKey(1)
        else:
            while True:
                image = cv2.QueryFrame(capture)
		print image
                track(image)
                if gui_enabled:
                    cv2.WaitKey(1)

    except KeyboardInterrupt:#Intercept keyboardinterrupt(CTRL+C) and shut down...
        print "Interrupted, shutting down..."

    finally:#clean up windows
        if gui_enabled:
            cv2.DestroyWindow(WINDOW_NAME)
            #OpenCV is buggy and needs to have this for som fucked up reason
            cv2.WaitKey(1)
        print "I don't blame you"
        #os.system("mpg123 'turret.mpga' > /dev/null")
    













