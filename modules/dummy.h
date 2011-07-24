#ifndef DUMMY_H
#define DUMMY_H
using namespace flux_net_irc;
/**
 * \example dummy.h
 * This is an example module showing a simple if-said-that-reply-this situation.
 * \file dummy.h Header file holding the \a dummy function.
 * \author Lordofsraam
 */

/**
 * \defgroup dummyM Dummy (Example) Module
 * This is a template or example module.
 * \section commands Commands associated with this module.
 * \subsection test testing testing
 * Say \a testing \a testing to see if the bot replies with \a I \a hear \a you!
 * @{
 */

/**
 * \fn class dummy(bool a):module("Dummy", a, PRIORITY_LAST){ this->SetDesc("Example Dummy module"); }
 * \brief Replies to a test
 * We will try to put as many examples here in the future as we can.
 */
class dummy : module{

public:
  dummy(bool a):module("Dummy", a, PRIORITY_LAST){ this->SetDesc("Example Dummy module"); }
  
  ModuleReturn run(SendMessage *Send, Flux::string rply, irc_string *reply){
    
    if (reply->said("testing testing")){
      Send->privmsg(reply->channel,"I hear you!");
    }
    
  }
};

/**
 * @}
 */

#endif
