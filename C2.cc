    else if (configuration == 2){
        
        //UDP Echo Server su n2
        UdpEchoServerHelper echoServer (63);
        ApplicationContainer udpApps = echoServer.Install (star.GetSpokeNode(1));

        //UDP Echo Client su n8
        UdpEchoClientHelper echoClient (star.GetSpokeIpv4Address(1), 63);
        echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0))); //PACCHETTI DA INVIARE 3s,5s,7s,9s,11s
        echoClient.SetAttribute("PacketSize", UintegerValue(2560));

        std::string matricola = "Matricola: 779780 |";
        int size = 2559 - matricola.size();
        for( int i = 0; i<size ; i++){
            matricola += char((configuration*5)*i);
        }

        udpApps = echoClient.Install (csma2Nodes.Get (1)); 
        echoClient.SetFill (udpApps.Get(0), matricola);
        udpApps.Start (Seconds (3.0));
        udpApps.Stop (Seconds (12.0));

        //TCP SINK n1
        uint16_t portN1 = 2600;
        Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), portN1));
        PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
        ApplicationContainer sinkApp = packetSinkHelper.Install (star.GetSpokeNode(0));

        //UDP SINK n3
        uint16_t portN3 = 2500;
        Address udpSink (InetSocketAddress (Ipv4Address::GetAny (), portN3));
        PacketSinkHelper udpSinkHelper ("ns3::UdpSocketFactory", udpSink);
        ApplicationContainer udpApp = udpSinkHelper.Install (star.GetSpokeNode(2));

        //TCP OnOff Client sul nodo n9 che manda dati a n1
        OnOffHelper onOffHelper1 ("ns3::TcpSocketFactory", Address ());
        onOffHelper1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelper1.SetAttribute("PacketSize", UintegerValue(3000));
        AddressValue remoteN1(InetSocketAddress(star.GetSpokeIpv4Address(0), portN1));
        onOffHelper1.SetAttribute("Remote", remoteN1);
        ApplicationContainer node9App = onOffHelper1.Install (csma2Nodes.Get(2));
        node9App.Start (Seconds (3.0));
        node9App.Stop (Seconds (9.0));

        //Udp OnOff Client sul nodo n8 che manda dati a n3
        OnOffHelper onOffHelper2 ("ns3::UdpSocketFactory", Address ());
        onOffHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelper2.SetAttribute("PacketSize", UintegerValue(3000));
        AddressValue remoteN2(InetSocketAddress(star.GetSpokeIpv4Address(2), portN3));
        onOffHelper2.SetAttribute("Remote", remoteN2);
        ApplicationContainer node8App = onOffHelper1.Install (csma2Nodes.Get(1));
        node8App.Start (Seconds (5.0));
        node8App.Stop (Seconds (15.0));

        //abilitazione pcap
        NodeContainer hub = star.GetHub();
        csma1.EnablePcap("task1-2", csma1Devices.Get(1),true); // Pcap su n5
        pointToPoint2.EnablePcap("task1-2", p2pDevices.Get(1),true); // Pcap su n7
        pointToPoint.EnablePcap("task1-2", hub,true); // Pcap su n0
    
        //fine pcap

        //abilitazione ascii tracing
        pointToPoint.EnableAscii("task1-2-1.tr", star.GetSpokeNode(0)->GetDevice(0),true);
        pointToPoint.EnableAscii("task1-2-2.tr", star.GetSpokeNode(1)->GetDevice(0),true);
        pointToPoint.EnableAscii("task1-2-3.tr", star.GetSpokeNode(2)->GetDevice(0),true);
        csma2.EnableAscii("task1-2-8.tr", csma2Devices.Get(1),true);
        csma2.EnableAscii("task1-2-9.tr", csma2Devices.Get(2),true);
        //fine abilitazione ascii tracing


    }
