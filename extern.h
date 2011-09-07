#ifndef EXTERN_H
#define EXTERN_H
#include "flux.h"
/* This file is NOT for functions, it is for defining stuff like classes */

/* Classes */
class Channel;
class User;
class SocketIO;
class Commands;
class Command;
class Oper;
class module;
struct CommandSource;

/*  *pointer definitions */
Channel *findchannel(const Flux::string&);
User *finduser(const Flux::string &nick);
module *FindModule(const Flux::string &name);
Command *FindCommand(const Flux::string &name);

/* extern's */
extern SocketIO *sock;
extern SendMessage *Send;
extern Flux::string nick;
extern bool IsValidChannel(const Flux::string&);
extern Flux::string logfile;
extern bool protocoldebug, IsOper;
extern std::vector<Flux::string> StringVector(const Flux::string&, char);
extern std::vector<module*> moduleList;
extern Flux::map<User *> UserNickList;

/* Typedef's */



/* void's */
void ListChans(CommandSource &source);
void ListUsers(CommandSource &source);
void send_cmd(const char *fmt, ...);
void log(const char *fmt, ...);
void process(const Flux::string&);
void ProcessModules(CommandSource&, std::vector<Flux::string>&);

#endif