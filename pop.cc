#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/point-to-point-module.h"
#include <iostream>
#include <inttypes.h>
#include <stdio.h>

//    p2p
// n0------n1    n2    n3
//         |     |     |
//         =============
//              CSMA


using namespace ns3;
NS_LOG_COMPONENT_DEFINE("---");

int main(int argc, char* argv[]){


    NodeContainer allNodes;
    allNodes.Create(4);

    NodeContainer p2pNodes = NodeContainer(allNodes.Get(0), allNodes.Get(1));
    NodeContainer csmaNodes = NodeContainer(allNodes.Get(1), allNodes.Get(2), allNodes.Get(3));

    //setup point2point helper
    PointToPointHelper p2pH;
    p2pH.SetDeviceAttribute ("DataRate", StringValue ("80Mbps"));
    p2pH.SetChannelAttribute ("Delay", StringValue("10us"));

    //setup csma helper
    CsmaHelper csmaH;
    csmaH.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    csmaH.SetChannelAttribute("Delay", StringValue("2ms"));

    //p2p Device Container
    NetDeviceContainer p2pDevices;
    p2pDevices = p2pH.Install(p2pNodes);

    //csma Device Container
    NetDeviceContainer csmaDevices;
    csmaDevices = csmaH.Install(csmaNodes);

    //instaling interner stack
    InternetStackHelper internet;
    internet.Install(allNodes);

    Ipv4AddressHelper ipv4P2P;
    ipv4P2P.SetBase("10.0.2.0", "/30", "0.0.0.1");
    Ipv4InterfaceContainer p2pInterfaces = ipv4P2P.Assign(p2pDevices);

    Ipv4AddressHelper ipv4csma;
    ipv4csma.SetBase("192.138.2.0", "/24", "0.0.0.1");
    Ipv4InterfaceContainer csmaInterfaces = ipv4csma.Assign(csmaDevices);




    //packet sink n2
    uint16_t sinkPort = 2400;
    Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
    PacketSinkHelper sink("ns3::TcpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApps = sink.Install(allNodes.Get(2));



    //TCP OnOff Client sul nodo n0
    OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
    onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1500));
    AddressValue remoteAddress(InetSocketAddress(csmaInterfaces.GetAddress(1), sinkPort));
    onOffHelper.SetAttribute("Remote", remoteAddress);
    ApplicationContainer nodeApp = onOffHelper.Install (allNodes.Get(0));
    nodeApp.Start (Seconds (3.0));
    nodeApp.Stop (Seconds (15.0));

    p2pH.EnablePcap("p2p", allNodes.Get(0) -> GetDevice(0));
    p2pH.EnablePcap("p2p", allNodes.Get(1) -> GetDevice(0));
    csmaH.EnablePcap("csma", allNodes.Get(2) -> GetDevice(0));
    csmaH.EnablePcap("csma", allNodes.Get(3) -> GetDevice(0));
    

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    
    Simulator::Stop(Seconds(20));
    Simulator::Run(); 
    
    Simulator::Destroy();
    
	return 0;

}