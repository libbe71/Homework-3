/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//                                  I0                       I1
//                              10.0.1.0/30             10.0.2.0/30
//                  |--------------------------------n3-------------|
//       n0         n1         n2                                   |-n6   n7   n8  
//       |          |          |      n4-------------n5-------------| |    |    |    
//       =======================             I2              I3       ===========
//                CSMA                   10.0.3.0/30     10.0.4.0/30     CSMA
//           192.138.1.0/24                                          192.138.2.0/24
//               25 Mbs                                                  30 Mbs
//               10 us                                                   20 us
// 
//
//   I{0, 1, 2, 3} 
//       80 Mbs
//        5 us
//
//
//p2p: I0, ..., I3
//CSMA n0,n1,n2; n6,n7,n8

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1");

int
main(int argc, char* argv[])
{

    LogComponentEnable("Task_1", LOG_LEVEL_INFO);
    
    //bool verbose = true;
    uint32_t configuration = 0;

    CommandLine cmd(__FILE__);
    cmd.AddValue("configuration", "type: 0, 1 or 2", configuration);

    cmd.Parse(argc, argv);

    if(configuration > 2 || configuration < 0) 
        printf("errore\n");
    else{
         

        NodeContainer allNodes;
        allNodes.Create(9); //index 0 = n0 //index 1 = n1 //index 2 = n2
        //index 3 = n3 //index 4 = n4 //index 5 = n5 //index 6 = n6 
        //index 7 = n7 //index 8 = n8 

        NS_LOG_INFO("Create nodes.");

        NodeContainer I0;
        I0.Add(allNodes.Get(1));
        I0.Add(allNodes.Get(3));

        NodeContainer I1;
        I1.Add(allNodes.Get(3));
        I1.Add(allNodes.Get(6));

        NodeContainer I2;
        I2.Add(allNodes.Get(4));
        I2.Add(allNodes.Get(5));

        NodeContainer I3;
        I3.Add(allNodes.Get(5));
        I3.Add(allNodes.Get(6));


        NodeContainer csmaNodes1;
        csmaNodes1.Add(allNodes.Get(0));
        csmaNodes1.Add(allNodes.Get(1));
        csmaNodes1.Add(allNodes.Get(2));

        NodeContainer csmaNodes2;
        csmaNodes1.Add(allNodes.Get(6));
        csmaNodes1.Add(allNodes.Get(7));
        csmaNodes1.Add(allNodes.Get(8));

        
        NS_LOG_INFO("Create channels.");

        //setup helpers
        PointToPointHelper I0123H;
        I0123H.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
        I0123H.SetChannelAttribute("Delay", StringValue("5us"));

        CsmaHelper csma1;
        csma1.SetChannelAttribute("DataRate", StringValue("25Mbps"));
        csma1.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));


        //setup net devices
        NetDeviceContainer I0D;
        I0D = I0123H.Install(I0);
    
        NetDeviceContainer I1D;
        I1D = I0123H.Install(I1);

        NetDeviceContainer I2D;
        I2D = I0123H.Install(I2);

        NetDeviceContainer I3D;
        I3D = I0123H.Install(I3);

       

        NetDeviceContainer csmaDevices1;
        csmaDevices1 = csma1.Install(csmaNodes1);

        CsmaHelper csma2;
        csma2.SetChannelAttribute("DataRate", StringValue("30Mbps"));
        csma2.SetChannelAttribute("Delay", TimeValue(MicroSeconds(20)));

        NetDeviceContainer csmaDevices2;
        csmaDevices2 = csma2.Install(csmaNodes2);


        //instaling interner stack
        InternetStackHelper internet;
        internet.Install(allNodes);

        NS_LOG_INFO("Assign IP Addresses.");

        Ipv4AddressHelper address;

        address.SetBase("10.0.1.0", "/30", "0.0.0.1");
        Ipv4InterfaceContainer I0Interfaces;
        I0Interfaces = address.Assign(I0D);

        address.SetBase("10.0.2.0", "/30", "0.0.0.1");
        Ipv4InterfaceContainer I1Interfaces;
        I1Interfaces = address.Assign(I1D);

        address.SetBase("10.0.3.0", "/30", "0.0.0.1");
        Ipv4InterfaceContainer I2Interfaces;
        I2Interfaces = address.Assign(I2D);

        address.SetBase("10.0.4.0", "/30", "0.0.0.1");
        Ipv4InterfaceContainer I3Interfaces;
        I3Interfaces = address.Assign(I3D);

        address.SetBase("192.138.1.0", "/24", "0.0.0.1");
        Ipv4InterfaceContainer csmaInterfaces1;
        csmaInterfaces1 = address.Assign(csmaDevices1);

        
        address.SetBase("192.138.2.0", "/24", "0.0.0.1");
        Ipv4InterfaceContainer csmaInterfaces2;
        csmaInterfaces2 = address.Assign(csmaDevices2);
        if(configuration == 0){
            

            NS_LOG_INFO("starting configuration 0");
            //packet sink n2


            NS_LOG_INFO("setup sink");
            uint16_t sinkPort = 2400;
            Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
            PacketSinkHelper sink("ns3::TcpSocketFactory", sinkLocalAddress);
            ApplicationContainer sinkApps = sink.Install(allNodes.Get(2));



            //TCP OnOff Client sul nodo n4
           NS_LOG_INFO("setup onoff");

            OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
            onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
            onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
            onOffHelper.SetAttribute("PacketSize", UintegerValue(1500));
            AddressValue remoteAddress(InetSocketAddress(csmaInterfaces1.GetAddress(2), sinkPort));
            onOffHelper.SetAttribute("Remote", remoteAddress);
            ApplicationContainer nodeApp = onOffHelper.Install (allNodes.Get(4));
            nodeApp.Start (Seconds (3.0));
            nodeApp.Stop (Seconds (15.0));


            csma1.EnableAscii("task1-0-1.tr", allNodes.Get(2)->GetDevice(0),true);
            I0123H.EnableAscii("task1-0-9.tr", allNodes.Get(4)->GetDevice(0),true);

            //abilitazione pcap
            //I0123H.EnablePcap("n3", allNodes.Get(2)->GetDevice(0),true); // Pcap su n3
            //I0123H.EnablePcap("n5", allNodes.Get(4)->GetDevice(0),true); // Pcap su n5
            //csma2.EnablePcap("n6", allNodes.Get(6)->GetDevice(0),true); // Pcap su n6
            I0123H.EnablePcapAll("ciao");
            csma1.EnablePcapAll("ciao1", true);
            csma2.EnablePcapAll("ciao2", true);
            //fine pcap

        }

    }
   //Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    
    Simulator::Stop(Seconds(20));
    Simulator::Run(); 
    
    Simulator::Destroy();
    
	return 0;


}
