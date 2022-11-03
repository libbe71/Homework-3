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

    //bool verbose = true;
    uint32_t nCsma1 = 2;
    uint32_t nCsma2 = 2;
    uint32_t configuration = 0;

    LogComponentEnable("1", LOG_INFO);
    LogComponentEnable("2", LOG_INFO);

    CommandLine cmd(__FILE__);
    cmd.AddValue("configuration", "type: 0, 1 or 2", configuration);

    cmd.Parse(argc, argv);

    if(configuration > 2 || configuration < 0) 
        printf("errore\n");
    else{
         NodeContainer I0;
         I0.Create(2); //index 0 = n1 index 1 = n3


         NodeContainer I1;
         I1.Add(I0.Get(1)); //index 0 n3
         I1.Create(1); // index 1 = n6

         NodeContainer I2;
         I2.Create(2); // index 0 = n4 index 1 = n5

         NodeContainer I3;
         I3.Add(I2.Get(1)); // index 0 n5
         I3.Add(I1.Get(1)); // index 1 = n6


        NodeContainer csmaNodes1;
        csmaNodes1.Add(I0.Get(0)); // index 0 = n1
        csmaNodes1.Create(nCsma1); // index 1 = n0, index 2 = n2

        NodeContainer csmaNodes2;
        csmaNodes2.Add(I1.Get(1)); // index 0 = n6
        csmaNodes2.Create(nCsma2); // index 1 = n7, index 2 = n8

        PointToPointHelper I0123H;
        I0123H.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
        I0123H.SetChannelAttribute("Delay", StringValue("5us"));

        NetDeviceContainer I0D;
        I0D = I0123H.Install(I0);
    
        NetDeviceContainer I1D;
        I1D = I0123H.Install(I1);

        NetDeviceContainer I2D;
        I2D = I0123H.Install(I2);

        NetDeviceContainer I3D;
        I3D = I0123H.Install(I3);

        CsmaHelper csma1;
        csma1.SetChannelAttribute("DataRate", StringValue("25Mbps"));
        csma1.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));

        NetDeviceContainer csmaDevices1;
        csmaDevices1 = csma1.Install(csmaNodes1);

        CsmaHelper csma2;
        csma2.SetChannelAttribute("DataRate", StringValue("30Mbps"));
        csma2.SetChannelAttribute("Delay", TimeValue(MicroSeconds(20)));

        NetDeviceContainer csmaDevices2;
        csmaDevices2 = csma2.Install(csmaNodes2);
        
        InternetStackHelper stack;
        stack.Install(I0.Get(1)); // n3
        stack.Install(I2.Get(0)); // n4
        stack.Install(I2.Get(1)); // n5
        stack.Install(csmaNodes1); // n0, n1, n2
        stack.Install(csmaNodes2); // n6, n7, n8
        
        

    }
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
