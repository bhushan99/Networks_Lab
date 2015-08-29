import csv

ofile=open('out.csv','w')
writer=csv.writer(ofile,delimiter=',')
row=["","timeFirstTPacket","timeLastTPacket","timeFirstRPacket","timeLastRPacket","delaySum","tBytes, tPackets","rBytes, rPackets","lostPackets","timesForwarded","packetsDropped, bytesDropped","transmitterThroughput","recieverThroughput"]
writer.writerow(row)

for j in range(9):
	ifile = open("udp-echo"+str(j)+".tr","r")
	node0={}
	node1={}
	for i in range(1000):
		node0[i]=[0.0,0.0,0.0,0]
		node1[i]=[0.0,0.0,0.0,0]
	bt0=bt1=pt0=pt1=br0=br1=pr0=pr1=0
	totaltime=0.0
	packetsDropped=bytesDropped=0
	for line in ifile:
		terms = line.split(' ')
		time = float(terms[1])
		totaltime+=time
		node_id = int(terms[terms.index("id")+1])
		packetsize=int(terms[terms.index("Payload")+1][6:len(terms[terms.index("Payload")+1])-1])
		words = terms[2].split('/')
		if(words[2]=='0'):
			if(terms[0]=='+'):
				node0[node_id][0]=time
			elif(terms[0]=='-'):
				bt0+=packetsize
				node0[node_id][1]=time
				if(node0[node_id][3]==0):
					pt0+=1
				node0[node_id][3]+=1
			elif(terms[0]=='r'):
				node1[node_id][2]=time
				pr0+=1
				br0+=packetsize
			else:
				node1[node_id][2]=-1.0
				packetsDropped+=1
				bytesDropped+=packetsize
		else:
			if(terms[0]=='+'):
				node1[node_id][0]=time
			elif(terms[0]=='-'):
				bt1+=packetsize
				node1[node_id][1]=time
				if(node1[node_id][3]==0):
					pt1+=1
				node1[node_id][3]+=1
			elif(terms[0]=='r'):
				node0[node_id][2]=time
				pr1+=1
				br1+=packetsize
			else:
				node0[node_id][2]=-1.0
				packetsDropped+=1
				bytesDropped+=packetsize
	ifile.close()

	tBytes=bt0+bt1
	#tPackets=pt0+pt1
	rBytes=br0+br1
	rPackets=pr0+pr1
	timeFirstTPacket=min(node0[0][1],node1[0][1])
	timeLastTPacket=max(node0[pt0-1][1],node1[pt1-1][1])
	timeFirstRPacket=0.0
	for i in range(pt1):
		if(node0[i][2]>0.0):
			timeFirstRPacket=node0[i][2]
			break
	for i in range(pt0):
		if(node1[i][2]>0.0):
			timeFirstRPacket=min(timeFirstRPacket,node1[i][2])
			break
	timeLastRPacket=0.0
	for i in range(pt1):
		if(node0[i][2]>timeLastRPacket):
			timeLastRPacket=node0[i][2]
	for i in range(pt0):
		if(node1[i][2]>timeLastRPacket):
			timeLastRPacket=node1[i][2]
	delaySum=0.0
	lostPackets=0
	for i in range(pt0):
		if(node0[i][2]>0.0):
			delaySum+=(node0[i][2]-node0[i][1])
		elif(node0[i][2]!=-1.0):
			lostPackets+=1
	for i in range(pt1):
		if(node1[i][2]>0.0):
			delaySum+=(node1[i][2]-node1[i][1])
		elif(node1[i][2]!=-1.0):
			lostPackets+=1
	timesForwarded=0
	for i in range(pt0):
		timesForwarded+=(node0[i][3]-1)
	for i in range(pt1):
		timesForwarded+=(node1[i][3]-1)
	transmitterThroughput=(bt0+bt1)/totaltime
	recieverThroughput=(br0+br1)/totaltime
	tPackets=pt0+pt1+timesForwarded

	header=""
	if(j<5):
		header=str(64*(2**(j)))
	elif(j==5):
		row=[]
		writer.writerow(row)
		header=str(0.02)
	elif(j==6):
		header=str(0.05)
	elif(j==7):
		header=str(0.1)
	elif(j==8):
		header=str(1)

	row=[header,str(timeFirstTPacket),str(timeLastTPacket),str(timeFirstRPacket),str(timeLastRPacket),str(delaySum),str(tBytes)+","+str(tPackets),str(rBytes)+","+str(rPackets),str(lostPackets),str(timesForwarded),str(packetsDropped)+","+str(bytesDropped),str(transmitterThroughput),str(recieverThroughput)]
	writer.writerow(row)

ofile.close()
