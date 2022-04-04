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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Node 생성
  NodeContainer nodes;
  nodes.Create (2);

  // Channel 생성
  PointToPointHelper pointToPoint;
  // Channel 값 설정
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // NetDevice 생성
  NetDeviceContainer devices;
  // Node와 NetDevice 연결
  devices = pointToPoint.Install (nodes);

  // internet stack 생성
  InternetStackHelper stack;
  // stack에 node 추가
  stack.Install (nodes);

  // IPv4 주소 생성
  Ipv4AddressHelper address;
  //  Gateway, subnet mask 설정
  address.SetBase ("10.1.1.0", "255.255.255.0");

  // NetDevice에 Interface 등록
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // UDP Server 생성
  UdpEchoServerHelper echoServer (9);

  // UDP Server가 동작하는 Application Node 생성(Node 1번)
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  // Application의 Start 시간 설정
  serverApps.Start (Seconds (1.0));
  // Application의 Stop 시간 설정
  serverApps.Stop (Seconds (10.0));

  // UDP Client 생성
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  // Data 길이 설정(1024)
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  // UDP Client가 동작하는 Application Node 생성(Node 0번)
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  // pcap 파일 생성하기
  pointToPoint.EnablePcapAll ("first_analysis");

  // 시뮬레이션 실행
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}