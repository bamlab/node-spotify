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

#include "NodePlaylist.h"
#include "../../exceptions.h"
#include "../../common_macros.h"
#include "../spotify/Track.h"
#include "NodeTrack.h"
#include "NodeUser.h"

NodePlaylist::NodePlaylist(std::shared_ptr<Playlist> _playlist) : playlist(_playlist),
  playlistCallbacksHolder(this, _playlist->playlist) {
}

NodePlaylist::~NodePlaylist() {
  
}

void NodePlaylist::setName(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  String::Utf8Value newName(value->ToString());
  nodePlaylist->playlist->name(*newName);
}

Handle<Value> NodePlaylist::getName(Local<String> property, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  return String::New(nodePlaylist->playlist->name().c_str());
}

void NodePlaylist::setCollaborative(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  nodePlaylist->playlist->setCollaborative(value->ToBoolean()->Value());
  scope.Close(Undefined());
}

Handle<Value> NodePlaylist::getCollaborative(Local<String> property, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  return Boolean::New(nodePlaylist->playlist->isCollaborative());
}

Handle<Value> NodePlaylist::getLink(Local<String> property, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  return String::New(nodePlaylist->playlist->link().c_str());
}

Handle<Value> NodePlaylist::getDescription(Local<String> property, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  return String::New(nodePlaylist->playlist->description().c_str());
}

Handle<Value> NodePlaylist::getTracks(const Arguments& args) {
  HandleScope scope;
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  std::vector<std::shared_ptr<Track>> tracks = nodePlaylist->playlist->getTracks();
  Local<Array> outArray = Array::New(tracks.size());
  for(int i = 0; i < (int)tracks.size(); i++) {
    NodeTrack* nodeTrack = new NodeTrack(tracks[i]);
    outArray->Set(Number::New(i), nodeTrack->getV8Object());
  }
  return scope.Close(outArray);
}

Handle<Value> NodePlaylist::addTracks(const Arguments& args) {
  HandleScope scope;
  if(args.Length() < 2 || !args[0]->IsArray() || !args[1]->IsNumber()) {
    return scope.Close(V8_EXCEPTION("addTracks needs an array and a number as its arguments."));
  }
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  Handle<Array> trackArray = Handle<Array>::Cast(args[0]);
  std::vector<std::shared_ptr<Track>> tracks(trackArray->Length());
  for(unsigned int i = 0; i < trackArray->Length(); i++) {
    Handle<Object> trackObject = trackArray->Get(i)->ToObject();
    NodeTrack* nodeTrack = node::ObjectWrap::Unwrap<NodeTrack>(trackObject);
    tracks[i] = nodeTrack->track;
  }
  int position = args[1]->ToNumber()->IntegerValue();
  nodePlaylist->playlist->addTracks(tracks, position);
  return scope.Close(Undefined());
}

Handle<Value> NodePlaylist::removeTracks(const Arguments& args) {
  HandleScope scope;
  if(args.Length() < 1 || !args[0]->IsArray()) {
    return scope.Close(V8_EXCEPTION("removeTracks needs an array as its first argument."));
  }
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  Handle<Array> trackPositionsArray = Handle<Array>::Cast(args[0]);
  int trackPositions[trackPositionsArray->Length()];
  for(unsigned int i = 0; i < trackPositionsArray->Length(); i++) {
    trackPositions[i] = trackPositionsArray->Get(i)->ToNumber()->IntegerValue();
  }
  try {
    nodePlaylist->playlist->removeTracks(trackPositions, trackPositionsArray->Length());
  } catch(const TracksNotRemoveableException& e) {
    return scope.Close(V8_EXCEPTION("Tracks not removeable, permission denied."));
  }

  return scope.Close(Undefined());
}

Handle<Value> NodePlaylist::reorderTracks(const Arguments& args) {
  HandleScope scope;
  if(args.Length() < 2 || !args[0]->IsArray() || !args[1]->IsNumber()) {
    return scope.Close(V8_EXCEPTION("reorderTracks needs an array and a numer as its arguments."));
  }
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  Handle<Array> trackPositionsArray = Handle<Array>::Cast(args[0]);
  int trackPositions[trackPositionsArray->Length()];
  int newPosition = args[1]->ToNumber()->IntegerValue();
  for(unsigned int i = 0; i < trackPositionsArray->Length(); i++) {
    trackPositions[i] = trackPositionsArray->Get(i)->ToNumber()->IntegerValue();
  }
  try {
    nodePlaylist->playlist->reorderTracks(trackPositions, trackPositionsArray->Length(), newPosition);
  } catch(const TracksNotReorderableException& e) {
    return scope.Close(V8_EXCEPTION(e.message.c_str()));
  }

  return scope.Close(Undefined());
}

Handle<Value> NodePlaylist::isLoaded(Local<String> property, const AccessorInfo& info) {
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  return Boolean::New(nodePlaylist->playlist->isLoaded());
}

Handle<Value> NodePlaylist::getOwner(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(info.Holder());
  Handle<Value> owner;
  if(nodePlaylist->playlist->owner().use_count() > 0) {
    owner = (new NodeUser(nodePlaylist->playlist->owner()))->getV8Object();
  }
  return scope.Close(owner);
}

/**
  Get a field from an object as a persistent function handle. Empty handle if the key does not exist.
**/
static Handle<Function> getFunctionFromObject(Handle<Object> callbacks, Handle<String> key) {
  Handle<Function> callback;
  if(callbacks->Has(key)) {
    callback = Persistent<Function>::New(Handle<Function>::Cast(callbacks->Get(key)));
  }
  return callback;
}

/**
  Set all callbacks for this playlist. Replaces all old callbacks.
**/
Handle<Value> NodePlaylist::on(const Arguments& args) {
  HandleScope scope;
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  if(args.Length() < 1 || !args[0]->IsObject()) {
    return scope.Close(V8_EXCEPTION("on needs an object as its first argument."));
  }
  Handle<Object> callbacks = args[0]->ToObject();
  Handle<String> playlistRenamedKey = String::New("playlistRenamed");
  Handle<String> tracksMovedKey = String::New("tracksMoved");
  Handle<String> tracksAddedKey = String::New("tracksAdded");
  Handle<String> tracksRemovedKey = String::New("tracksRemoved");
  nodePlaylist->playlistCallbacksHolder.playlistRenamedCallback = getFunctionFromObject(callbacks, playlistRenamedKey);
  nodePlaylist->playlistCallbacksHolder.tracksAddedCallback = getFunctionFromObject(callbacks, tracksAddedKey);
  nodePlaylist->playlistCallbacksHolder.tracksMovedCallback = getFunctionFromObject(callbacks, tracksMovedKey);
  nodePlaylist->playlistCallbacksHolder.tracksRemovedCallback = getFunctionFromObject(callbacks, tracksRemovedKey);
  nodePlaylist->playlistCallbacksHolder.setCallbacks();
  return scope.Close(Undefined());
}

Handle<Value> NodePlaylist::off(const Arguments& args) {
  HandleScope scope;
  NodePlaylist* nodePlaylist = node::ObjectWrap::Unwrap<NodePlaylist>(args.This());
  nodePlaylist->playlistCallbacksHolder.unsetCallbacks();
  return scope.Close(Undefined());
}

void NodePlaylist::init() {
  HandleScope scope;
  Local<FunctionTemplate> constructorTemplate = FunctionTemplate::New();
  constructorTemplate->SetClassName(String::NewSymbol("Playlist"));
  constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "on", on);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "off", off);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), getName, setName);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("collaborative"), getCollaborative, setCollaborative);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("link"), getLink);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("description"), getDescription);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("isLoaded"), isLoaded);
  constructorTemplate->InstanceTemplate()->SetAccessor(String::NewSymbol("owner"), getOwner);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getTracks", getTracks);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "addTracks", addTracks);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "removeTracks", removeTracks);
  NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "reorderTracks", reorderTracks);

  constructor = Persistent<Function>::New(constructorTemplate->GetFunction());
  scope.Close(Undefined());
}