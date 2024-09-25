import requests

url = 'http://192.168.10.129:8080/Warning'
data = {
    'No': '',
    'Local': 'toilet2',
    'From' : '001',
    'Time' : '7:09PM',
    'Date' : '18/09/2006'
}

response = requests.post(url, json=data)  # For JSON data
# Or use `data=data` for form-encoded data (application/x-www-form-urlencoded)

print(response.status_code)
print(response.text)  # Assuming the server returns JSON
