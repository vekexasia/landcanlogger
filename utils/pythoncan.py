import argparse
import time
import uuid
import can
import paho.mqtt.client as mqtt
import ssl

startTime = time.time()

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client: mqtt.Client, userdata, flags, rc):
	print("Connected with result code "+str(rc))

	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	print(client.subscribe("mower/can"))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg: mqtt.MQTTMessage):
	m = str(msg.payload).split('\'')[1]
	index = int(m.split(',')[0], 16)
	payload = bytearray.fromhex(m.split(',')[1].replace(' ',''))
	canMsg = can.Message(
		arbitration_id=index,
		#timestamp= int(round((time.time() - startTime)* 1000)),
		timestamp= int(round((time.time() -startTime) * 1000)),
		data=payload,
		is_extended_id=False
	)

	flags = 0
	if (canMsg.is_extended_id): flags += 1
	if (canMsg.is_remote_frame): flags += 2
	if (canMsg.is_fd): flags += 4
	if (canMsg.error_state_indicator): flags += 8

	microsStamp = int(canMsg.timestamp ).to_bytes(8, 'little')
	fullTopic = arg_results.topic + "/" + str(canMsg.arbitration_id)
	mqttMsg = (microsStamp + int(flags).to_bytes(1, 'little') + canMsg.data)
	print(hex(index) + str(payload))
	client.publish(fullTopic, mqttMsg, qos=0)
	
	
	
client_id = str(uuid.uuid4())

parser = argparse.ArgumentParser(description='SocketCAN To MQTT Conduit')
parser.add_argument('-u', action='store', dest='username', help='Specify MQTT Username')
parser.add_argument('-p', action='store', dest='password', help='Specify MQTT Password')
parser.add_argument('-b', action='store', dest='bustype', default='socketcan', help='Set usage of a different bus type (defaults to socketcan)')
parser.add_argument('-i', action='store', dest='channel', default='can0', help='Specify which socketcan interface to use')
parser.add_argument('-s', action='store', dest='speed', default=500000, type=int, help='Set speed of socketcan interface')
parser.add_argument('-t', action='store', dest='topic', default="can", help='Set MQTT topic to use')
parser.add_argument('-H', action='store', dest='mqtthost', default="api.savvycan.com", help='Set hostname of MQTT Broker')
parser.add_argument('-P', action='store', dest='mqttport', default=8883, type=int, help='Set port to connect to on MQTT Broker')

arg_results = parser.parse_args()
print(arg_results)
# bus = can.interface.Bus(channel=arg_results.channel, bustype=arg_results.bustype, bitrate=arg_results.speed)
	
client = mqtt.Client(client_id=client_id, clean_session=True)
client.on_connect = on_connect
client.on_message = on_message

if arg_results.mqttport == 8883:
	client.tls_set(ca_certs=None, certfile=None, keyfile=None, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLS, ciphers=None)
if len(arg_results.username) > 0:
	client.username_pw_set(arg_results.username, arg_results.password)

client.connect(arg_results.mqtthost, arg_results.mqttport, 60)

client.loop_forever()