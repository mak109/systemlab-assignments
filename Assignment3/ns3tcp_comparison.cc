#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("TcpComparision");

AsciiTraceHelper ascii_trace;
Ptr<PacketSink> cbrSinks[5], tcpDataSink, tcpControlSink;

int total_value;
int total_drops = 0;
bool is_first_drop = true;

// Function to record packet drops
static void
onPacketDrop(Ptr<OutputStreamWrapper> outstream, Ptr<const Packet> p)
{
    if (is_first_drop == true)
    {
        is_first_drop = false;
        *outstream->GetStream() << 0 << " " << 0 << endl;
    }
    else
        *outstream->GetStream() << Simulator::Now().GetSeconds() << " " << ++total_drops << endl;
}

// Function to find the total cumulative recieved bytes
static void
onPacketReceive(Ptr<OutputStreamWrapper> outstream)
{
    total_value = tcpDataSink->GetTotalRx() + tcpControlSink->GetTotalRx();

    for (int i = 0; i < 5; i++)
    {
        total_value += cbrSinks[i]->GetTotalRx();
    }

    *outstream->GetStream() << Simulator::Now().GetSeconds() << " " << total_value << endl;

    Simulator::Schedule(Seconds(0.0001), &onPacketReceive, outstream);
}

// Function to record Congestion Window values
static void
onCwndChange(Ptr<OutputStreamWrapper> outstream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *outstream->GetStream() << Simulator::Now().GetSeconds() << " " << newCwnd << endl;
}

// Trace Congestion window length
static void
traceCwndLen(Ptr<OutputStreamWrapper> outstream)
{
    // Trace changes to the congestion window
    Config::ConnectWithoutContext("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeBoundCallback(&onCwndChange, outstream));
}

int main(int argc, char *argv[])
{
    ns3::LogComponentEnable("TcpComparision", ns3::LOG_LEVEL_INFO);

    bool isTracing = true;
    uint32_t maxBytes = 0;
    string protocol = "TcpWestwood";
    double error_rate = 0.000001;

    // Allow the user to override any of the defaults at
    // run-time, via command-line arguments
    CommandLine cmd;
    cmd.AddValue("tracing", "Flag to enable/disable tracing", isTracing);
    cmd.AddValue("maxBytes",
                 "Total number of bytes for application to send", maxBytes);
    cmd.AddValue("prot", "Transport protocol to use: TcpNewReno, "
                         "TcpHybla, TcpWestwood, TcpScalable, TcpVegas ",
                 protocol);
    cmd.AddValue("error", "Packet error rate", error_rate);

    cmd.Parse(argc, argv);
    string log_comment = "Generating data for Prototype= ";
    log_comment = log_comment + protocol;
    NS_LOG_INFO(log_comment);
    // Tcp Congestion Control Algorithm options
    if (protocol.compare("TcpNewReno") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpNewReno::GetTypeId()));
    }
    else if (protocol.compare("TcpHybla") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHybla::GetTypeId()));
    }
    else if (protocol.compare("TcpVegas") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpVegas::GetTypeId()));
    }
    else if (protocol.compare("TcpScalable") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpScalable::GetTypeId()));
    }

    else if (protocol.compare("TcpWestwood") == 0)
    {
        // the default protocol type in ns3::TcpWestwood is WESTWOOD
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpWestwood::GetTypeId()));
        Config::SetDefault("ns3::TcpWestwood::FilterType", EnumValue(TcpWestwood::TUSTIN));
    }
    else
    {
        NS_LOG_DEBUG("Invalid TCP version");
        exit(1);
    }
    string a_s = "bytes_" + protocol + ".dat";
    string b_s = "drop_" + protocol + ".dat";
    string c_s = "cw_" + protocol + ".dat";

    // Create file streams for data storage
    Ptr<OutputStreamWrapper> total_bytes_data = ascii_trace.CreateFileStream(a_s);
    Ptr<OutputStreamWrapper> dropped_packets_data = ascii_trace.CreateFileStream(b_s);
    Ptr<OutputStreamWrapper> cw_data = ascii_trace.CreateFileStream(c_s);

    // Explicitly create the nodes required by the topology (shown above).
    NS_LOG_INFO("Create nodes.");
    NodeContainer nodes;
    nodes.Create(2);

    NS_LOG_INFO("Create channels.");

    // Explicitly create the point-to-point link required by the topology (shown above).
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));
    p2p.SetQueue("ns3::DropTailQueue");

    NetDeviceContainer devices;
    devices = p2p.Install(nodes);

    // Create error model
    Ptr<RateErrorModel> error_model = CreateObject<RateErrorModel>();
    error_model->SetAttribute("ErrorRate", DoubleValue(error_rate));
    devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(error_model));

    // Install the internet stack on the nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // We've got the "hardware" in place.  Now we need to add IP addresses.
    NS_LOG_INFO("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ipv4Container = ipv4.Assign(devices);

    NS_LOG_INFO("Create Applications.");
    // Create a OnOffHelper and install it on node 0 for ftp control port
    uint16_t ftp_control_port = 21;
    OnOffHelper ftpControlHelper("ns3::TcpSocketFactory", InetSocketAddress(ipv4Container.GetAddress(1), ftp_control_port));
    ftpControlHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    ftpControlHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer ftpControlApp = ftpControlHelper.Install(nodes.Get(0));
    ftpControlApp.Start(Seconds(0.0));
    ftpControlApp.Stop(Seconds(1.8));

    PacketSinkHelper ftpControlSink("ns3::TcpSocketFactory",
                                    InetSocketAddress(Ipv4Address::GetAny(), ftp_control_port));
    ApplicationContainer ftpControlSinkApps = ftpControlSink.Install(nodes.Get(1));

    ftpControlSinkApps.Start(Seconds(0.0));
    ftpControlSinkApps.Stop(Seconds(1.8));

    tcpControlSink = DynamicCast<PacketSink>(ftpControlSinkApps.Get(0));

    // Create a BulkSendApplication and install it on node 0 for ftp data port
    uint16_t ftp_data_port = 20;
    BulkSendHelper ftpDataSource("ns3::TcpSocketFactory", InetSocketAddress(ipv4Container.GetAddress(1), ftp_data_port));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    ftpDataSource.SetAttribute("MaxBytes", UintegerValue(maxBytes));
    ApplicationContainer ftpDataSourceApps = ftpDataSource.Install(nodes.Get(0));
    ftpDataSourceApps.Start(Seconds(0.0));
    ftpDataSourceApps.Stop(Seconds(1.8));

    // Create a PacketSinkApplication and install it on node 1
    PacketSinkHelper ftpDataSink("ns3::TcpSocketFactory",
                                 InetSocketAddress(Ipv4Address::GetAny(), ftp_data_port));
    ApplicationContainer ftpDataSinkApps = ftpDataSink.Install(nodes.Get(1));

    ftpDataSinkApps.Start(Seconds(0.0));
    ftpDataSinkApps.Stop(Seconds(1.8));

    tcpDataSink = DynamicCast<PacketSink>(ftpDataSinkApps.Get(0));

    uint16_t cbrPort = 12345;

    for (int i = 0; i < 5; i++)
    {
        // Install applications: five CBR streams each saturating the channel
        ApplicationContainer cbrApps;
        ApplicationContainer cbrSinkApps;
        OnOffHelper cbrHelper("ns3::UdpSocketFactory", InetSocketAddress(ipv4Container.GetAddress(1), cbrPort + i));
        cbrHelper.SetAttribute("PacketSize", UintegerValue(1024));
        cbrHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        cbrHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        cbrHelper.SetAttribute("DataRate", StringValue("300Kbps"));
        cbrHelper.SetAttribute("StartTime", TimeValue(Seconds(0.2 * (i + 1))));
        if (i < 2)
        {
            cbrHelper.SetAttribute("StopTime", TimeValue(Seconds(1.8)));
        }
        else
        {
            cbrHelper.SetAttribute("StopTime", TimeValue(Seconds(0.8 + i * 0.2)));
        }
        cbrApps.Add(cbrHelper.Install(nodes.Get(0)));
        // Packet sinks for each CBR agent

        PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), cbrPort + i));
        cbrSinkApps = sink.Install(nodes.Get(1));
        cbrSinkApps.Start(Seconds(0.0));
        cbrSinkApps.Stop(Seconds(1.8));
        cbrSinks[i] = DynamicCast<PacketSink>(cbrSinkApps.Get(0));
    }

    devices.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&onPacketDrop, dropped_packets_data));

    // Enable tracing
    if (isTracing)
    {
        AsciiTraceHelper ascii_trace;
        p2p.EnableAsciiAll(ascii_trace.CreateFileStream("tcp-comparision.tr"));
        p2p.EnablePcapAll("tcp-comparision", true);
    }

    NS_LOG_INFO("Run Simulation.");

    Simulator::Schedule(Seconds(0.00001), &onPacketReceive, total_bytes_data);
    Simulator::Schedule(Seconds(0.00001), &traceCwndLen, cw_data);

    // Flow monitor
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(1.8));
    Simulator::Run();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats();
    flowMonitor->SerializeToXmlFile("data.flowmon", true, true);
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
}
