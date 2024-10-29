import requests

url = 'http://192.168.23.149:8080/ReceiveData'
data = {
    'ID': '1',
    'Co': '40',
    'VOC' : '500',
    'Temp' : '31',
    'Hum' : '61'
}

response = requests.post(url, json=data)  # For JSON data
# Or use `data=data` for form-encoded data (application/x-www-form-urlencoded)

print(response.status_code)
print(response.text)  # Assuming the server returns JSON
