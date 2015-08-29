import csv
import math

current="1024"
ofile=open('out.csv','a')
writer=csv.writer(ofile,delimiter=',')
row=[current+"Kbps","delaySum(s)","Throughput(Kbps)","jitter"]
writer.writerow(row)

delaySum={}
Throughput={}
jitter={}


for j in range(10):
	ifile = open(current+"csma"+str(j+1)+".txt","r")
	timeFirstRPacket=timeFirstTPacket=11.0
	timeLastRPacket=timeLastTPacket=0.0
	delaySum[j]=0.0
	jitter[j]=0.0
	pt=0
	lastdelay=0.0
	node={}
	for i in range(8):
		node[i]={}
		for k in range(1000):
			node[i][k]=[0.0,0.0,0.0,0]
	for line in ifile:
		terms = line.split(' ')
		time = float(terms[1])
		packetID = int(terms[terms.index("id")+1])
		#packetsize=int(terms[terms.index("Payload")+1][6:len(terms[terms.index("Payload")+1])-1])
		words = terms[2].split('/')
		nodeID=int(words[2])
		if(terms[0]=='+'):
			node[nodeID][packetID][0]=time
		elif(terms[0]=='-'):
			timeFirstTPacket=min(timeFirstTPacket,time)
			timeLastTPacket=max(timeLastTPacket,time)
			node[nodeID][packetID][1]=time
		elif(terms[0]=='r'):
			timeFirstRPacket=min(timeFirstRPacket,time)
			timeLastRPacket=max(timeLastRPacket,time)
			pt+=1
			com_nodeID=0
			if(nodeID<=3):
				com_nodeID=nodeID+4
			else:
				com_nodeID=nodeID-4
			node[com_nodeID][packetID][2]=time
			delaySum[j]+=time-node[com_nodeID][packetID][0]
			#jitter[j]+=abs(time-node[com_nodeID][packetID][0]-lastdelay)
			#lastdelay=time-node[com_nodeID][packetID][0]


	delaySum[j]/=pt

	for i in range(8):
		x=0
		while(x<1000):
			if(node[i][x][2]>0.0):
				jitter[j]+=(delaySum[j]-(node[i][x][2]-node[i][x][0]))**2
				x+=1
			else:
				break

	jitter[j]=math.sqrt(jitter[j]/pt)



	ifile.close()

	
	Throughput[j]=(pt*(int(current))/32)/(timeLastRPacket-timeFirstRPacket)

	row=["experiment "+str(j+1),str(delaySum[j]),str(Throughput[j]),str(jitter[j])]
	writer.writerow(row)

avgDelay=0.0
avgThroughput=0.0
avgJitter=0.0
for i in range(10):
	avgDelay+=delaySum[i]
	avgThroughput+=Throughput[i]
	avgJitter+=jitter[i]
avgDelay/=10
avgThroughput/=10
avgJitter/=10


row=["Average",str(avgDelay),str(avgThroughput),str(avgJitter)]
writer.writerow(row)

delaySD=0.0
ThroughputSD=0.0
JitterSD=0.0

for i in range(10):
	delaySD+=(delaySum[i]-avgDelay)**2
	ThroughputSD+=(Throughput[i]-avgThroughput)**2
	JitterSD+=(jitter[i]-avgJitter)**2

row=["SD",str(math.sqrt(delaySD/10)),str(math.sqrt(ThroughputSD/10)),str(math.sqrt(JitterSD/10))]
writer.writerow(row)

ofile.close()

plotfile=open('plotDelay.txt','a')
plotfile.write(current+"\t"+str(avgDelay)+"\t"+str(delaySD)+"\n")
plotfile.close()

plotfile=open('plotThroughput.txt','a')
plotfile.write(current+"\t"+str(avgThroughput)+"\t"+str(ThroughputSD)+"\n")
plotfile.close()

plotfile=open('plotJitter.txt','a')
plotfile.write(current+"\t"+str(avgJitter)+"\t"+str(avgJitter)+"\n")
plotfile.close()