import json
from flask import Flask, jsonify, request
import cv2
import os
from datetime import datetime
import time
from telegram import Bot

Token_bot = "7284069938:AAEzY6BpHskyexgtlpmCCW1BZQR7SPl7FEg"
Chat_id = "6776867686"
app = Flask(__name__)
bot = Bot(token=Token_bot)

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


def send_message(chat){
    bot.send_message(chat_id=Chat_id, text = chat)
}

def record_video(record_duration):
    cap = None
    out = None
    try:
        current_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        folder_name = f'video_recordings/{current_time}'
        os.makedirs(folder_name, exist_ok=True)
        video_filename = os.path.join(folder_name, 'output_video.mp4')

        cap = cv2.VideoCapture(0, cv2.CAP_ANY)


        fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # Codec for .avi format
        fps = 20.0  # Frames per second
        frame_size = (int(cap.get(3)), int(cap.get(4)))  # Get width and height of the frames

        out = cv2.VideoWriter(video_filename, fourcc, fps, frame_size)

        if not cap.isOpened():
            print("Error: Could not open webcam")
            return

        start_time = time.time()
        print(f"Recording started for {record_duration} seconds. Saving to folder: {folder_name}")

        cv2.startWindowThread()
        # cv2.namedWindow('Recording...', cv2.WINDOW_NORMAL)

        while True:
            ret, frame = cap.read()
            if not ret:
                break
            out.write(frame)
            cv2.imshow('Recording...', frame)
            elapsed_time = time.time() - start_time
            if elapsed_time > record_duration:
                print(f"Recording finished after {record_duration} seconds.")
                break

            if cv2.waitKey(1) & 0xFF == ord('q'):
                print("Recording stopped by user.")
                break

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        # Ensure that resources are released even in case of an error
        if cap is not None and cap.isOpened():
            cap.release()
            print("Video capture released.")
        if out is not None:
            out.release()
            print("Video writer released.")
        cv2.destroyAllWindows()
        print(f"Recording saved to {video_filename}")



@app.route('/', methods=['GET'])
def get_status_server():
    return "Hello, Server is activated"

@app.route('/ArClients', methods=['GET'])
def get_ArClients():
    return jsonify(ArClients)

@app.route('/ArClients/<int:id>', methods=['GET'])
def get_ArClient_by_id(id: int):
    ArClient = get_ArClient(id)
    if ArClient is None:
        return jsonify({ 'error': 'ArClient does not exist'}), 404
    return jsonify(ArClient)

@app.route('/ArClients/ip/<string:ip_adr>', methods=['GET'])
def get_ArClient_by_ip(ip_adr):
    # Find the location with the matching IP address
    ArClient = next((loc for loc in ArClients if loc['ip'] == ip_adr), None)
    
    if ArClient:
        return jsonify(ArClient), 200
    else:
        return jsonify({'error': 'Location not found for the given IP'}), 404


def get_ArClient(id):
    return next((e for e in ArClients if e['id'] == id), None)

def ArClient_is_valid(ArClient):
    for key in ArClient.keys():
        if key != 'id':
            return False
        return True

def Warning_is_valid(Warn):
    for key in Warn.keys():
        if key != 'No':
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


@app.route('/Warning', methods=['GET'])
def get_Warning():
    return jsonify(WarFile)




@app.route('/Warning', methods=['POST'])
def start():
    Warn = json.loads(request.data)
    if not Warning_is_valid(Warn): 
        return jsonify({'error': 'Invalid ArClient properties.'}), 400
    global nextWarnNo
    Warn['No'] = nextWarnNo
    Warn['Time'] = datetime.now().strftime('%H:%M:%Ss')
    Warn['Date'] = datetime.now().strftime('%d-%m-%Y')
    nextWarnNo+=1
    WarFile.append(Warn)
    save_data(WarningHistory_file,WarFile)
    send_message("Phat hien hut thuoc tai phong: " + Warn['Local'])
    record_video(record_duration=10)
    return '', 201, {'location': f'/WarFile/{Warn["No"]}'}


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
    app.run(host = "192.168.1.2",port=8080)

