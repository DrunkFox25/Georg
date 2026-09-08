Thanks for contributing!

# Guidelines

- You can use a language of your choice, but be prepared to explain it to me and help me with integrating it into the build process. If you write C code I will respect you, but I cannot live without meathods and vectors.

- If you don't understand my code, message me. The more platforms you message me on the higher chance I respond, but please don't spam.

- If you want for convinience a new libary in this code, please ask me. Also be prepared to teach me the library and don't forget to tell me what license it has, so I can judge.

- Do not edit the cmake file without permission, and if you do, make sure # nice is on line 69.

- If you write debug code, thank you. Please NEVER delete debug code, instead we will only remove it for releases.

- Any funny programming related videos can be commented anywhere in the codebase.

- Please label unused code, and if an entire file is, then label it with a comment in the first line and consider moving it to unused. Do not do this for code that someone is planning to use.

- We will be using the latest stable C++ compiler, as why not, we aint launching rockets.

# AI

- You can use AI, I would prefer Gemini but I don't care, although the in github codespace copilot is goated too, however the condition is you must understand all code written, or be using a new library in which case the code will be overturned soon enough to where it will only be temporary that you don't understand.

- [Great Video](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

# Style Guide

- All debug messages go to Log

- Please don't write shit code, and if you must write temporary code, please write a comment there about it. Even todo optimize is good.

- While I don't recommend commenting on things that don't have to be changed/isn't external documentation.

- If you use a single letter variable (or double letter most of the time) in a way not approved by me, you will have bad things happen to you. Honestly, don't do it unless I did it, although I am actively trying to remove where I did. See the section approve variable names with few letters.

- While you can use namespace std temporarly, do not push code with it in a header file, and if such code is in a cpp file please try to remove it with time. I don't want everybody spreading stds. Also beware of a namespace collision if you do as QT's emit macro collides with std::emit().

- Don't use endl, use \\n + flush.

- Don't use pragma once, use:

    - #ifndef PROJECT_PATH_FILENAME_H/HPP_
    - #define PROJECT_PATH_FILENAME_H/HPP_
    - #endif // PROJECT_PATH_FILENAME_H/HPP_

- All code must be platform independant, no #ifs even, if you would need some, some libraries already done it, so ask me to add it.

- Voodoo magic code is approved, but please put comments labeling it.

- Please do not write optimized code that sacrifices readability if -O3 would do it anyway.

- Please don't use depricated or unrecommened features of anything without my approval.

- I will find you if you use a single letter lowercase variable name for a vector. You do not need the extra time, trust me, I've got EJOI gold without it.

- I will be very disgusted with you as a person if you write code that looks like shit and do not plan on fixing it.

- Macros are for pussies (except for .h clauses)

- This is a threat.

- [Important Video](https://www.youtube.com/watch?v=cwq2FfpTIAE)

# Aproved variable names with few letters

- i for index, but use index if it is a member variable
- x,y,z,w for position, but don't overuse in a way to make collision
- v for QJsonValue if no other option
- ob for QJsonObject if no other option
- pq for any priority_queue
- str for string
- m for map though this one I don't like
- vars, vals, var, val but I realy want to get rid of these