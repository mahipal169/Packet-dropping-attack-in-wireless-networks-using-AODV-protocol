#include "ns3/aodv-module.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/command-line.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/random-variable-stream.h"
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/double.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/mobility-module.h"
#include "myapp.h"
#include <sys/time.h>
#include <iostream>

NS_LOG_COMPONENT_DEFINE ("Packet Dropping Attack");

using namespace ns3;

uint16_t  m_packetSize=1024;
 int       m_NumOfPacket=10;
 int packetCount,received_bits=0,transmitted_bits=0;
float throughput,localThrou,pdf;
float first_transmittedpacket,last_transmittedpacket,sum_of_ete_delay;

int getRandomValue(int min=0, int max=100)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
    //long int sec = tp.tv_sec;
    //std::cout << ms <<" Sec : "<<sec<< std::endl;

 

    RngSeedManager::SetSeed((int)ms);
    Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
    uv->SetAttribute("Min", DoubleValue(min));
    uv->SetAttribute("Max", DoubleValue(max));
    //std::cout<<(int)uv->GetValue()<<std::endl;
    
    return ((int)uv->GetValue());
}

 static void 
 CourseChange (std::string context, Ptr<const MobilityModel> position)
 {
   Vector pos = position->GetPosition ();
   std::cout << Simulator::Now () << ", pos=" << position << ", x=" << pos.x << ", y=" << pos.y
             << ", z=" << pos.z << std::endl;
   //std::cout << packetCount << "\t" << Simulator::Now ().GetSeconds () << "\t" << n->GetId() <<"\n";
 }


void
ReceivePacket(Ptr<const Packet> p, const Address & addr)
{
	
	std::cout << packetCount << "\t" << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() <<"\n";
	//std::cout << packetCount << "\t" << Simulator::Now ().GetSeconds () << "\t" << n->c.GetId() <<"\n";
	
	
	if(packetCount==1)
    {
        first_transmittedpacket=Simulator::Now ().GetSeconds ();
    }

    transmitted_bits+=p->GetSize()*8;
    received_bits+=p->GetSize()*8;
    //std::map<FlowId, FlowMonitor::FlowStats> flowStats1 = monitor->GetFlowStats();

	

    
        last_transmittedpacket=Simulator::Now ().GetSeconds ();
        sum_of_ete_delay=last_transmittedpacket - first_transmittedpacket;

        std::cout<<"*************************************"<<std::endl;
        std::cout<<"Total Sent Packet="<<m_NumOfPacket<<std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout<<"Total Received Packet="<<packetCount<<std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout << "Duration    : " <<Simulator::Now ().GetSeconds ()<< "Seconds" << std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout << "transmitted bits : " <<transmitted_bits<< "bits" << std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout << "received bits : " <<received_bits<< "bits" << std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout << "Throughput    : " << (received_bits)/(Simulator::Now ().GetSeconds () - first_transmittedpacket)/1024 << " Kbps" <<
                  std::endl;

        pdf = (double)packetCount/10;
        std::cout<<"*************************************"<<std::endl;
        std::cout<<"Avereage End to End Delay = "<<sum_of_ete_delay*1000/(packetCount)<<"ms"<<std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout<<"Probablity of  Packet Delivery = "<<pdf<<""<<std::endl;
        std::cout<<"*************************************"<<std::endl;
        std::cout<<"Probablity of packet loss = "<<(1-pdf)<<""<<std::endl;
        std::cout<<"*************************************"<<std::endl;
        packetCount++;
        transmitted_bits=0;
        received_bits=0;
        sum_of_ete_delay=0;
    

}

int main (int argc, char *argv[])
{
  bool enableFlowMonitor = false;
  std::string phyMode ("DsssRate1Mbps");

  CommandLine cmd;
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.Parse (argc, argv);
  std::cout<<(int)getRandomValue(1, 100)<<std::endl; //get the seed value from 1 to 100
  
  Config::SetDefault  ("ns3::OnOffApplication::PacketSize",StringValue ("1040"));
  
  
  NS_LOG_INFO ("Create nodes.");
  
  NodeContainer c; 
  c.Create(21); //creation of number of nodes
  
   NetDeviceContainer devices;
WifiHelper wifi;
 WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (-10) ); 
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());
  devices = wifi.Install (wifiPhy, wifiMac, c); 

 
  wifiPhy.Set ("TxPowerStart", DoubleValue(33));
  wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
  wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
  wifiPhy.Set ("TxGain", DoubleValue(0));
  wifiPhy.Set ("RxGain", DoubleValue(0));
  //wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));
 // wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));
AodvHelper aodv;

InternetStackHelper internet;
internet.SetRoutingHelper (aodv);
internet.Install(c);

Ipv4AddressHelper ipv4;
NS_LOG_INFO ("Assign IP Addresses.");
ipv4.SetBase ("10.0.0.0", "255.255.255.0");
Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);

bool packetdrop=true;
if(packetdrop){    
   Ptr<Node> n1 = c.Get (4);
   Ptr<Node> n2 = c.Get (6);
   Ptr<Node> n3 = c.Get (8);
   Ptr<Node> n4 = c.Get (10);
   Ptr<Node> n5 = c.Get (12);
   Ptr<Node> n6 = c.Get (14);
   Ptr<Node> n7 = c.Get (16);
   Ptr<Node> n8 = c.Get (18);
   Ptr<Node> n9 = c.Get (20);
   Ptr<Node> n10 = c.Get (2);
  Ptr<Node> n11 = c.Get (1);
   Ptr<Node> n12 = c.Get (3);
   Ptr<Node> n13 = c.Get (5);
   Ptr<Node> n14 = c.Get (7); 
   Ptr<Ipv4> ipv41 = n1->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv42 = n2->GetObject<Ipv4> ();
  Ptr<Ipv4> ipv43 = n3->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv44 = n4->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv45 = n5->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv46 = n6->GetObject<Ipv4> ();
  Ptr<Ipv4> ipv47 = n7->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv48 = n8->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv49 = n9->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv410 = n10->GetObject<Ipv4> ();
  Ptr<Ipv4> ipv411 = n11->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv412 = n12->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv413 = n13->GetObject<Ipv4> ();
   Ptr<Ipv4> ipv414 = n14->GetObject<Ipv4> (); 
   uint32_t ipv4ifIndex1 = 1;
    
Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv41, ipv4ifIndex1, false); 
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv41, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv42, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv42, ipv4ifIndex1, true);

Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv43, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv43, ipv4ifIndex1, true);

Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv44, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv44, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv45, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv45, ipv4ifIndex1, true);

Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv46, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv46, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv47, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv47, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv48, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv48, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv49, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv49, ipv4ifIndex1, true);

Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv410, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv410, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv411, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv411, ipv4ifIndex1, true);

Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv412, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv412, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv413, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv413, ipv4ifIndex1, true);


Simulator::Schedule (Seconds (50),&Ipv4::SetForwarding, ipv414, ipv4ifIndex1, false);
Simulator::Schedule (Seconds (55),&Ipv4::SetForwarding, ipv414, ipv4ifIndex1, true);
}




NS_LOG_INFO ("source and destination pair");

  //Connection Between source and Destination node

  uint16_t sinkPort = 9;
  Address sinkAddress (InetSocketAddress (ifcont.GetAddress (20), sinkPort)); 
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (20)); 
  sinkApps.Start (Seconds (0));
  sinkApps.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (c.Get (0), UdpSocketFactory::GetTypeId ()); 

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 1040, 10, DataRate ("250Kbps")); //we are passing packet size 1040 bytes and number of packets are 10
  c.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (2.));
  app->SetStopTime (Seconds (100.)); 
  
   MobilityHelper mobility;
   mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                  "X", StringValue ("100.0"),
                                  "Y", StringValue ("100.0"),
                                  "Rho", StringValue ("ns3::UniformRandomVariable[Min=50|Max=100]"));
   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (c); 
   Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("IP.routes", std::ios::out);
      aodv.PrintRoutingTableAllAt (Seconds (45), routingStream);
   Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback (&CourseChange));
   
    AnimationInterface anim ("FINAL.xml"); 
    anim.EnablePacketMetadata(true);
    
    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback (&ReceivePacket));
    
    AsciiTraceHelper ascii;
  MobilityHelper::EnableAsciiAll (ascii.CreateFileStream ("Final1.tr"));
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("pHYFINAL.tr"));

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(100.0));
  Simulator::Run ();

  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress=="10.0.0.1" && t.destinationAddress == "10.0.0.21"))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      	  std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024  << " Kbps\n";
      }
     }

  monitor->SerializeToXmlFile("FINAL-4.flowmon", true, true);


}
