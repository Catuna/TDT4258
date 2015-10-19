from scipy.io import wavfile as wav
import numpy as np

def getResolutionFromType(dataType):
    if(dataType is np.dtype(np.int32)):
        return 2**32
    elif(dataType is np.dtype(np.int16)):
        return 2**16
    elif(dataType is np.dtype(np.int8)):
        return 2**8
    elif(dataType is np.dtype(np.float32)):
        return 1
    elif(dataType is np.dtype(np.float64)):
        return 1
    elif(dataType is np.dtype(np.float16)):
        return 1
    return 0

def convertSamples(samples,dacResolution,sampleResolution,hasOffset,sampleRate):
    cData=[]
    offset=0
    if(hasOffset):
        offset=dacResolution//2
        
    scaleFactor=(float(dacResolution)/sampleResolution)*volume
    sampleSkip = float(samples[0]) / sampleRate #should not be hard coded
    sampleIndex = 0
    
    while sampleIndex < len(samples[1]):
        sample=[]
        i = samples[1][int(sampleIndex)]
        if(type(i) is np.ndarray):
            for u in i:
                sample.append(int(float(u)*scaleFactor)+offset)
        else:
            sample.append(int(float(i)*scaleFactor)+offset)
        if(len(sample)==1):
            cData.append(sample[0])
        else:
            cData.append(sample)

        sampleIndex += sampleSkip

    return cData

def writeCData(cData,path,sampleRate):
    f=open(path+".h",'w')

    numberOfChannels=1
    if(type(cData[0]) is list):
        numberOfChannels=len(cData[0])

    if(numberOfChannels>1):
        for channel in range(numberOfChannels):
            f.write("const uint16_t "+path+"DataCh"+str(channel)+" [] = {")
            for i in range(len(cData)):
                if(i<len(cData)-1):
                    f.write(str(cData[i][channel])+",")
                else:
                    f.write(str(cData[i][channel]))
                    f.write("};\n")
                
        f.write("long int "+path+"Length = "+str(len(cData))+";\n")
        f.write("int "+path+"SampleRate = "+str(sampleRate)+";\n")
    else:
        f.write("const uint16_t "+path+"Data [] = {")
        for i in range(len(cData)):
            if(i<len(cData)-1):
                f.write(str(cData[i])+",")
            else:
                f.write(str(cData[i]))
                f.write("};\n")
        f.write("long int "+path+"Length = "+str(len(cData))+";\n")
        f.write("int "+path+"SampleRate = "+str(sampleRate)+";\n")
            
    f.close()

    
fileName="lose.wav"
volume=1
dacResolution=2**12
sampleRate=44100 / 4
def main():
    print("Reading data file...")
    dataName=fileName[0:fileName.rfind(".")]
    data=wav.read(fileName)

    dataResolution=getResolutionFromType(data[1].dtype)
    if(dataResolution==0):
        print("Invalid data resolution")
        return

    offset=False
    #Check if there are negative samples. In that case, we need an offset
    for i in data[1]:
        if(i is np.ndarray and i<0): #More channels
            for u in i:
                if(u<0):
                    offset=True
                    break
        else:   #One channel
            offset=True
            break

    print("Converting data...")
    cData=convertSamples(data,dacResolution,dataResolution,offset,sampleRate)
    print("File consisting of "+str(len(cData))+" samples")
    print("Writing to file...")

    writeCData(cData,dataName,sampleRate)

    print("Done!")

main()
