# chobits os visual studio 2022 port

개발자를 위한 나만의 운영체제 만들기 서적에서 소개한
chobits os를 비쥬얼 스튜디오 2022에서 빌드 가능하도록 작업한
프로젝트입니다. 

## 테스트
* 릴리즈탭에서 chobits.ima를 다운받는다.
* VMWare 가상 에뮬레이터를 실행하고 가상 머신을 만든다음 플로피 디스크 장치를 추가한다.
* 플로피 디스크 장치에 chobits.ima를 추가한 다음 부팅한다.

### 주의
QEMU나 VirtualBox 등에서는 플로피 디스크 드라이버 문제로 정상 동작하지 않는다.
