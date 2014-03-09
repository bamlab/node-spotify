/**
The MIT License (MIT)

Copyright (c) <2013> <Moritz Schulze>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/

#ifndef _NODESPOTIFY_EXCEPTIONS_H
#define _NODESPOTIFY_EXCEPTIONS_H

#define CREATE_EXCEPTION_WITH_MESSAGE(name) class name : public ExceptionWithMessage {\
  public:\
    name(const char* message) : ExceptionWithMessage(message) {};\
  };

#include <exception>

class ExceptionWithMessage {
public:
  ExceptionWithMessage(const char* _message) : message(_message) {};
  std::string message;
};
class FileException : public std::exception {};
class TrackNotPlayableException : public std::exception {};
class PlaylistCreationException : public std::exception {};
class PlaylistNotDeleteableException : public std::exception {};
class TracksNotRemoveableException : public std::exception {};

CREATE_EXCEPTION_WITH_MESSAGE(PlaylistNotMoveableException)
CREATE_EXCEPTION_WITH_MESSAGE(TracksNotReorderableException)
CREATE_EXCEPTION_WITH_MESSAGE(SessionCreationException)

#endif