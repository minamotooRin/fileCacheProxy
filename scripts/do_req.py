import requests

register_url = "http://172.17.0.4:9999/upload"
# register_url = "http://172.17.0.4:9999/file_info"

header = {
    "FileID" : "group1/M00/00/00/fwAAAWJzW0mAO1MRAAAAPfT7Vy8813.abc",
    "FileExt" : "abc"
}

# json = {
#     "mobile_phone": "15612345678",
#     "pwd": "Test1234",
#     "type": 0
# }
# response = requests.post(url=register_url, json=json, headers=header)

# upload_files = {'file': open('fpmoni.sh', 'rb')}
# response = requests.post(url=register_url, files=upload_files, headers=header)

for i in range(1000):
    response = requests.post(url=register_url, headers=header, data='this is a string')

print(response.status_code)
print(response.content)
