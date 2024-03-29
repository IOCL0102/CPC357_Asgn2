import pymongo 
import paho.mqtt.client as mqtt 
from datetime import datetime  


server_ip = 'localhost' 

# MQTT configuration 
mqtt_broker_address = server_ip
mqtt_topic = ['GATE_MODULE', 'CAR_DETECTOR_MODULE']  

# MongoDB configuration 
mongo_client = pymongo.MongoClient(f"mongodb://{server_ip}:27017/") 
db = mongo_client["smart_parking_system"] 
  

def on_message(client, userdata, message):    
    
    payload = message.payload.decode('utf-8')     
    print(f'Topic : {message.topic} Received message: {payload}')     

    collection = db[message.topic]
    # Convert MQTT timestamp to datetime     
    timestamp = datetime.utcnow()  # Use current UTC time     
    datetime_obj = timestamp.strftime("%Y-%m-%dT%H:%M:%S.%fZ")      
    
    # Process the payload and insert into MongoDB with proper timestamp     
    document = {         
        'timestamp': datetime_obj,         
        'data': payload     
    }     

    collection.insert_one(document)     
    print('Data ingested into MongoDB')  

client = mqtt.Client() 
client.on_message = on_message  

# Connect to MQTT broker 
client.connect(mqtt_broker_address, 1883, 60)  

# Subscribe to MQTT topic 
for topic in mqtt_topic:     
    client.subscribe(topic)

client.loop_forever()
