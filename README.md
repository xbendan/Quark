## About

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/d7767659183a4b2b8e317616b1fa1952)](https://app.codacy.com/gh/xbendan/Quark/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)  

> **⚠ Warning** Quark is only a hobby system right now.  

**What is Quark?**  
Quark is a hobby operating system project written in C/C++, aim to run in modern desktop and laptop devices. This repository includes a kernel, runtime libraries, essential applications, and an XML-like UI framework to build UI-based program.  

## Build  
To build this repository, run `build-toolchains.sh` in `/Scripts` to get compilers ready. Then use makefile with `make all PLATFORM=<platform>`, it will compile and build everything for the targeting platform (Platform will default to x86_64 if not specified).  

## Architectures
- [x] x86 [64-bit, x64, amd64] **(In progress)**  
- [ ] risc-v [64-bit]  
*There is no future plan to support 32-bit archs because they are already outdated.*  