#!/usr/bin/env python3
"""
Simple Python client that implements the same client commands available
in the C++ main.cpp client mode.

Usage:
  ./client.py --address localhost:50051 list
  ./client.py --address localhost:50051 sysinfo
  ./client.py --address localhost:50051 add-array-powermax <name> <symmetrix_id> <dns> <user> <pass>
  ./client.py --address localhost:50051 add-array-powerstore <name> <appliance_id> <dns> <user> <pass>
  ./client.py --address localhost:50051 list-arrays
  ./client.py --address localhost:50051 count
  ./client.py --address localhost:50051 ping [msg]

Defaults:
  address: localhost:50051
"""

import sys
import argparse
import grpc
import time

# Generated protobuf/grpc python modules - make sure these are on PYTHONPATH or installed.
import google.protobuf.empty_pb2 as empty_pb2

from data_shield import common_pb2
from data_shield import common_pb2_grpc
from data_shield import component_block_pb2
from data_shield import component_block_pb2_grpc


DEFAULT_ADDRESS = "localhost:50051"


def list_apis(stub_common, search: str):
    req = common_pb2.ListApisInput()
    if search:
        req.search = search
    resp = stub_common.ListApis(req)
    if resp.apis:
        print("APIs:")
        for a in resp.apis:
            print("  " + a)
    else:
        print("No APIs returned")


def get_sysinfo(stub_common):
    req = empty_pb2.Empty()
    resp = stub_common.GetSystemInfo(req)
    ts = resp.timestamp
    # timestamp may be empty depending on server; print seconds/nanos if present
    ts_secs = ts.seconds if hasattr(ts, "seconds") else None
    print("SystemInfo:")
    if ts_secs is not None:
        print("  timestamp (seconds):", ts_secs)
    print("  uptime (s):", resp.uptime)
    if resp.HasField("ram"):
        print("  ram: total={} used={} free={}".format(resp.ram.total, resp.ram.used, resp.ram.free))
    if resp.HasField("swap"):
        print("  swap: total={} used={} free={}".format(resp.swap.total, resp.swap.used, resp.swap.free))


def register_powermax(stub_comp, name, symmetrix_id, dns, user, password):
    req = component_block_pb2.RegisterPowerMaxInput()
    req.name = name
    req.symmetrix_id = symmetrix_id
    req.dns_name = dns
    cred = req.mgmt_credential
    cred.user_name = user
    cred.password = password

    resp = stub_comp.RegisterPowerMax(req)
    if not resp.success:
        print("RegisterPowerMax failed:", resp.error.message if resp.HasField("error") else "<unknown>")
        return 1
    arr = resp.array
    print("Registered array:", arr.name)
    return 0


def register_powerstore(stub_comp, name, appliance_id, dns, user, password):
    req = component_block_pb2.RegisterPowerStoreInput()
    req.name = name
    req.appliance_id = appliance_id
    req.dns_name = dns
    cred = req.mgmt_credential
    cred.user_name = user
    cred.password = password

    resp = stub_comp.RegisterPowerStore(req)
    if not resp.success:
        print("RegisterPowerStore failed:", resp.error.message if resp.HasField("error") else "<unknown>")
        return 1
    arr = resp.array
    print("Registered array:", arr.name)
    return 0


def list_arrays(stub_comp):
    req = component_block_pb2.ListArraysInput()
    resp = stub_comp.ListArrays(req)
    print("Arrays ({}):".format(len(resp.arrays)))
    for a in resp.arrays:
        print("  name={} dns={}".format(a.name, a.dns_name))


def count_arrays(stub_comp):
    req = component_block_pb2.ListArraysInput()
    resp = stub_comp.ListArrays(req)
    print("Array count:", len(resp.arrays))


def main(argv):
    parser = argparse.ArgumentParser(description="CTA service client (Python)")
    parser.add_argument("--address", "-a", default=DEFAULT_ADDRESS, help="server address (default: %(default)s)")
    parser.add_argument("command", help="client command", nargs=argparse.REMAINDER)
    args = parser.parse_args(argv)

    if not args.command:
        parser.print_help()
        return 2

    # command is list of tokens; emulate main.cpp behavior: allow address to be first param without --address
    # (we already support --address; if user put address as first positional, handle it)
    cmd_tokens = args.command
    # If first token looks like an address (contains ':') treat it as address and shift it.
    if cmd_tokens and (":" in cmd_tokens[0] or "localhost" in cmd_tokens[0] or cmd_tokens[0].startswith("127.")):
        address = cmd_tokens[0]
        cmd_tokens = cmd_tokens[1:]
    else:
        address = args.address

    if not cmd_tokens:
        print("No command provided after address.")
        return 2

    cmd = cmd_tokens[0]
    cmd_args = cmd_tokens[1:]

    # Create channel and stubs
    channel = grpc.insecure_channel(address)
    # Give channel time to connect (optional)
    try:
        grpc.channel_ready_future(channel).result(timeout=5)
    except Exception:
        # proceed anyway; errors will surface on RPC
        pass

    stub_common = common_pb2_grpc.ApiStub(channel)
    stub_comp = component_block_pb2_grpc.ApiStub(channel)

    # Dispatch commands
    if cmd == "list":
        search = cmd_args[0] if cmd_args else ""
        list_apis(stub_common, search)
        return 0

    if cmd == "sysinfo":
        get_sysinfo(stub_common)
        return 0

    if cmd == "add-array-powermax":
        if len(cmd_args) < 5:
            print("add-array-powermax requires 5 args: <name> <symmetrix_id> <dns> <user> <pass>")
            return 2
        return register_powermax(stub_comp, cmd_args[0], cmd_args[1], cmd_args[2], cmd_args[3], cmd_args[4])

    if cmd == "add-array-powerstore":
        if len(cmd_args) < 5:
            print("add-array-powerstore requires 5 args: <name> <appliance_id> <dns> <user> <pass>")
            return 2
        return register_powerstore(stub_comp, cmd_args[0], cmd_args[1], cmd_args[2], cmd_args[3], cmd_args[4])

    if cmd == "list-arrays":
        list_arrays(stub_comp)
        return 0

    if cmd == "count":
        count_arrays(stub_comp)
        return 0

    # ping in the C++ client reused sysinfo as a simple check
    if cmd == "ping":
        msg = cmd_args[0] if cmd_args else "hello"
        # we don't have a Ping RPC; use GetSystemInfo as a healthcheck and echo msg
        get_sysinfo(stub_common)
        print("ping msg:", msg)
        return 0

    print("Unknown client command:", cmd)
    parser.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))