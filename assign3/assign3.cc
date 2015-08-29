#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("CSMAScript");

int main(int argc,char *argv[]){

	Time::SetResolution(Time::NS);
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  	NodeContainer nodes;
	nodes.Create (8);

	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", StringValue("1024Kbps"));
    csma.SetChannelAttribute ("Delay", StringValue("2ms"));

    NetDeviceContainer devices = csma.Install (nodes);

    InternetStackHelper internet;
    internet.Install (nodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

    int i;



    UdpEchoServerHelper echoServer[4];
    for(i=0;i<4;i++)
    	echoServer[i].SetAttribute("port",uint16_t(i+1));

    ApplicationContainer serverApps[4];
    serverApps[0] = echoServer[0].Install (nodes.Get (4));
    serverApps[0].Start (Seconds (1.0));
    serverApps[0].Stop (Seconds (10.0));
    serverApps[1] = echoServer[1].Install (nodes.Get (5));
    serverApps[1].Start (Seconds (1.0));
    serverApps[1].Stop (Seconds (10.0));
    serverApps[2] = echoServer[2].Install (nodes.Get (2));
    serverApps[2].Start (Seconds (1.0));
    serverApps[2].Stop (Seconds (10.0));
    serverApps[3] = echoServer[3].Install (nodes.Get (3));
    serverApps[3].Start (Seconds (1.0));
    serverApps[3].Stop (Seconds (10.0));



    UdpEchoClientHelper echoClient[4];
  
    echoClient[0].SetAttribute("ip",interfaces.GetAddress (4));
    echoClient[0].SetAttribute("port",uint16_t(1));
	echoClient[1].SetAttribute("ip",interfaces.GetAddress (5));
	echoClient[1].SetAttribute("port",uint16_t(2));
	echoClient[2].SetAttribute("ip",interfaces.GetAddress (2));
	echoClient[2].SetAttribute("port",uint16_t(3));
	echoClient[3].SetAttribute("ip",interfaces.GetAddress (3));
	echoClient[3].SetAttribute("port",uint16_t(4));

    ApplicationContainer clientApps[4];
    clientApps[0] = echoClient[0].Install (nodes.Get (0));
    clientApps[0].Start (Seconds (1.0));
    clientApps[0].Stop (Seconds (10.0));
    clientApps[1] = echoClient[1].Install (nodes.Get (1));
    clientApps[1].Start (Seconds (1.0));
    clientApps[1].Stop (Seconds (10.0));
    clientApps[2] = echoClient[2].Install (nodes.Get (6));
    clientApps[2].Start (Seconds (1.0));
    clientApps[2].Stop (Seconds (10.0));
    clientApps[3] = echoClient[3].Install (nodes.Get (7));
    clientApps[3].Start (Seconds (1.0));
    clientApps[3].Stop (Seconds (10.0));


    AsciiTraceHelper ascii;
    csma.EnableAsciiAll (ascii.CreateFileStream ("csma.tr"));
    csma.EnablePcapAll ("csma");

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
