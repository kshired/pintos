# Sogang Univ - Pintos Project

서강대학교 운영체제(CSE4070) 수업의 프로젝트인 Pintos 프로젝트의 구현 소스코드 및 보고서를 저장하는 Github 저장소입니다.

## 목차
- [Project #1](#project-1--userprog-1)
- [Project #2](#project-2--userprog-2)
- [Project #3](#project-3--threads)
- [Project #4](#project-4--virtual-memory)
- [Project #5](#project-5--file-system)
- [주의 사항](#주의-사항)


## Project #1 : UserProg (1)
- Argument Passing
- User Memory Access
- System Calls
- Additional System Calls ( fibonacci, max_of_four_int )

**모두 구현 완료**


[보고서 및 소스코드](https://github.com/kshired/pintos/blob/main/os_prj1_20171612.tar.gz)

## Project #2 : UserProg (2)
- File Descriptor
- System Calls ( filesystem )
- Synchronization in Filesystem

**모두 구현 완료**

[보고서 및 소스코드](https://github.com/kshired/pintos/blob/main/os_prj2_20171612.tar.gz)

## Project #3 : Threads
- Alarm Clock
- Priority Scheduling
- Advanced Scheduler

**모두 구현 완료**

> 참고: Priority Donation은 구현 범위가 아니라 구현하지 않았습니다.

[보고서 및 소스코드](https://github.com/kshired/pintos/blob/main/os_prj3_20171612.tar.gz)

## Project #4 : Virtual Memory

- Page Table and Page Fault Handler
- Disk Swap
- Stack Growth

**Paget Fault Handler 일부분 + Stack Growth 구현**

> 참고 : 테스트 결과 16개 중 11개 통과

[보고서 및 소스코드](https://github.com/kshired/pintos/blob/main/os_prj4_20171612.tar.gz)

## Project #5 : File System

- Buffer Cache
- Extensible File & File Growth
- Subdirectory

**모두 구현 완료**

테스트 결과 

> 참고 : 프로젝트5는 프로젝트4가 전부 구현되지 않아도 작동이 가능합니다. 하지만, 프로젝트3까지는 구현이 되어 있어야합니다.

[보고서 및 소스코드](https://github.com/kshired/pintos/blob/main/os_prj5_20171612.tar.gz)


## 주의 사항

이 코드와 보고서를 그대로 사용하면, 카피 체크에 걸릴 수 있습니다. ( 카피체크에 걸릴시 책임드리지 못합니다. )

참고용으로만 사용하고 실제 구현은 [Pintos Manual](https://web.stanford.edu/class/cs140/projects/pintos/pintos.pdf) 및 [KAIST Pintos 강의자료](https://oslab.kaist.ac.kr/ee415-spring-2020/)를 참고하여 직접 구현하는 것을 추천드립니다.

