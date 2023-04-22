# Project FPS Server Setup Guide
## 1. Prerequisite
* Nakama
* [CockroachDB](https://binaries.cockroachdb.com/cockroach-v22.1.8.windows-6.2-amd64.zip) Download, unzip and add path to system environment.

## 2. Nakama migrations
The first time running Nakama, you need run Nakama database migrations.
```
nakama.exe migrate up
```

## 3. Start Cockroach database
```
cockroach.exe start-single-node --insecure --listen-addr=127.0.0.1
```
## 4. Start Nakama server
```
.\nakama.exe
.\nakama.exe --config config.yml > log.txt
```
## 5. Nakama Console
127.0.0.1:7351 \
account: admin \
password: password 
