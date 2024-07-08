# ft_ping

## Overview

`ft_ping` is a custom implementation of the classic `ping` (inetutils-2.0) command, designed to test the accessibility and measure the round-trip time (RTT) of messages sent to another machine over an IP network.

## Features

-   Basic functionality of the `ping` command.
-   Handles IPv4 addresses and hostnames.
-   Supports `-?` , `-v`, `--ttl`, `-l`, `-q`, `-c`, and `-w` options.


## Installation
 To build and install `ft_ping`, follow these steps: 
 1. **Clone the repository**: ```git clone https://github.com/mbrettsc/ft_ping.git && cd ft_ping``` 
 2. **Build the project**: ```make```
    
## Usage 
Run the `ft_ping` executable with the desired options and target: ```bash ./ft_ping [options] [destination]
