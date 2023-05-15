#!/bin/bash

# Run the first command
./ns3 run "scratch/ns3tcp_comparison --error=0.001 --prot=TcpNewReno"

# Add a space
echo

# Run the second command
./ns3 run "scratch/ns3tcp_comparison --error=0.001 --prot=TcpHybla"

# Add a space
echo

# Run the third command
./ns3 run "scratch/ns3tcp_comparison --error=0.001 --prot=TcpWestwood"

# Add a space
echo

# Run the fourth command
./ns3 run "scratch/ns3tcp_comparison --error=0.001 --prot=TcpScalable"

# Add a space
echo

# Run the fifth command
./ns3 run "scratch/ns3tcp_comparison --error=0.001 --prot=TcpVegas"

# Add a space
echo

# Run the Python script to generate plots
echo "Running plots.py..."
python3 plots.py >> /dev/null 2>&1

