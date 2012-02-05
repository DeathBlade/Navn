/* All code is licensed under GNU General Public License GPL v3 (http://www.gnu.org/licenses/gpl.html) */
#include <channel.h>

Flux::insensitive_map<Channel*> ChanMap;
Channel::Channel(const Flux::string &nname, time_t ts){
  if(nname.empty())
    throw CoreException("I don't like empty channel names in my channel constructor >:d");
  if(!IsValidChannel(nname))
    throw CoreException("An Invalid channel was passed into the Channel constructor :<");
  
  this->name = nname;
  this->creation_time = ts;
  this->topic_time = 0;
  this->SendWho();
  ChanMap[this->name] = this;
  Log(LOG_DEBUG) << "Created new channel: " << nname;
}
Channel::~Channel()
{
  this->SendPart();
  Log(LOG_DEBUG) << "Deleted channel " << this->name;
  ChanMap.erase(this->name);
}
User *Channel::finduser(const Flux::string &usr)
{
  Flux::insensitive_map<User*>::iterator it1 = UserNickList.find(usr);
  User *u = it1->second;
  if(!u)
    return NULL;
  UList::iterator it = UserList.find(u);
  if(it != UserList.end())
    return it->first;
  return NULL;
}
void Channel::SendJoin(){ ircproto->join(this->name); }
void Channel::SendPart(){ ircproto->part(this->name); }
void Channel::AddUser(User *u) { if(u) this->UserList[u] = this; }
void Channel::DelUser(User *u)
{
  UList::iterator it = UserList.find(u);
  if(it != UserList.end())
    UserList.erase(it);
}
void Channel::SendPart(const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->SendPart(Flux::string(buffer));
    va_end(args);
  }
}
void Channel::SendPart(const Flux::string &reason){ ircproto->part(this->name, reason); }
void Channel::kick(User *u, const Flux::string &reason){ u->kick(this->name, reason); }
void Channel::kick(User *u, const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->kick(u, Flux::string(buffer));
    va_end(args);
  }
}
void Channel::kick(const Flux::string &u, const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->kick(u, Flux::string(buffer));
    va_end(args);
  }
}
void Channel::kick(const Flux::string &u, const Flux::string &reason){ ircproto->kick(this->name, u, reason); }
void Channel::SetMode(const Flux::string &mode){
 if(mode[0] == '+'){
   ircproto->mode(this->name, mode);
 }else{
   mode == '+' + mode;
   ircproto->mode(this->name, mode);
 }
}
void Channel::SetMode(User *u, const Flux::string &mode){
 if(mode[0] == '+'){
   ircproto->mode(this->name, mode, u->nick);
 }else{
   mode == '+' + mode;
   ircproto->mode(this->name, mode, u->nick);
 }
}
void Channel::RemoveMode(const Flux::string &mode){
  if(mode[0] == '-'){
    ircproto->mode(this->name, mode);
  }else{
    
    mode == '-' + mode;
    ircproto->mode(this->name, mode);
  }
}
void Channel::RemoveMode(User *u, const Flux::string &mode){
  if(mode[0] == '-'){
    ircproto->mode(this->name, mode, u->nick);
  }else{
    mode == '-' + mode;
    ircproto->mode(this->name, mode, u->nick);
  }
}
void Channel::ChangeTopic(const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->ChangeTopic(Flux::string(buffer));
    va_end(args);
  }
}
void Channel::ChangeTopic(const Flux::string &topicstr){ ircproto->topic(this->name, topicstr); }
void Channel::SendMessage(const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->SendMessage(Flux::string(buffer));
    va_end(args);
  }
}
void Channel::SendMessage(const Flux::string &message){ ircproto->privmsg(this->name, message); }
void Channel::SendAction(const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->SendAction(Flux::string(buffer));
    va_end(args);
  }
}
void Channel::SendAction(const Flux::string &message) { ircproto->action(this->name, message); }
void Channel::SendNotice(const char *fmt, ...){
  if(fmt){
    char buffer[BUFSIZE] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    this->SendNotice(Flux::string(buffer));
    va_end(args);
  }
}
void Channel::SendNotice(const Flux::string &message){ ircproto->notice(this->name, message); }
void Channel::SendWho(){ ircproto->who(this->name); }
/****************************************************************/
void QuitUser(User *u)
{
  if(!u)
    return;
#ifdef _CXX11
    for(auto var : ChanMap)
      for(auto var1 : var.second->UserList)
	if(var1.first == u)
	  var1.second->DelUser(u);
#else
  for(Flux::insensitive_map<Channel*>::iterator it = ChanMap.begin(), it_end = ChanMap.end(); it != it_end; ++it)
    for(UList::iterator it1 = it->second->UserList.begin(), it1_end = it->second->UserList.end(); it1 != it1_end; ++it1)
    {
      if(it1->first == u)
	it1->second->DelUser(u);
    }
#endif
    delete u;
}
void ListChans(CommandSource &source){
  Flux::string channels;
#ifdef _CXX11
  for(auto var : ChanMap)
    channels += var.second->name+' ';
#else
  for(Flux::map<Channel*>::iterator it = ChanMap.begin(), it_end = ChanMap.end(); it != it_end; ++it){
    Channel *ch = it->second;
    channels += ch->name+' ';
  }
#endif
  channels.trim();
  source.Reply("Channels: %s\n", channels.c_str());
}
Channel *findchannel(const Flux::string &channel){
  Flux::map<Channel *>::iterator it = ChanMap.find(channel);
  if(it != ChanMap.end())
    return it->second;
  return NULL;
}