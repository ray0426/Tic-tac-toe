# Computer Network Project Phase 2
## About
This project is about a simple implementation of HTTP server which can host a profile page. Clients can easily browse it by simply entering the ip address on the web browsers, including `Chrome`, `Edge` and `FireFox`.  ( Do not support `Safari`)\
GitHub Repository URL: [https://github.com/ray0426/Tic-tac-toe](https://github.com/ray0426/Tic-tac-toe) 

## Our Project IP Adderss
Our project is deloyed on the below ip address.

```
http://60.251.128.233:48763
```

## Compile and Execute
### Step 0
Make sure that you are under the directory of our project. 

### Step 1
Modify the ip address and port number in line 13 and 14 in `main.cpp`.

```c++
#define PORT 8000
#define HOST "0.0.0.0"
```

### Step 2
Compile and run the server. Run below commands on your terminal. 

```bash
$ g++ main.cpp
$ ./a.out
```

### Step 3
Open your web browser and enter your ip address and port number on `Chrome`, `Edge` or `FireFox`. Then you can browse the webpage. 

## Demo
This is what our page look like.

[![Demo Video] (https://drive.google.com/file/d/1ZTZZK-RbhgxNzteOVDA2jnUpRTEiuT9u/preview)](https://drive.google.com/file/d/15lyMjopYzNLPRqalIy9j1eI02Aun5lDj/preview "Demo Video")

## Architecture
### Server
The following file is about the server. 

```bash
phase2
└─── main.cpp	// the server
```

### Profile Page
The following files are about the profile page.

```bash
phase2
├─── index.html	    // html
├─── board.html     // html
├─── file.html      // html
├─── profile.css	// css
├─── laybkgnd.gif	// background
├─── 346496.jpg		// my photo
├─── intro.mp4          // video
└─── librispeech.flac   // sound
```


## Functions

### A Website with Videoes, Sounds and Images
- Our server provides the `GET` request of multiple types of files from clients, including `.html`, `.css`, `.gif`, `.jpg`, `.mp4` and `.flac`. 
- Restriction: Cannot pull the video/sound progress bar on `Chrome` and `Edge`.
### Message Board
- We implement the response of `POST` request in our server. The message left by the visitors can be recorded.  
- Restriction: Can only enter english alpabet and numbers.

## Contributers

* B08901211 電機四 游耿睿
* B08901215 電機四 蕭淇元
