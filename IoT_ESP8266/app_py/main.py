import cv2, time, requests
from ultralytics import YOLO

ESP8266_IP = "http://10.173.138.187"  # 你的 ESP8266 IP
FAN_ON_URL = f"{ESP8266_IP}/fan/on"
FAN_OFF_URL = f"{ESP8266_IP}/fan/off"
NO_PERSON_URL = f"{ESP8266_IP}/noperson"

NO_PERSON_TIMEOUT = 20
PERSON_CONFIDENCE = 0.5

model = YOLO("yolov8n.pt")
cap = cv2.VideoCapture(0)
last_person_time = time.time()
fan_status = False

def fan_on(): global fan_status; requests.get(FAN_ON_URL, timeout=20); fan_status = True
def fan_off(): global fan_status; requests.get(FAN_OFF_URL, timeout=20); fan_status = False

while True:
    ret, frame = cap.read()
    if not ret: break
    results = model(frame, verbose=False)
    person_detected = any(int(box.cls[0])==0 and float(box.conf[0])>PERSON_CONFIDENCE for r in results for box in r.boxes)
    current_time = time.time()

    if person_detected:
        last_person_time = current_time
        cv2.putText(frame, "Person Detected", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    else:
        cv2.putText(frame, f"No Person Detected: {int(current_time - last_person_time + 1)}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        if current_time - last_person_time >= NO_PERSON_TIMEOUT:
            fan_off()
            requests.get(NO_PERSON_URL, timeout=20)

    cv2.imshow("YOLO Detection", frame)
    if cv2.waitKey(1) & 0xFF == 27: break

cap.release()
cv2.destroyAllWindows()
