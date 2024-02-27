# Project : platooning_bando_truck_999
The system that controls three or more vehicles by implementing platooning in a hub to hub environment

## High Level Design
![bando_truck_999_HLD](https://github.com/Intel-Edge-AI-SW-Developers-2nd-Platoon/platooning_bando_truck_999/assets/45201672/42225cf4-90c1-4001-b70e-d1f72436401d)
![image](https://github.com/Intel-Edge-AI-SW-Developers-2nd-Platoon/platooning_bando_truck_999/assets/45201672/474f715d-6610-4f62-a177-c4b9136d9dc8)

## Sequence Diagram

## Contributors

**yeongdaekim(roby238)**
- Project Leader
- Git Manager
- High Level Design
- Interface MPU6050 module(gyroscope) on PACU
- Interface Ethernet module on ECU/PACU
- Interface HC-SR04(Ultra sonic wave) on PACU
- Middleware - CCU server / client(common and follower case)
- Middleware - platoon server / client
- Create Deliverables for Develop

**User3198352(User3198352)**
- Project Manager
- Specify Design
- Interface Motor Driver on ECU
- Implement manual drive with HW and GUI
- Graphic User Interface
- Middleware- CCU client(leader case)
- Create Deliverables for Develop

**simpleis6est(simpleis6est)**
- Documentation
- Interface HC-SR04(Ultra sonic wave) on PACU
- Interface GP2Y0A21(IR) on PACU
- Implement manual drive with HW
- Graphic User Interface
- Try to Integrate FW(Target board STM32F429ZI -> STM32F103C8T6)  
- Create Deliverables for Develop

## Clone code
* Enter below code to run this project.

## Prerequite
* Create new python virtual environment for this project.

## Steps to run
1. How to run with 

## Output

## Commit rules
- Tag?
  ```
  gpg --list-keys
  ```
  
If you don't have any tag, generate it.
- Generate a tag
  ```
  gpg --gen-key
  ```

If you have a tag, set signing tag.
- Set signing tag
  ```
  git config --global user.signingkey XXXXXXXX
  ```
  
- Sign a tag:
  ```
  git tag MYTAG -m 'my signed tag'
  # Creates a signed tag
  ```

- Verify signed tag
  ```
  git tag -v MYTAG
  # Verifies the signed tag
  ```

- Do Signing Commit
  ```
  git commit -S -m "YOUR_COMMIT_MESSAGE"
  # Creates a signed commit
  ```

- Commit Types:
  ```
  - feat 		: add new feature
  - fix 		: fix bug
  - docs 		: modify documents
  - style 	: code formatting, fix no semi-colon(;), no change of code
  - refactor 	: code refactoring
  - test 		: test code, add refactoring test code
  - chore 	: modify build task, package manager
  ```

- Commit Title:
  ```
  - max 50 charactors
  - write in statements instead of in past tense
  - saperate title and body
  - first charactor of title must be upper case
  - if there is issue number, attach it
  ```

- Commit Body(optional):
  ```
  - max 72 charactors in a line
  - not only do you use it for explanations, but also when you write the reason for the commit
  ```

- Commit footer(optional):
  ```
  - issue tracker id
  ```
  ```
  Resolves: #123
  See also: #456, #789
  ```

- Commit Message Format: 
  ```
  Type : Subject
  body(optional)
  footer(optional)
  ```

- Example
  ```
  git commit -S -m "feat: Update title"
  ```

  ```
  git commit --amend
  # Modify your lastest commit message
  ```

  ```
  feat: Update title
  More detailed explanatory text, if necessary. Wrap it to about 72
  characters or so. 
  
  Explain the problem that this commit is solving. Focus on why you
  are making this change as opposed to how (the code explains that).
  
  Further paragraphs come after blank lines.
  
   - Bullet points are okay, too
  
   - Typically a hyphen or asterisk is used for the bullet, preceded
     by a single space, with blank lines in between, but conventions
     vary here
  
  If you use an issue tracker, put references to them at the bottom,
  like this:
  
  Resolves: #123
  See also: #456, #789
  ``` 

## PR rules
