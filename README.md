# MPI-VIC: Distributed Hydrological Model Acceleration Tool

## Overview

This project implements an enhanced tool for accelerating the computational performance of the Variable Infiltration Capacity (VIC) hydrological model using MPI (Message Passing Interface) parallel computing and GFS (Global File System) technology in distributed computing environments.

### Key Features

- **Spatial Decomposition**: Automatic partitioning of VIC model grids across multiple computing nodes
- **Parallel Processing**: MPI-based parallel execution for simultaneous multi-grid calculations
- **Result Synthesis**: GFS-enabled aggregation of simulation results from distributed nodes
- **Real-time Performance**: Significantly enhanced computational efficiency for complex hydrological models

## System Requirements

### Hardware Requirements
- Multiple computing nodes (minimum 2 nodes recommended)
- Network connectivity between all nodes
- Sufficient storage space for VIC model data and results
- RAM: Minimum 4GB per node (8GB+ recommended)

### Software Requirements
- Linux operating system (Ubuntu 18.04+ or CentOS 7+ recommended)
- GCC compiler with C support
- Root or sudo access for system-level installations

## Installation and Configuration

### 1. GlusterFS Installation and Configuration

#### 1. Machine Preparation
| IP | Hostname | Role |
|---|---|---|
| 172.16.254.81 | dell81 | GFS Server, Client |
| 172.16.254.89 | s206 | GFS Server, Client |
| 172.16.254.90 | s207 | GFS Server, Client |
| 172.16.254.91 | s208 | GFS Server, Client |

#### 2. Installation
Install using yum by executing the following commands:
```bash
$ yum install -y centos-release-gluster
$ yum install -y glusterfs glusterfs-server 
$ yum install -y glusterfs-fuse glusterfs-rdma
```

#### 3. Deployment Configuration
Execute the following commands on host dell81:
```bash
gluster peer probe s206
gluster peer probe s207
gluster peer probe s208
```

After executing the above commands, check the cluster status:
```bash
gluster pool list
```

Create a data storage directory on each node:
```bash
mkdir -p /home/mesos/glusterfs/gfs1 
```

Create the GFS volume:
```bash
gluster volume create gfs1 replica 4 dell81:/home/mesos/glusterfs/gfs1 s206:/home/mesos/glusterfs/gfs1 s207:/home/mesos/glusterfs/gfs1 s208:/home/mesos/glusterfs/gfs1
```

Start the volume:
```bash
gluster volume start gfs1
```

Check the volume status:
```bash
gluster volume info gfs1
```

Mount the volume by executing the following commands on each node:
```bash
mkdir -p /dis_cloud
```

Execute the mount commands:
```bash
mount.glusterfs dell81:/home/mesos/glusterfs/gfs1 /dis_cloud  # Execute on dell81
mount.glusterfs s206:/home/mesos/glusterfs/gfs1 /dis_cloud  # Execute on s206
mount.glusterfs s207:/home/mesos/glusterfs/gfs1 /dis_cloud  # Execute on s207
mount.glusterfs s208:/home/mesos/glusterfs/gfs1 /dis_cloud  # Execute on s208
```

### 2. MPI Environment Configuration

#### 2.1 Download Source Code
Visit the MPICH official website (mpich.org) and download the latest source code tarball. In the terminal, use the following command to download the source code:
```bash
v=3.2
wget http://www.mpich.org/static/downloads/${v}/mpich-${v}.tar.gz
```

#### 2.2 Extract the Source Code
In the terminal, use the following command to extract the downloaded file:
```bash
tar –xzf mpich-${v}.tar.gz
cd mpich-${v}
export MPICH2_3_2_DIR="/Download Path/mpich-${v}"
./configure --prefix=$MPICH2_3_2_DIR
```

#### 2.3 Compile and Install
```bash
make
sudo make install
```

#### 2.4 Set Environment Variables
Once installed, you need to set environment variables to use MPICH. Add the following lines to your `.bashrc` or `.bash_profile`:

```bash
export PATH=/path/to/install/bin:$PATH
export LD_LIBRARY_PATH=/path/to/install/lib:$LD_LIBRARY_PATH
```

### 3. MPI-VIC Model Deployment

#### 3.1 Download and Compile MPI-VIC Model

```bash
# Clone the MPI-VIC model repository
cd /dis_cloud
git clone https://github.com/JinfengM/MPI-VIC.git
cd MPI-VIC

# Compile MPI-VIC for your system
make 

# Check if MPI-VIC.X exists
ls MPI-VIC.X

# Make the executable accessible to all nodes
chmod +x MPI-VIC.X
```

#### 3.2 Prepare MPI-VIC Input Data

```bash
# Download input data from the website on host dell81
# Go to https://github.com/JinfengM/VIC-Borg/tree/main/example,
# Navigate to the example directory and download example.tar.gzaa and example.tar.gzab

# Unzip the example files 'example.tar.gz*' to obtain the 'run_lh' directory
cat example.tar.gz* | tar -xzv
tar -zxvf example_files.tar.gz

# Move the run_lh directory to host dell81: /dis_cloud directory
# Check if run_lh exists
ls /dis_cloud/run_lh
```

#### 3.3 Edit MPI-VIC Input Data

```bash
# Navigate to the MPI-VIC directory
cd /dis_cloud

# Check if chanliu_input.txt exists
ls chanliu_input.txt 

# Update chanliu_input.txt, replace "/home/dell/mjf/run_dql" with "/dis_cloud/run_lh"

# Note: /dis_cloud/run_lh directory is the current MPI-VIC input data directory
```

## Usage

### 1. Basic Execution

```bash
# Navigate to the shared directory
cd /dis_cloud

# Run MPI-VIC with parameters
mpirun -np 12 ./MPI-VIC.X -g chanliu_input.txt \
    0.910654 0.436423 24.482024 0.502105 0.988413 0.413380
```

### 2. Parameter Explanation

The MPI-VIC program accepts the following command line arguments:

```bash
./MPI-VIC.X [program_name] -g [config_file] [param1] [param2] [param3] [param4] [param5] [param6]
```

Where:
- `config_file`: Path to the VIC global parameter file; here we use chanliu_input.txt
- `param1-param6`: Six floating-point parameters for VIC model calibration,
-  namely infilt, Ds, Dsmax, Ws, depth[1], and depth[2]

### 3. Monitoring Execution

```bash
# Monitor MPI processes
ps aux | grep MPI-VIC.X

# Check GlusterFS filesystem usage
df -h /dis_cloud

# Monitor network traffic between nodes
iftop -i eth0
```

### 4. Result Collection

Results are automatically synchronized across all nodes via GlusterFS. Check the results directory:

```bash
ls -la /dis_cloud/run_lh/chanliu_result
```
### 5. Advanced Execution Using hostfile
mpirun -np 24 --hostfile hostfile ./MPI-VIC.X -g chanliu_input.txt \
    0.910654 0.436423 24.482024 0.502105 0.988413 0.413380
## Troubleshooting

### Common Issues

#### 1. MPI Communication Errors

```bash
# Test MPI connectivity
mpirun -np 2 --hostfile hostfile hostname

# Check firewall settings
sudo ufw status
sudo systemctl status firewalld
```

#### 2. GlusterFS Mount Issues

```bash
# Check GlusterFS volume status
gluster volume status gfs1

# Check GlusterFS peers
gluster peer status

# Remount the GlusterFS filesystem
sudo umount /dis_cloud
mount.glusterfs dell81:/home/mesos/glusterfs/gfs1 /dis_cloud
```

#### 3. Memory Issues

```bash
# Monitor memory usage
free -h
htop

# Adjust the number of MPI processes
mpirun -np 24 --hostfile hostfile ./MPI-VIC.X [parameters]
```
