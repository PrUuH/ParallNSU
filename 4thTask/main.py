import cv2
import logging
import threading
import queue
import argparse
import time
import os

# Настройка логирования
log_dir = "log"
if not os.path.exists(log_dir):
    os.makedirs(log_dir)

logging.basicConfig(
    filename=f"{log_dir}/app.log",
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

# Базовый класс Sensor
class Sensor:
    def __init__(self):
        self.running = True

    def get(self):
        raise NotImplementedError("Method should be in child class")

    def stop(self):
        self.running = False

# Класс для камеры
class SensorCam(Sensor):
    def __init__(self, camera_index=0, resolution=(1280, 720)):
        super().__init__()
        self.camera_index = camera_index
        self.resolution = resolution
        self.data = None
        try:
            self.cap = cv2.VideoCapture(self.camera_index)
            self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.resolution[0])
            self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.resolution[1])
            if not self.cap.isOpened():
                raise ValueError(f"Cannot open camera {self.camera_index}")
            logging.info(f"Camera with index {self.camera_index} initialized successfully")
        except Exception as e:
            logging.error(f"Initialization error: {e}")
            raise

    def get(self):
        while self.running:
            ret, frame = self.cap.read()
            if ret:
                self.data = frame
            else:
                logging.error(f"Cannot read frame from camera {self.camera_index}")
                break

    def __del__(self):
        if hasattr(self, 'cap') and self.cap.isOpened():
            self.cap.release()
            logging.info("Camera resources have been released")

# Класс для эмуляции датчиков
class SensorX(Sensor):
    def __init__(self, delay: float):
        super().__init__()
        self._delay = delay
        self._data = 0

    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data

# Класс для отображения изображения
class WindowImage:
    def __init__(self, display_frequency):
        self.display_frequency = display_frequency
        self.running = True
        cv2.namedWindow("Output", cv2.WINDOW_NORMAL)

    def show(self, img, sensor_data):
        try:
            for i, data in enumerate(sensor_data):
                cv2.putText(img, data, (10, 30 + i * 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
            cv2.imshow("Output", img)
        except Exception as e:
            logging.error(f"Error showing image: {e}")

    def __del__(self):
        cv2.destroyAllWindows()
        logging.info("Window closed")

# Функция для работы потоков датчиков
def process_sensor(sensor, queue):
    while sensor.running:
        data = sensor.get()
        print(f"Adding data to queue for {sensor}: {data}")
        if not queue.full():
            queue.put(data)

# Основная функция программы
def main():
    parser = argparse.ArgumentParser(description="Program for processing data from sensors and a camera")
    parser.add_argument("--camera_index", type=int, default=0, help="Camera index (default == 0)")
    parser.add_argument("--resolution", type=str, default="1280x720", help="Camera resolution")
    parser.add_argument("--display_frequency", type=int, default=30, help="Display frequency (Hz)")
    args = parser.parse_args()

    resolution = tuple(map(int, args.resolution.split("x")))

    cam_queue = queue.Queue()
    sensor1_queue = queue.Queue(maxsize=10)
    sensor2_queue = queue.Queue(maxsize=10)
    sensor3_queue = queue.Queue(maxsize=10)

    cam = SensorCam(camera_index=args.camera_index, resolution=resolution)
    sensor1 = SensorX(delay=0.01)  # 100 Hz
    sensor2 = SensorX(delay=0.1)   # 10 Hz
    sensor3 = SensorX(delay=1.0)   # 1 Hz

    threads = []
    threads.append(threading.Thread(target=cam.get))
    threads.append(threading.Thread(target=process_sensor, args=(sensor1, sensor1_queue)))
    threads.append(threading.Thread(target=process_sensor, args=(sensor2, sensor2_queue)))
    threads.append(threading.Thread(target=process_sensor, args=(sensor3, sensor3_queue)))

    for thread in threads:
        thread.start()

    window = WindowImage(display_frequency=args.display_frequency)

    last_sensor1_data = "No data"
    last_sensor2_data = "No data"
    last_sensor3_data = "No data"

    try:
        while True:
            cam_data = cam.data if cam.data is not None else "No data"

            if not sensor1_queue.empty():
                last_sensor1_data = sensor1_queue.get()
            if not sensor2_queue.empty():
                last_sensor2_data = sensor2_queue.get()
            if not sensor3_queue.empty():
                last_sensor3_data = sensor3_queue.get()


            if isinstance(cam_data, str):
                img = None
            else:
                img = cam_data.copy()

            if img is not None:
                window.show(img, [
                    f"Sensor1 (100 Hz): {last_sensor1_data}",
                    f"Sensor2 (10 Hz): {last_sensor2_data}",
                    f"Sensor3 (1 Hz): {last_sensor3_data}"
                ])

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

            time.sleep(1 / args.display_frequency)
    except KeyboardInterrupt:
        logging.info("Program terminated by user")
    finally:
        cam.stop()
        sensor1.stop()
        sensor2.stop()
        sensor3.stop()

        for thread in threads:
            thread.join()

        del cam
        del window
        logging.info("All resources have been released")

if __name__ == "__main__":
    main()