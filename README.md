# Project : platooning_bando_truck_999
The system that controls three or more vehicles by implementing platooning in a hub to hub environment

## High Level Design
![bando_truck_999_HLD](https://github.com/Intel-Edge-AI-SW-Developers-2nd-Platoon/platooning_bando_truck_999/assets/45201672/42225cf4-90c1-4001-b70e-d1f72436401d)

## Sequence Diagram
![Whole Sequence Diagram drawio](https://github.com/roby238/BoardMaestro/assets/45201672/b28c0dff-38d2-4396-9cba-d74d72cf9517)

## Contributors

**yeongdaekim(roby238)**
- Project Leader
- Git Manager
- Expression Calculating
- Create Deliverables for Develop

**User3198352(User3198352)**
- Project Manager
- 
- Hand Pose Estimaion
- Create Deliverables for Develop

**Judy(jjooeun)**
- Image Preprocessing
- Create Deliverables for Develop
  
**simpleis6est(simpleis6est)**
- Image Preprocessing
- Create Deliverables for Develop

  

## Clone code

* Enter below code to run this project.

```shell
git clone https://github.com/Intel-Edge-AI-SW-Developers-2nd-Team-1/BoardMaestro.git
```

## Prerequite

* Create new python virtual environment for this project.

```shell
# Create virtual env.
python3 -m venv board_maestro

# Activate virtual env.
source board_maestro/bin/activate

# Install requirements.
pip install -r requirements.txt
```

## Steps to run

1. How to run with python file.

2. How to run with bash file

## Output
  
## Commit rules
- Sign a tag:
  ```
  git tag -s MYTAG
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
