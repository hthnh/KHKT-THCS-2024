import json
from flask import Flask, jsonify, request
import cv2
import os
from datetime import datetime
import time
from threading import Thread
import threading


app = Flask(__name__)

Client_data_file = 'clients.json'
WarningHistory_file = "warning.json"


def load_data(dic):
    if os.path.exists(dic):
        with open(dic, 'r') as f:
            return json.load(f)
    return []

def save_data(dic,data):
    with open(dic, 'w') as f:
        json.dump(data, f, indent=4)

# Initialize client data
ArClients = load_data(Client_data_file)
WarFile = load_data(WarningHistory_file)
nextArClientId = max(client['id'] for client in ArClients) + 1 if ArClients else 1
nextWarnNo = max(client['No'] for client in WarFile) + 1 if WarFile else 1


def recordVideo():
    cap = cv2.VideoCapture(0)
    # Create a folder with the current date and time
    current_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
    folder_name = f'video_recordings/{current_time}'
    os.makedirs(folder_name, exist_ok=True)
    video_filename = os.path.join(folder_name, 'output_video.mp4')
    fourcc = cv2.VideoWriter_fourcc(*'MP4V')  # Codec for .mp4 format
    fps = 30.0  # Frames per second
    frame_size = (int(cap.get(3)), int(cap.get(4)))  # Get width and height of the frames
    out = cv2.VideoWriter(video_filename, fourcc, fps, frame_size)

    # Set the desired duration in seconds (e.g., 10 seconds)
    record_duration = 300  # Set this to the duration you want
    start_time = time.time()
    
    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()

        if not ret:
            break

        # Write the frame to the output video file
        out.write(frame)

        # Display the frame
        cv2.imshow('Recording...', frame)

        elapsed_time = time.time() - start_time
        if elapsed_time > record_duration:
            print(f"Recording finished after {record_duration} seconds.")
            break

        # Press 'q' to stop recording
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    out.release()
    cap.release()
    cv2.destroyAllWindows()    





@app.route('/', methods=['GET'])
def get_status_server():
    return "Hello, Server is activated"

@app.route('/ArClient', methods=['GET'])
def get_ArClients():
    return jsonify(ArClients)

@app.route('/ArClients/<int:id>', methods=['GET'])
def get_ArClient_by_id(id: int):
    ArClient = get_ArClient(id)
    if Arclient is None:
        return jsonify({ 'error': 'ArClient does not exist'}), 404
    return jsonify(ArClient)

def get_ArClient(id):
    return next((e for e in ArClients if e['id'] == id), None)

def ArClient_is_valid(ArClient):
    for key in ArClient.keys():
        if key != 'name':
            return False
        return True

@app.route('/ArClients', methods=['POST'])
def create_ArClient():
    global nextArClientId
    ArClient = json.loads(request.data)
    if not ArClient_is_valid(ArClient):
        return jsonify({'error': 'Invalid ArClient properties.'}), 400

    ArClient['id'] = nextArClientId
    nextArClientId += 1
    ArClients.append(ArClient)
    save_data(Client_data_file,ArClients)  # Save to file

    return '', 201, {'location': f'/ArClients/{ArClient["id"]}'}

@app.route('/Warning', methods=['POST'])
def start():
    t1 = threading.Thread(target=recordVideo)
    t2 = threading.Thread(target=WarningProcess)
    t1.start()
    t2.start()
    t1.join()
    t2.join()

def WarningProcess():
    global WarFile
    Warn = json.loads(request.data)
    Warn['No'] = nextWarnNo
    nextWarnNo+=1
    WarFile.append(Warn)
    save_data(WarningHistory_file,WarFile)
    return '', 201, {'location': f'/WarningHistory_file/{WarFile["id"]}'}


@app.route('/ArClients/<int:id>', methods=['PUT'])
def update_ArClient(id: int):
    ArClient = get_ArClient(id)
    if ArClient is None:
        return jsonify({'error': 'ArClient does not exist.'}), 404

    updated_ArClient = json.loads(request.data)
    if not ArClient_is_valid(updated_ArClient):
        return jsonify({'error': 'Invalid ArClient properties.'}), 400

    ArClient.update(updated_ArClient)
    save_data(Client_data_file,ArClients)  # Save to file
    return jsonify(ArClient)


@app.route('/ArClients/<int:id>', methods=['DELETE'])
def delete_ArClient(id: int):
    global ArClients
    ArClient = get_ArClient(id)
    if ArClient is None:
        return jsonify({'error': 'ArClient does not exist.'}), 404

    ArClients = [e for e in ArClients if e['id'] != id]
    save_data(Client_data_file, ArClients)  # Save to file
    return jsonify(ArClient), 200


if __name__ == '__main__':
    ArClients = load_data(Client_data_file)
    WarFile = load_data(WarningHistory_file)
    nextArClientId = max(client['id'] for client in ArClients) + 1 if ArClients else 1
    nextWarnNo = max(client['No'] for client in WarFile) + 1 if WarFile else 1
    app.run(host = "192.168.23.149",port=8080)
