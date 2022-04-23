import argparse
import time
import uuid
import can
import paho.mqtt.client as mqtt
import ssl

startTime = time.time()
	
client_id = str(uuid.uuid4())

parser = argparse.ArgumentParser(description='SocketCAN To MQTT Conduit')
parser.add_argument('-u', action='store', dest='username', help='Specify MQTT Username')
parser.add_argument('-p', action='store', dest='password', help='Specify MQTT Password')
parser.add_argument('-d', action='store', dest='data', default='0000', help='hex encoded bytes')
parser.add_argument('-i', action='store', dest='id', default='0x123', help='Specify the can id')
parser.add_argument('-t', action='store', dest='topic', default="can", help='Set MQTT topic to use')
parser.add_argument('-H', action='store', dest='mqtthost', default="api.savvycan.com", help='Set hostname of MQTT Broker')
parser.add_argument('-P', action='store', dest='mqttport', default=1883, type=int, help='Set port to connect to on MQTT Broker')

arg_results = parser.parse_args()
print(arg_results)
# bus = can.interface.Bus(channel=arg_results.channel, bustype=arg_results.bustype, bitrate=arg_results.speed)
	
client = mqtt.Client(client_id=client_id, clean_session=True)

if arg_results.mqttport == 8883:
	client.tls_set(ca_certs=None, certfile=None, keyfile=None, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLS, ciphers=None)
if len(arg_results.username) > 0:
	client.username_pw_set(arg_results.username, arg_results.password)

client.connect(arg_results.mqtthost, arg_results.mqttport, 60)


topic = arg_results.topic + "/s/" +str( int(arg_results.id, 16))

microsStamp = int(0 ).to_bytes(8, 'little')
data = bytearray.fromhex(arg_results.data.replace(' ',''))
mqttMsg = (microsStamp + int(0).to_bytes(1, 'little') + data)
client.publish(topic, mqttMsg, qos=0)
client.loop();
'''
 client.publish(arg_results.topic + "/" + id, )
'''