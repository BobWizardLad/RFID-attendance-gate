# Security Notes
#
# 1- CLEAN INPUTS FOR INJECTION
# 2- STUDENT DATA AS OBSCURE AS POSSIBLE
# 3- ONLY TRANSMIT STUDENT RFID TAG ON OPEN AIR
# 4- SECURE RFID ASSIGNMENTS, CHANGE REGULARLY
#

import paho.mqtt.client as mqtt
import time
import numpy

# Relevant MQTT topic and Client ID
TOPIC = "uark/csce5013/robell/testFinalProj"
CLIENT = 'MQTT_HANDLER'

# Let's see a simple structure in this script to remember IDs
db = []

def on_connect(client, userdata, flag, rc):
  print("Connected with result code " + str(rc))

# Messages are saved during the script runtime
# -TODO- Package up relevant information and pass on to DB
def on_message(client, userdata, msg):
  global msgID, db

  print(msg.topic + "; " + str(msg.payload) + "from " + str(client._client_id) + "\n")
  db.append(msg.payload)

# Get instance of a MQTT client with unique session Client_ID
client = mqtt.Client(client_id=CLIENT + '_' + str(numpy.random.randint(0, 1024)))
client.on_connect = on_connect
client.on_message = on_message

# Open MQTT Comms
client.connect("broker.hivemq.com", 1883, 60)
client.subscribe(TOPIC)

# Begin
client.loop_start()
print("Server open! Listening for notifications on " + TOPIC)

# Just some code used to test the listener, MQTT pinged correctly
userIn = ""
while(userIn != "STOP"):
  userIn = input("Input message for phrase topic:")
  if(userIn == "READ"):
    for x in db:
      print(x)
  elif(userIn != "STOP"):
    client.publish(TOPIC, userIn)


#while(True):
#  pass