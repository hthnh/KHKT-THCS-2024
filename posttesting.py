import requests

url = 'http://192.168.23.149:8080/'
data = {
    'Id': 'value1',
    'W': 'value2'
}

response = requests.post(url, data=data)  # For JSON data
# Or use `data=data` for form-encoded data (application/x-www-form-urlencoded)

print(response.status_code)
print(response.text)  # Assuming the server returns JSON
