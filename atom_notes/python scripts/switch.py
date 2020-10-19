]

AclSet = [
    {
        "Name": "switch1:acl_set_ipv4_1",
        "name": "acl_set_ipv4_1",
        "type": "ACL_IPV4",
        "description": "ACL v4 policy for ingress",
    },
    {
        "Name": "switch1:acl_set_ipv4_2",
        "name": "acl_set_ipv4_2",
        "type": "ACL_IPV4",
        "description": "ACL v4 policy for ingress",
    },
    {
        "Name": "switch1:acl_set_ipv4_3",
        "name": "acl_set_ipv4_3",
        "type": "ACL_IPV4",
        "description": "ACL V4 policy for egress"
    },
    {
        "Name": "switch1:acl_set_ipv4_4",
        "name": "acl_set_ipv4_4",
        "type": "ACL_IPV4",
        "description": "ACL V4 policy for egress"
    },
    {
        "Name": "switch1:acl_set_ipv6_1",
        "name": "acl_set_ipv6_1",
        "type": "ACL_IPV6",
        "description": "ACL v6 policy for ingress",
    },
    {
        "Name": "switch1:acl_set_ipv6_2",
        "name": "acl_set_ipv6_2",
        "type": "ACL_IPV6",
        "description": "ACL v6 policy for ingress",
    },
    {
        "Name": "switch1:acl_set_ipv6_3",
        "name": "acl_set_ipv6_3",
        "type": "ACL_IPV6",
        "description": "ACL v6 policy for egress",
    },
    {
        "Name": "switch1:acl_set_ipv6_4,
        "name": "acl_set_ipv6_4",
        "type": "ACL_IPV6",
        "description": "ACL v6 policy for egress",
    }
]

AclEntry = [
    {"Name": "switch1:v4_entry_ACCEPT_1", "sequence-id": 1, "description": "ACL switch1 ACCEPT entry 1",
     "forwarding-action": "ACCEPT", "parent": "switch1:acl_set_ipv4_1", "ipv4": {"source-address": "50.1.1.0/24"}},
    {"Name": "switch1:v4_entry_DROP_1", "sequence-id": 1, "description": "ACL switch1 Drop entry 1",
      "forwarding-action": "DROP", "parent": "switch1:acl_set_ipv4_2", "ipv4": {"source-address": "50.1.1.0/24"}},
    {"Name": "switch1:v4_entry_ACCEPT_2", "sequence-id": 1, "description": "ACL switch1 ACCEPT entry 2",
     "forwarding-action": "ACCEPT", "parent": "switch1:acl_set_ipv4_3", "ipv4": {"source-address": "10.1.1.0/24"}},
    {"Name": "switch1:v4_entry_DROP_2", "sequence-id": 1, "description": "ACL switch1 ACCEPT entry 2",
     "forwarding-action": "DROP", "parent": "switch1:acl_set_ipv4_4", "ipv4": {"source-address": "10.1.1.0/24"}},
    {"Name": "switch1:v6_entry_ACCEPT_1", "sequence-id": 1, "description": "ACL switch1 ACCEPT entry 1",
     "forwarding-action": "ACCEPT", "parent": "switch1:acl_set_ipv6_1", "ipv6": {"source-address": "50:1:1::1:0:0/112"}},
    {"Name": "switch1:v6_entry_DROP_1", "sequence-id": 1, "description": "ACL switch1 Drop entry 1",
     "forwarding-action": "DROP", "parent": "switch1:acl_set_ipv6_2", "ipv6": {"source-address": "50:1:1::1:0:0/112"}},
    {"Name": "switch1:v6_entry_ACCEPT_2", "sequence-id": 1, "description": "ACL switch1 ACCEPT entry 2",
     "forwarding-action": "ACCEPT", "parent": "switch1:acl_set_ipv6_1", "ipv6": {"source-address": "10:1:1::1:0:0/112"}},
    {"Name": "switch1:v6_entry_DROP_1", "sequence-id": 1, "description": "ACL switch1 Drop entry 2",
     "forwarding-action": "DROP", "parent": "switch1:acl_set_ipv6_2", "ipv6": {"source-address": "10:1:1::1:0:0/112"}}
]

IntfAclSet = [
    {"Name": "switch1:acl_intf2_ing_ipv4_ACCEPT_1", "parent": "switch1:bond20", "direction": constants.INGRESS_ACL,
     "type": constants.ACL_IPV4, "set-name": "acl_set_ipv4_1"},
    {"Name": "switch1:acl_intf2_ing_ipv4_DROP_1", "parent": "switch1:bond20", "direction": constants.INGRESS_ACL,
     "type": constants.ACL_IPV4, "set-name": "acl_set_ipv4_2"},
    {"Name": "switch1:acl_intf2_ing_ipv4_ACCEPT_2", "parent": "switch1:bond20", "direction": constants.EGRESS_ACL,
     "type": constants.ACL_IPV4, "set-name": "acl_set_ipv4_3"},
    {"Name": "switch1:acl_intf2_ing_ipv4_DROP_2", "parent": "switch1:bond20", "direction": constants.EGRESS_ACL,
     "type": constants.ACL_IPV4, "set-name": "acl_set_ipv4_4"},
    {"Name": "switch1:acl_intf2_ing_ipv6_ACCEPT_1", "parent": "switch1:bond20", "direction": constants.INGRESS_ACL,
     "type": constants.ACL_IPV6, "set-name": "acl_set_ipv6_1"},
    {"Name": "switch1:acl_intf2_ing_ipv6_DROP_1", "parent": "switch1:bond20", "direction": constants.INGRESS_ACL,
     "type": constants.ACL_IPV6, "set-name": "acl_set_ipv6_2"},
    {"Name": "switch1:acl_intf2_ing_ipv6_ACCEPT_2", "parent": "switch1:bond20", "direction": constants.EGRESS_ACL,
     "type": constants.ACL_IPV6, "set-name": "acl_set_ipv6_1"},
    {"Name": "switch1:acl_intf2_ing_ipv6_DROP_2", "parent": "switch1:bond20", "direction": constants.EGRESS_ACL,
     "type": constants.ACL_IPV6, "set-name": "acl_set_ipv6_2"}
]
