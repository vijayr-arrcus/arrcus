#!/usr/bin/python
from scapy.all import *
from scapy.contrib.ospf import *
import codecs


def main (argv):
    print "Enter hex input"
    in_hex = raw_input()
    pkt_hex = in_hex[208:]
    pkt_dump = codecs.decode(pkt_hex, 'hex')
    pkt = Ether(pkt_dump)
    pkt.show()
    meta_hex = in_hex[:208]
    print meta_hex


if __name__ == "__main__":
    main(sys.argv[1:])
