import paho.mqtt.client as mqtt
import time

TOPIC = "uark/csce5013/"

def on_connect(client, userdata, flag, rc):
  print("Connected with result code " + str(rc))

def on_message(client, userdata, msg):
  print(msg.topic + "; " + str(msg.payload))

# Get instance of a MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.hivemq.com", 1883, 60)

client.subscribe(TOPIC)

client.loop_start()

print("Server open! Listening for notifications on " + TOPIC)

# Just some code used to test the listener, MQTT pinged correctly
#
#userIn = ""
#while(userIn != "STOP"):
#  userIn = input("Input message for phrase topic:")
#  if(userIn != "STOP"):
#    client.publish(TOPIC, userIn)

while(True):
  pass