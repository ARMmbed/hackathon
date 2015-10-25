from peewee import *
import requests
from bs4 import BeautifulSoup
import pyaudio
import struct
import math
import audioop
import time
import numpy as np
import scipy
import serial

port = serial.Serial("COM3", 9600)

stream_url = 'http://localhost:8000'
database = SqliteDatabase(r'C:\Users\George Fortune\Local Settings\Application Data\Mixxx\mixxxdb.sqlite')
format = pyaudio.paInt16
rate = 10000
channels = 2
input_block_time = 1
input_frames_per_block = int(input_block_time*rate)
pa = pyaudio.PyAudio()

class BaseModel(Model):
    class Meta:
        database = database
   
   
class library(BaseModel):
    id = PrimaryKeyField()
    artist = TextField()
    title = TextField()
    album = TextField()
    year = DateTimeField()
    genre = TextField()
    tracknumber = IntegerField()
    location = IntegerField()
    comment = TextField()
    url = TextField()
    duration = IntegerField()
    bitrate = IntegerField()
    samplerate = IntegerField()
    cuepoint = IntegerField()
    bpm = FloatField()
  
  
    class Meta:
        db_table = 'library'

def getartist():
    try:
        #new request
        response = requests.get(stream_url, headers={'Icy-MetaData': 1}, stream=True)
        response.raise_for_status()
        headers, stream = response.headers, response.raw
        meta_int = headers.get('icy-metaint')
        meta_byte = stream.read(1)
        if (meta_byte):
            meta_length = ord(meta_byte) * 16
            meta_data = stream.read()
            mymetadata = BeautifulSoup(meta_data, 'html.parser')
            mymetadata = str(mymetadata.get_text())
            mymetadata = mymetadata.split("playing:",1)[1]
            mymetadata = (mymetadata.split("Support",1)[0]).rstrip()
            return mymetadata
    except KeyboardInterrupt:
        return "Failed"

def sin(bpm):
    f = bpm/60
    x = np.linspace(0,1,rate/f)
    y = scipy.cos(2*np.pi*x*f)
    y = np.int16(y*1000)
    #print(y)
    return y    
    
def signal_analysis(bpm):
    stream = pa.open(format=format,
            channels=channels,
            rate=rate,
            input=True,
            frames_per_buffer=input_frames_per_block)       
    block = stream.read(input_frames_per_block)
    offset,factor = audioop.findfit(block, sin(bpm))
    #print("factor is ", factor)
    z=offset*bpm/(4*rate*60)
    phase = z - math.floor(z)
    print(offset,phase)
    return phase
 
class main():
    for track in library.select(): 
        if (track.title == getartist()):   
            print ("Track title is: ", track.title, "BPM is: ", track.bpm) 
            bpm = track.bpm
    print("BPM is: ", bpm)  
    b = 1
    while(b == 1):    
        phase = signal_analysis(bpm)    
        offset_ms = int((60000 * phase) / bpm)
        bpm = int(bpm)
    
        port.write([(offset_ms >> 8) & 0xff])
        port.write([offset_ms & 0xff])
        port.write([(bpm >> 8) & 0xff])
        port.write([bpm & 0xff])
    
if __name__ == "__name__":
    main()
    
    
