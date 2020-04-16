from arctest.base import constants
from arctest.base.RobotFramework import RobotFramework
from collections import OrderedDict

default_packet_sampling_rate = 100000
default_counter_sampling_interval = 60
vm_ip_list = ["10.27.202.31", "10.27.202.34", "10.27.202.35"]
other_vm_ip_list = ["10.27.202.31", "10.27.202.34"]
loopback_ipv4_list = {"201.0.0.1", "201.0.0.2", "201.0.0.3"}

sampling_rate = 39
swp_interface1 = "swp7"
swp_interface2 = "swp7"
intf_lists = [swp_interface1, swp_interface2]
swp1="swp1"
src_agent_ip = ["ma1", "ma1", "ma1"]
user_name = "user"
password = "Arrcus2018"
collector_ip = "10.9.209.199"
ni_mgmt = "management"
switch_subintf = ["switch1:swp7", "switch2:swp7", "switch2:swp1s1", "switch3:swp2"]
dir_in="ingress"

other_switches = ["switch1", "switch2"]
switchlist = ["switch1", "switch2", "switch3"]
sflowlist = ["switch1:sflow", "switch2:sflow", "switch3:sflow"]
other_sflowlist = ["switch1:sflow", "switch2:sflow"]

sflowIntflist = ["switch1:sflowIntf", "switch2:sflowIntf", "switch3:sflowIntf"]
other_sflowIntflist = ["switch1:sflowIntf", "switch2:sflowIntf"]

sflowIntflist1 = ["switch1:sflowIntf1", "switch2:sflowIntf1", "switch3:sflowIntf1"]
other_sflowIntflist1 = ["switch1:sflowIntf1", "switch2:sflowIntf1"]
other_sflowIntflist1_egress = ["switch1:sflowIntfEgress", "switch2:sflowIntfEgress"]

ipv4Collectors = ["switch1:sflowipv4collector", "switch2:sflowipv4collector", "switch3:sflowipv4collector"]
ipv4Collectors1 = ["switch1:sflowipv4collector1", "switch2:sflowipv4collector1", "switch3:sflowipv4collector1"]
other_ipv4Collectors = ["switch1:sflowipv4collector", "switch2:sflowipv4collector"]
other_ipv4Collectors1 = ["switch1:sflowipv4collector1", "switch2:sflowipv4collector1"]

ipv6Collectors = ["switch1:sflowipv6collector", "switch2:sflowipv6collector", "switch3:sflowipv6collector"]
ipv6Collectors1 = ["switch1:sflowipv6collector1", "switch2:sflowipv6collector1", "switch3:sflowipv6collector1"]
other_ipv6Collectors = ["switch1:sflowipv4collector", "switch2:sflowipv6collector"]
other_ipv6Collectors1 = ["switch1:sflowipv6collector1", "switch2:sflowipv6collector1"]

allowed_ip_ma1 = ["10.27.202.31", "10.27.202.34", "10.27.202.35"]
allowed_ip_swp = ["172.16.1.4", "172.16.1.8", "172.16.1.5", "172.16.1.9"]
allowed_ip_swp_v6 = ["172.16.1.1", "172.16.2.1", "172.16.1.2", "172.16.2.2"]
ip = "10.27.202.35"
Switch = [
    {"Name": "switch1", "host": "10.9.126.246"},
    {"Name": "switch2", "host": "10.9.127.113"},
    {"Name": "switch3", "host": "10.9.127.114"},
]

Intf = [
    {"Name": "switch1:intf1", "type": constants.TYPE_CSMACD, "name": "swp1", "enabled": True,
     },
    {"Name": "switch1:intf2", "type": constants.TYPE_CSMACD, "name": "swp2", "enabled": True,
     },
    {"Name": "switch2:intf1", "type": constants.TYPE_CSMACD, "name": "swp1", "enabled": True,
     },
    {"Name": "switch2:intf2", "type": constants.TYPE_CSMACD, "name": "swp2", "enabled": True,
     },
    {"Name": "switch3:intf1", "type": constants.TYPE_CSMACD, "name": "swp1", "enabled": True,
     },
    {"Name": "switch3:intf2", "type": constants.TYPE_CSMACD, "name": "swp2", "enabled": True,
     },
    {"Name": "switch1:LOOPBACK", "type": constants.TYPE_LOOPBACK, "name": "loopback0", "enabled": True},
    {"Name": "switch2:LOOPBACK", "type": constants.TYPE_LOOPBACK, "name": "loopback0", "enabled": True},
    {"Name": "switch3:LOOPBACK", "type": constants.TYPE_LOOPBACK, "name": "loopback0", "enabled": True},
]

SubIntf = [{"Name": "switch1:intf1subintf0", "index": 0, "parent": "switch1:intf1"},
           {"Name": "switch1:intf2subintf0", "index": 0, "parent": "switch1:intf2"},
           {"Name": "switch2:intf1subintf0", "index": 0, "parent": "switch2:intf1"},
           {"Name": "switch2:intf2subintf0", "index": 0, "parent": "switch2:intf2"},
           {"Name": "switch3:intf1subintf0", "index": 0, "parent": "switch3:intf1"},
           {"Name": "switch3:intf2subintf0", "index": 0, "parent": "switch3:intf2"},
           {"Name": "switch1:Loopbacksubintf0", "index": 0, "enabled": True, "parent": "switch1:LOOPBACK"},
           {"Name": "switch2:Loopbacksubintf0", "index": 0, "enabled": True, "parent": "switch2:LOOPBACK"},
           {"Name": "switch3:Loopbacksubintf0", "index": 0, "enabled": True, "parent": "switch3:LOOPBACK"},
           ]

Intfv4Addr = [
    {"Name": "switch1:intf1_ipv4", "ip": "172.16.1.4", "prefix-length": "31", "index": 0,
     "parent": "switch1:intf1subintf0"},
    {"Name": "switch2:intf1_ipv4", "ip": "172.16.1.5", "prefix-length": "31", "index": 0,
     "parent": "switch2:intf1subintf0"},


    {"Name": "switch1:intf2_ipv4", "ip": "172.16.1.8", "prefix-length": "31", "index": 0,
     "parent": "switch1:intf2subintf0"},
    {"Name": "switch3:intf1_ipv4", "ip": "172.16.1.9", "prefix-length": "31", "index": 0,
     "parent": "switch3:intf1subintf0"},

    {"Name": "switch2:intf2_ipv4", "ip": "172.16.1.6", "prefix-length": "31", "index": 0,
     "parent": "switch2:intf1subintf0"},
    {"Name": "switch3:intf2_ipv4", "ip": "172.16.1.7", "prefix-length": "31", "index": 0,
     "parent": "switch3:intf1subintf0"},

    {"Name": "switch1:Loopbacksubintf0_ipv4", "ip": "201.0.0.1", "prefix-length": "32", "index": 0,
     "parent": "switch1:Loopbacksubintf0"},
    {"Name": "switch2:Loopbacksubintf0_ipv4", "ip": "201.0.0.2", "prefix-length": "32", "index": 0,
     "parent": "switch2:Loopbacksubintf0"},
    {"Name": "switch3:Loopbacksubintf0_ipv4", "ip": "201.0.0.3", "prefix-length": "32", "index": 0,
     "parent": "switch3:Loopbacksubintf0"},
]

NI = [
    {"Name": "switch1:ni", "name": "default"},
    {"Name": "switch2:ni", "name": "default"},
    {"Name": "switch3:ni", "name": "default"},
]

SFlowGbl = [
    {
        "Name": "switch1:sflow", "counter-sampling-interval": 60, "packet-sampling_rate": 100000,
    },
    {
        "Name": "switch2:sflow", "counter-sampling-interval": 60, "packet-sampling-rate": 100000
    },
    {
        "Name": "switch3:sflow", "counter-sampling-interval": 60, "packet-sampling-rate": 100000
    },
    {
        "Name": "switch1:sflow1", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "source-interface": "swp2"
    },
    {
        "Name": "switch2:sflow1", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "source-interface": "swp2"
    },
    {
        "Name": "switch3:sflow1", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "source-interface": "swp2"
    },

    {
        "Name": "switch1:sflow2", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "network-instance": "management", "source-interface": "ma1"
    },
    {
        "Name": "switch2:sflow2", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "network-instance": "management", "source-interface": "ma1"
    },
    {
        "Name": "switch3:sflow2", "counter-sampling-interval": 60, "packet-sampling-rate": 100000,
        "network-instance": "management", "source-interface": "ma1"
    }

]

SFlowIntf = [
    {"Name": "switch1:sflowIntf", "name": "swp1", "direction": "ingress", "packet-sampling-rate": "39"},
    {
        "Name": "switch1:sflowIntf1", "name": "ma1", "direction": "ingress", "packet-sampling-rate": "38"
    },
    {
        "Name": "switch1:sflowIntfEgress", "name": "swp2", "direction": "egress", "packet-sampling-rate": "39"
    },
    {
        "Name": "switch2:sflowIntf", "name": "ma1", "direction": "ingress", "packet-sampling-rate": "39"
    },
    {
        "Name": "switch2:sflowIntf3", "name": "swp1", "direction": "ingress", "packet-sampling-rate": "19"
    },
    {
        "Name": "switch2:sflowIntfEgress", "name": "swp2", "direction": "egress", "packet-sampling-rate": "39"
    },

    {
        "Name": "switch3:sflowIntf", "name": "swp1", "direction": "ingress", "packet-sampling-rate": "39"
    },
    {
        "Name": "switch3:sflowIntf1", "name": "ma1", "direction": "ingress", "packet-sampling-rate": "38"
    },
    {
        "Name": "switch3:sflowIntfEgress", "name": "swp2", "direction": "egress", "packet-sampling-rate": "39"
    },

]

Ipv4Collector = [
    {
        "Name": "switch1:sflowipv4collector", "address": "10.9.208.50", "parent": "switch1:sflow", "port": 6343
    },
    {
        "Name": "switch2:sflowipv4collector", "address": "10.9.208.50", "parent": "switch2:sflow", "port": 6343
    },
    {
        "Name": "switch3:sflowipv4collector", "address": "10.9.208.50", "parent": "switch3:sflow", "port": 6343
    },
    {
        "Name": "switch1:sflowipv4collector1", "address": "10.9.208.50", "parent": "switch1:sflow2", "port": 7000
    },
    {
        "Name": "switch2:sflowipv4collector1", "address": "10.9.208.50", "parent": "switch2:sflow2", "port": 7000
    },
    {
        "Name": "switch3:sflowipv4collector1", "address": "10.9.208.50", "parent": "switch3:sflow2", "port": 7000
    },
]

# Network to be built!
Network = [
    {"Name": "BGPv4Network01",
     "objects": ["switch1", "switch2", "switch3",
                 "switch1:intf1subintf0","switch2:intf1subintf0", "switch3:intf1subintf0",
                 "switch1:intf2subintf0", "switch2:intf2subintf0",  "switch3:intf2subintf0",
                 "switch1:LOOPBACK", "switch2:LOOPBACK","switch2:LOOPBACK",
                 "switch1:Loopbacksubintf0", "switch2:Loopbacksubintf0", "switch3:Loopbacksubintf0",
                 "switch1:Loopbacksubintf0_ipv4", "switch2:Loopbacksubintf0_ipv4", "switch3:Loopbacksubintf0_ipv4",
                 #"switch1:brkout1",
                 "switch1:intf1_ipv4",
                 "switch1:intf2_ipv4",
                 "switch2:intf1_ipv4",
                 "switch3:intf1_ipv4",
                 "switch1:sflow", "switch2:sflow", "switch3:sflow",
                 "switch1:sflowIntfEgress", "switch2:sflowIntfEgress", "switch3:sflowIntfEgress",
                 "switch1:sflowipv4collector", "switch2:sflowipv4collector","switch2:sflowipv4collector",
                 ]
     }
]

# Mapping of object names to classes
ord = OrderedDict([("Switches", Switch), ("Intf", Intf), ("SubIntf", SubIntf),
                   ("Intfv4Addr", Intfv4Addr), ("NI", NI),
                   ("SFlowGbl", SFlowGbl), ("SFlowIntf", SFlowIntf), ("Ipv4Collector", Ipv4Collector),
                   ("Network", Network)])

tfw = RobotFramework(ord)
