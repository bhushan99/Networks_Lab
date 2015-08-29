#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "src/core/model/rng-seed-manager.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("CSMAScript");

int main(int argc,char *argv[]){

    uint32_t packetSize = 1024;
    uint32_t maxPacketCount = 1000;
    Time interPacketInterval = Seconds(1.);
    string Rate;
    int seed;


    CommandLine cmd;
    cmd.AddValue ("packetSize","Use packetSize",packetSize);
    cmd.AddValue ("maxPacketCount","Use maxPacketCount",maxPacketCount);
    cmd.AddValue ("interPacketInterval","Use interPacketInterval",interPacketInterval);
    cmd.AddValue ("Rate","Use Rate",Rate);
    cmd.AddValue ("Seed","Use Seed",seed);
    cmd.Parse (argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


    int i;
    for(i=1;i<=10;i++){

        //const long double sysTimeMS = time(0)*1000;
        //RngSeedManager::SetSeed(sysTimeMS);
        RngSeedManager::SetSeed(seed);


      	NodeContainer nodes;
    	nodes.Create (8);

    	CsmaHelper csma;
    	csma.SetChannelAttribute ("DataRate", StringValue("1024Kbps"));
        csma.SetChannelAttribute ("Delay", StringValue ("2ms"));

        NetDeviceContainer devices = csma.Install (nodes);

        InternetStackHelper internet;
        internet.Install (nodes);

        Ipv4AddressHelper ipv4;
        ipv4.SetBase ("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

        
        UdpEchoServerHelper echoServer1(1);
        UdpEchoServerHelper echoServer2(2);
        UdpEchoServerHelper echoServer3(3);
        UdpEchoServerHelper echoServer4(4);

        ApplicationContainer serverApps1 = echoServer1.Install (nodes.Get (4));
        serverApps1.Start (Seconds (0.0));
        serverApps1.Stop (Seconds (10.0));
        ApplicationContainer serverApps2 = echoServer2.Install (nodes.Get (5));
        serverApps2.Start (Seconds (0.0));
        serverApps2.Stop (Seconds (10.0));
        ApplicationContainer serverApps3 = echoServer3.Install (nodes.Get (2));
        serverApps3.Start (Seconds (0.0));
        serverApps3.Stop (Seconds (10.0));
        ApplicationContainer serverApps4 = echoServer4.Install (nodes.Get (3));
        serverApps4.Start (Seconds (0.0));
        serverApps4.Stop (Seconds (10.0));



        UdpEchoClientHelper echoClient1(interfaces.GetAddress(4),1);
        echoClient1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        echoClient1.SetAttribute ("PacketSize", UintegerValue (packetSize));
    	UdpEchoClientHelper echoClient2(interfaces.GetAddress(5),2);
    	echoClient2.SetAttribute ("Interval", TimeValue (interPacketInterval));
        echoClient2.SetAttribute ("PacketSize", UintegerValue (packetSize));
        UdpEchoClientHelper echoClient3(interfaces.GetAddress(2),3);
        echoClient3.SetAttribute ("Interval", TimeValue (interPacketInterval));
        echoClient3.SetAttribute ("PacketSize", UintegerValue (packetSize));
        UdpEchoClientHelper echoClient4(interfaces.GetAddress(3),4);
        echoClient4.SetAttribute ("Interval", TimeValue (interPacketInterval));
        echoClient4.SetAttribute ("PacketSize", UintegerValue (packetSize));


        ApplicationContainer clientApps1 = echoClient1.Install (nodes.Get (0));
        clientApps1.Start (Seconds (0.0));
        clientApps1.Stop (Seconds (10.0));
        ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get (1));
        clientApps2.Start (Seconds (0.0));
        clientApps2.Stop (Seconds (10.0));
        ApplicationContainer clientApps3 = echoClient3.Install (nodes.Get (6));
        clientApps3.Start (Seconds (0.0));
        clientApps3.Stop (Seconds (10.0));
        ApplicationContainer clientApps4 = echoClient4.Install (nodes.Get (7));
        clientApps4.Start (Seconds (0.0));
        clientApps4.Stop (Seconds (10.0));

        char str[20];
        sprintf(str,"%d",i);

        AsciiTraceHelper ascii;
        csma.EnableAsciiAll (ascii.CreateFileStream (Rate+"csma"+string(str)+".tr"));
        //csma.EnablePcapAll ("csma"+str(i+1));

        Simulator::Run ();
        Simulator::Destroy ();
    }
    return 0;
}
