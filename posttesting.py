import requests

url1 = 'http://192.168.69.15:8080/ReceiveData'
url2 = 'http://192.168.69.15:8080/Warning'
data = {
    'ID': '1',
    'Co': '80',
    'VOC' : '500',
    'Temp' : '31',
    'Hum' : '61'
}

response = requests.post(url1, json=data)  # For JSON data
# Or use `data=data` for form-encoded data (application/x-www-form-urlencoded)

print(response.status_code)
print(response.text)  # Assuming the server returns JSON

data = {
    'No': '',
    'Local': 'test',
    'From' : '0',
    'Time' : '',
    'Date' : ''
}

if(response.status_code == 301):
    response = requests.post(url2, json=data)
    pass