import requests
import google

import messages_pb2


# check esp ip
IP = "***"

print("-----RESPONSE MESSAGE1-----")
r = requests.get("http://{}/Message1".format(IP))
print(r.headers)
print(r.content)
testmessage = messages_pb2.Message1()
testmessage.ParseFromString(r.content)
print(testmessage)


print("-----RESPONSE MESSAGE2-----")
r = requests.get("http://{}/Message2".format(IP))
print(r.headers)
print(r.content)
testmessage = messages_pb2.Message2()
testmessage.ParseFromString(r.content)
print(testmessage)


print("-----RESPONSE MESSAGE3-----")
r = requests.get("http://{}/Message3".format(IP))
print(r.headers)
print(r.content)
testmessage = messages_pb2.Message3()
testmessage.ParseFromString(r.content)
print(testmessage)


print("-----RESPONSE MESSAGE4-----")
r = requests.get("http://{}/Message4".format(IP))
print(r.headers)
print(r.content)
testmessage = messages_pb2.Message4()
testmessage.ParseFromString(r.content)
print(testmessage)


print("-----REQUEST MESSAGE1-----")
testmessage = messages_pb2.Message1()
testmessage.timestamp = 1111
testmessage.content = "POST1"
station = messages_pb2.Message1.Station()
station.id = 1
station.name = "rpi"
testmessage.station.CopyFrom(station)
print(testmessage)
url = "http://{}/Message1".format(IP)
myobj = testmessage.SerializeToString()
print(myobj)
x = requests.post(url, data = myobj,
    headers={'Content-Type': 'application/octet-stream'})

print("-----REQUEST MESSAGE2-----")
testmessage = messages_pb2.Message2()
testmessage.timestamp = 2222
testmessage.anotherContent.extend([2.2, 1.1])
station = messages_pb2.Message2.Station()
station.id = 2
station.name = "rpi"
testmessage.station.CopyFrom(station)
print(testmessage)
url = "http://{}/Message2".format(IP)
myobj = testmessage.SerializeToString()
print(myobj)
x = requests.post(url, data = myobj,
    headers={'Content-Type': 'application/octet-stream'})

print("-----REQUEST MESSAGE3-----")
testmessage = messages_pb2.Message3()
testmessage.timestamp = 3333
testmessage.stationType = messages_pb2.Message3.StationType.RASPBERRY
station = messages_pb2.Message3.Station()
station.id = 3
station.name = "rpi"
testmessage.station.CopyFrom(station)
print(testmessage)
url = "http://{}/Message3".format(IP)
myobj = testmessage.SerializeToString()
print(myobj)
x = requests.post(url, data = myobj,
    headers={'Content-Type': 'application/octet-stream'})

print("-----REQUEST MESSAGE4-----")
testmessage = messages_pb2.Message4()
testmessage.timestamp = 4444
testmessage.isOK = True
station = messages_pb2.Message4.Station()
station.id = 4
station.name = "rpi"
testmessage.station.CopyFrom(station)
print(testmessage)
url = "http://{}/Message4".format(IP)
myobj = testmessage.SerializeToString()
print(myobj)
x = requests.post(url, data = myobj,
    headers={'Content-Type': 'application/octet-stream'})