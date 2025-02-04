import cv2
import numpy as np
import urllib.request
import requests

from pidLib import*

print(cv2.__version__)

url = 'http://192.168.137.119/cam-hi.jpg'
ESP32_IP = "http://192.168.137.119" 

# Roboflow API Details (Replace with your own)
#ROBOFLOW_API_KEY = "your_api_key"
#ROBOFLOW_MODEL_ID = "your_model_id"
#ROBOFLOW_VERSION = "your_version_number"

def move_servo(servo, value):
    """
    Sends a command to the ESP32 to move a servo.
    
    :param servo: Servo number (1 or 2)
    :param value: Change in angle (positive or negative)
    """
    url = f"{ESP32_IP}/servo{servo}/{value}"
    try:
        response = requests.get(url, timeout=5)
        if response.status_code == 200:
            print(f"Servo {servo} moved by {value} degrees: {response.text}")
        else:
            print(f"Error: Received status code {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request failed: {e}")

#sieć dla malego zbioru danych
#ROBOFLOW_API_URL = "https://detect.roboflow.com/my-first-project-n4eso/1?api_key=iq70OOUzLagzL8U72zf8"
#sieć dla duzego zbioru dancyh
ROBOFLOW_API_URL = "https://detect.roboflow.com/morephotos/2?api_key=iq70OOUzLagzL8U72zf8"

pidX=PID(0.5,0,0)    

prevErrorX=0 
errorSumX=0 
maxPIDValueX=20

pidY=PID(0.5,0,0)    

prevErrorY=0 
errorSumY=0 
maxPIDValueY=20
while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    im = cv2.imdecode(imgnp, -1)

    _, img_encoded = cv2.imencode('.jpg', im)
    files = {"file": img_encoded.tobytes()}

    response = requests.post(ROBOFLOW_API_URL, files=files)
    detections = response.json()

    img_width=im.shape[0]
    img_height=im.shape[1]
    
    bestFaceConfidence=0
    bestFaceCenterX=img_width/2
    bestFaceCenterY=img_height/2
    
    for detection in detections.get("predictions", []):
        x, y, w, h = int(detection["x"]), int(detection["y"]), int(detection["width"]), int(detection["height"])
        class_name = detection["class"]
        confidence = detection["confidence"]

        cv2.rectangle(im, (x - w//2, y - h//2), (x + w//2, y + h//2), (0, 255, 0), 2)
        cv2.putText(im, f"{class_name}: {confidence:.2f}", (x - w//2, y - h//2 - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
        if(confidence>=bestFaceConfidence):
                bestFaceConfidence=confidence
                bestFaceCenterX=x+w/2
                bestFaceCenterY=y+h/2

    print(bestFaceCenterX)
    print(bestFaceCenterY)
    print(img_width/2)
    print(img_height/2)
    if(bestFaceCenterX<img_width/2):
        move_servo(2, 3)
    elif(bestFaceCenterX>img_width/2):
        move_servo(2, -3)
    else:
        move_servo(2, 0)
    if(bestFaceCenterY>img_height/2):
        move_servo(1, 3)
    elif(bestFaceCenterY<img_height/2):
        move_servo(1, -3)
    else:
        move_servo(1, 0)

    cv2.imshow('Processed Image', im)
    cv2.waitKey(1)
