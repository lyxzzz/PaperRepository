# Paper Repository
***This is a version controll program to maintain a paper repository***

## How to build?
---
#### for client: 
use **paper_repo_client** (this is a windows program)
##### &emsp;&emsp;&emsp;&emsp;if you don't have **g++**, please download.
#### &emsp;&emsp;1. first go into paper_repo_client dir `cd paper_repo_client`
#### &emsp;&emsp;2. then run `mingw32-make`

#### for server: use **paper_repo_server** (this is a linux program)
#### &emsp;&emsp;1. first go into paper_repo_server dir `cd paper_repo_server`
#### &emsp;&emsp;2. then run `make clean` and `make`

## How to run?
---
#### for client: 
use **paper_repo_client** (this is a windows program)
##### &emsp;&emsp;&emsp;&emsp;assume that your server is runing.
#### &emsp;&emsp;1. first go into paper_repo_client dir `cd paper_repo_client`
#### &emsp;&emsp;2. then run `run.bat`

#### for server: use **paper_repo_server** (this is a linux program)
##### &emsp;&emsp;&emsp;&emsp;maybe you'd better run server before you start your client
#### &emsp;&emsp;1. first go into paper_repo_server dir `cd paper_repo_server`
#### &emsp;&emsp;2. then run `./run.sh`

## Parameters
---
#### for client:
&emsp;&emsp;you can change **run.bat** if you want, the parameters list are ***port,ip,repository_dir,config_file***

#### for server:
&emsp;&emsp;you can change **.run** if you want, the parameters list are ***port***