import serial
import time
import csv
import os

# Configuration
SERIAL_PORT = '/dev/ttyACM0'  # Replace with your Arduino's serial port (e.g., '/dev/ttyACM0' or '/dev/ttyUSB0')
BAUD_RATE = 9600
CSV_FILE = 'noSmokeData.csv'

def find_serial_port():
    """Function to help find the correct serial port."""
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        print(f"Found port: {p.device} - {p.description}")
    if not ports:
        print("No serial ports found.")
    return

def main():
    # Uncomment the following line to list available serial ports
    # find_serial_port()
    
    # Initialize serial connection
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
    except serial.SerialException as e:
        print(f"Error connecting to serial port {SERIAL_PORT}: {e}")
        return

    # Wait for Arduino to reset
    time.sleep(2)

    # Check if CSV file exists; if not, create it and write headers
    if not os.path.isfile(CSV_FILE):
        with open(CSV_FILE, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "MQ7_Value", "VOC_Value", "Temperature_C", "Humidity_%", "Smoke"])
        print(f"Created new CSV file: {CSV_FILE}")
    else:
        print(f"Appending to existing CSV file: {CSV_FILE}")

    print("Starting data collection... Press Ctrl+C to stop.")

    try:
        while True:
            if ser.in_waiting > 0:
                try:
                    line = ser.readline().decode('utf-8').rstrip()
                    # Expected format: Timestamp,MQ7_Value,Temperature,Humidity
                    data = line.split(',')
                    if len(data) == 5:
                        timestamp, mq7,voc, temp, hum = data
                        # Optional: Convert data types
                        try:
                            # If using RTC, timestamp is a string; else, it's an integer
                            # Attempt to convert timestamp to int, if fails assume string
                            try:
                                timestamp = int(timestamp)
                            except ValueError:
                                pass  # Keep as string
                            mq7 = int(mq7)
                            voc = int(voc)
                            temp = float(temp)
                            hum = float(hum)

                            # Append data to CSV
                            with open(CSV_FILE, mode='a', newline='') as file:
                                writer = csv.writer(file)
                                writer.writerow([timestamp, mq7, voc, temp, hum])

                            # Print to console (optional)
                            print(f"{timestamp}, {mq7}, {voc}, {temp}°C, {hum}%")
                        except ValueError:
                            print(f"Invalid data types in line: {line}")
                    else:
                        print(f"Unexpected data format: {line}")
                except UnicodeDecodeError:
                    print("Received undecodable bytes.")
            else:
                time.sleep(0.1)  # Prevent CPU overuse
    except KeyboardInterrupt:
        print("\nData collection stopped by user.")
    finally:
        ser.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()
